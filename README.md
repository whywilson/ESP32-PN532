# ESP32-PN532 BLE Reader

This project transforms an ESP32 into a BLE module for the PN532 NFC reader, enabling the entire device to function as a PN532 BLE reader. With this setup, the MTools BLE App can connect to the device for seamless NFC operations.

## Wiring Diagram 
The default wiring for the ESP32 WROOM32
- GPIO 17 (TX2) to PN532 RX
- GPIO 16 (RX2) to PN532 TX
- GND to PN532 GND
- VCC to PN532 VCC  

![EPS32-PN532](assets/connection.jpg)
Or you can change **TXD_PIN** and **RXD_PIN** in the code to use other GPIO pins.

## Features
- Use the ESP32 as a BLE bridge for the PN532.
- Compatible with the MTools BLE App for NFC interactions.
- Supports both iOS and Android platforms.

## RFID Protocols
- ISO14443A
- Felica

## MTools BLE App
Download the MTools BLE App to connect and interact with the PN532 BLE reader:

- [Download on the App Store](https://apps.apple.com/hk/app/mtools-ble-rfid-reader/id1531345398)
- [Download on Google Play](https://play.google.com/store/apps/details?id=com.mtoolstec.mtoolsLite)

## Requirements
- ESP32 development board.
- PN532 NFC module under HSU (High Speed UART) mode.

## Getting Started
Follow the instructions in this repository to set up and configure your ESP32-PN532 BLE reader.