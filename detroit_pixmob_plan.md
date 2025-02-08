# Detroit Sports Teams PixMob ESPHome Implementation Plan

## Overview
Convert web-based PixMob IR transmitter to ESPHome for Home Assistant integration, creating buttons for Detroit sports teams light shows.

## Hardware Requirements
- ESP8266/ESP32 board
- IR LED (connected to GPIO4)
- Transistor circuit for IR LED (as per IRremoteESP8266 wiki)

## ESPHome Component Structure

### 1. Base Configuration
```yaml
esphome:
  name: detroit_pixmob
  platform: ESP8266
  board: d1_mini

# Required ESPHome components
api:
external_components:
  - source: github://esphome/esphome@dev
    components: [remote_transmitter]

# Configure IR LED output
remote_transmitter:
  pin: GPIO4
  carrier_duty_percent: 50%
```

### 2. Custom Components Needed
1. IR Pattern Sender:
   - Convert bit patterns to timing sequences
   - Handle IR transmission
   - Support different effects and tail codes

2. Team Light Show Service:
   - Package effects into themed sequences
   - Handle timing between effects
   - Support different animation patterns

### 3. Home Assistant Integration
Create buttons for each team that trigger predefined light shows:

1. Detroit Lions
   - Primary: Light Blue sequences (LIGHT_BLUE variants)
   - Secondary: White effects (WHITISH variants)
   - Special: Alternating blue/white for touchdowns

2. Detroit Tigers  
   - Primary: Navy Blue effects (BLUE variants)
   - Secondary: Orange effects (ORANGE variants)
   - Special: Alternating pattern for home runs

3. Detroit Red Wings
   - Primary: Red effects (RED variants)
   - Secondary: White effects (WHITISH variants)
   - Special: Alternating pattern for goals

4. Detroit Pistons
   - Primary: Red and Blue effects
   - Secondary: White effects
   - Special: Three-color sequence for big plays

5. University of Michigan
   - Primary: Maize effects (YELLOW variants)
   - Secondary: Blue effects (BLUE variants)
   - Special: Alternating pattern for touchdowns/baskets

6. Michigan State
   - Primary: Green effects (GREEN variants)
   - Secondary: White effects (WHITISH variants)
   - Special: Alternating pattern for touchdowns/baskets

## Implementation Steps

1. ESPHome Setup
   - Create base configuration
   - Test IR transmission
   - Validate GPIO connections

2. Custom Component Development
   - Port IR pattern conversion logic from script.js
   - Create light show sequences for each team
   - Implement service calls for Home Assistant integration

3. Home Assistant Integration
   - Create button entities
   - Design card layout
   - Add button actions

4. Testing & Validation
   - Verify IR patterns match original implementation
   - Test each team's light show sequence
   - Validate Home Assistant integration

## Technical Details

### IR Pattern Format
Convert arrays of 1s and 0s to timing sequences:
```python
# Example conversion
[1, 1, 0, 0] -> [2, 2]  # 2 ones, 2 zeros
```

### Light Show Sequences
Each team's light show will cycle through:
1. Primary color effect (3 seconds)
2. Secondary color effect (3 seconds)
3. Special effect sequence (5 seconds)

### Error Handling
- Validate IR patterns before transmission
- Monitor IR LED temperature
- Implement retry logic for failed transmissions

## Next Steps
1. Create initial ESPHome configuration file
2. Test base IR transmission
3. Develop custom components
4. Create Home Assistant integration