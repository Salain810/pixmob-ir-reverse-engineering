#!/usr/bin/env python3

import os
import sys
import getpass

def create_secrets_file():
    """Create the secrets.yaml file with user input"""
    print("\nPixMob ESPHome Configuration Setup")
    print("==================================")
    
    wifi_ssid = input("\nEnter your WiFi SSID: ")
    wifi_password = getpass.getpass("Enter your WiFi password: ")
    ota_password = getpass.getpass("Enter a password for OTA updates: ")
    
    secrets_content = f"""# WiFi credentials
wifi_ssid: "{wifi_ssid}"
wifi_password: "{wifi_password}"

# OTA updates password
ota_password: "{ota_password}"

# Fallback AP password
fallback_password: "pixmob-backup"
"""
    
    with open("secrets.yaml", "w") as f:
        f.write(secrets_content)
    
    print("\nSecrets file created successfully!")

def verify_files():
    """Verify all required files exist"""
    required_files = [
        "detroit_pixmob.yaml",
        "secrets.yaml",
        "pixmob_ir.h"
    ]
    
    missing = []
    for file in required_files:
        if not os.path.exists(file):
            missing.append(file)
    
    if missing:
        print("\nError: Missing required files:")
        for file in missing:
            print(f"- {file}")
        sys.exit(1)

def print_next_steps():
    """Print instructions for next steps"""
    print("""
Next Steps:
===========

1. Install ESPHome if you haven't already:
   pip install esphome

2. Connect your ESP8266/ESP32 board to your computer

3. Flash the firmware:
   esphome run detroit_pixmob.yaml

4. Add to Home Assistant:
   The device should automatically be discovered by Home Assistant
   If not, add it manually using the IP address shown in the ESPHome logs

5. Test the buttons:
   You should now see buttons for each Detroit sports team in Home Assistant
   Each button will trigger a unique light show sequence

Troubleshooting:
===============
- Ensure the IR LED is properly connected to GPIO4 with the correct transistor circuit
- Check the ESPHome logs for any transmission errors
- Verify the WiFi credentials are correct
- Test with a phone camera to see if the IR LED is flashing

For more information, see:
https://github.com/your-username/detroit-pixmob-esphome
""")

def main():
    # Create secrets file if it doesn't exist or user wants to update it
    if not os.path.exists("secrets.yaml"):
        create_secrets_file()
    else:
        update = input("\nSecrets file already exists. Do you want to update it? (y/N): ")
        if update.lower() == 'y':
            create_secrets_file()
    
    # Verify all files exist
    verify_files()
    
    print("\nAll required files are present!")
    
    # Print next steps
    print_next_steps()

if __name__ == "__main__":
    main()