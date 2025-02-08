#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"

namespace esphome {

// IR Pattern definitions ported from effects_definitions.js
static const std::map<std::string, std::vector<int>> base_color_effects = {
    {"RED", {1,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"RED_2", {1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"LIGHT_BLUE", {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,1,0,1,1,0,0,1,0,0,0,1}},
    {"LIGHT_BLUE_2", {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,0,1}},
    {"BLUE", {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1}},
    {"BLUE_2", {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1}},
    {"GREEN", {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"GREEN_2", {1,1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"WHITISH", {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,0,1}},
    {"YELLOW", {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"YELLOW_2", {1,1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"ORANGE", {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1}}
};

static const std::map<std::string, std::vector<int>> tail_codes = {
    {"FADE_1", {0,0,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"FADE_2", {0,0,1,0,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1}},
    {"FADE_3", {0,0,0,1,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1}}
};

class PixMobComponent : public Component {
public:
    PixMobComponent() = default;

    void setup() override {
        // Initialize the IR transmitter
        this->transmitter_ = this->get_remote_transmitter();
    }

    // Convert bit array to timing array (ported from script.js bits_to_run_lengths_pulses)
    std::vector<int> bits_to_run_lengths(const std::vector<int>& bits) {
        std::vector<int> run_lengths;
        int current_count = 0;
        int current_bit = -1;

        for (int bit : bits) {
            if (bit != current_bit) {
                if (current_bit != -1) {
                    run_lengths.push_back(current_count);
                }
                current_count = 1;
                current_bit = bit;
            } else {
                current_count++;
            }
        }

        if (current_bit != -1) {
            run_lengths.push_back(current_count);
        }

        return run_lengths;
    }

    // Send a single IR pattern
    void send_pattern(const std::string& effect, const std::string& tail = "") {
        std::vector<int> pattern;
        
        // Get main effect pattern
        auto main_it = base_color_effects.find(effect);
        if (main_it == base_color_effects.end()) {
            ESP_LOGE("PixMob", "Invalid effect name: %s", effect.c_str());
            return;
        }
        pattern = main_it->second;

        // Add tail code if specified
        if (!tail.empty()) {
            auto tail_it = tail_codes.find(tail);
            if (tail_it == tail_codes.end()) {
                ESP_LOGE("PixMob", "Invalid tail code: %s", tail.c_str());
                return;
            }
            pattern.insert(pattern.end(), tail_it->second.begin(), tail_it->second.end());
        }

        // Convert to run lengths
        auto run_lengths = bits_to_run_lengths(pattern);

        // Verify run lengths are valid
        for (int length : run_lengths) {
            if (length > 9) {
                ESP_LOGE("PixMob", "Invalid run length: %d (must be ≤ 9)", length);
                return;
            }
        }

        // Convert to timing array (each unit is 700µs)
        std::vector<uint16_t> timings;
        bool is_mark = true;  // Start with mark (1)
        for (int length : run_lengths) {
            timings.push_back(length * 700);
            is_mark = !is_mark;
        }

        // Send the pattern
        auto transmit = this->transmitter_->transmit();
        transmit.set_carrier_frequency(38000);  // 38 kHz
        transmit.space(0);  // No leading space
        
        for (size_t i = 0; i < timings.size(); i++) {
            if (i % 2 == 0) {
                transmit.mark(timings[i]);
            } else {
                transmit.space(timings[i]);
            }
        }
        
        transmit.send();
        delay(3);  // Small delay between transmissions
    }

    // Play a sequence of patterns
    void play_sequence(const std::vector<std::pair<std::string, std::string>>& sequence) {
        for (const auto& pattern : sequence) {
            send_pattern(pattern.first, pattern.second);
            delay(3000);  // 3 second delay between patterns
        }
    }

protected:
    remote_transmitter::RemoteTransmitterComponent *transmitter_{nullptr};

    remote_transmitter::RemoteTransmitterComponent *get_remote_transmitter() {
        auto *transmitter = App.get_remote_transmitter_component();
        if (transmitter == nullptr) {
            ESP_LOGE("PixMob", "No remote transmitter configured!");
        }
        return transmitter;
    }
};

}  // namespace esphome