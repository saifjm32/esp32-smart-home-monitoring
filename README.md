# ESP32 Smart Home Monitoring (DHT22 + LDR + ThingSpeak + IFTTT)

## Overview
Smart home sensor monitoring system using an ESP32 to read:
- Temperature via DHT22
- Light status via LDR
Data is transmitted over WiFi to ThingSpeak for real-time visualization, and an IFTTT email alert is triggered if the environment stays dark for a configured duration.

## Hardware
- ESP32
- DHT22 (GPIO 27)
- LDR Digital Output (GPIO 26)
- LED (GPIO 14)

## Cloud / Services
- ThingSpeak (data logging + charts)
- IFTTT Webhooks (darkness alert email)

## Project Structure
- `firmware/esp32.ino` — ESP32 firmware
- `paper/` — project report PDF

## Setup
1. Open `firmware/esp32.ino` in Arduino IDE
2. Install library: **DHT sensor library**
3. Select ESP32 board + correct COM port
4. Edit credentials:
   - `WIFI_SSID`, `WIFI_PASS`
   - `THINGSPEAK_API_KEY`
   - `IFTTT_EVENT`, `IFTTT_KEY`
5. Upload to ESP32

## Behavior
- Reads temperature + humidity from DHT22
- Reads light state from LDR (configured as HIGH = DARK)
- Turns LED ON if temperature < 26°C
- Sends IFTTT email once if it remains dark for the configured time window
