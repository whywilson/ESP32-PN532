#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define TXD_PIN 17 // GPIO1 (TX)
#define RXD_PIN 16 // GPIO3 (RX)

#define BLE_NAME "PN532-BLE"
#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_NOTIFY "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_WRITE "0000ffe2-0000-1000-8000-00805f9b34fb"

BLECharacteristic *pNotifyCharacteristic;
bool deviceConnected = false;
String receivedData = "";

// BLE server callbacks
class CustomBLEServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

// BLE write callback
class WriteCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.print("PN532 <- ");
      for (size_t i = 0; i < value.length(); i++)
      {
        Serial.printf("%02X ", (uint8_t)value[i]);
      }
      Serial.println();

      // Forward BLE write data to Serial1 TX
      Serial1.write((uint8_t *)value.data(), value.length());
    }
  }
};

bool activatePN532()
{
  std::vector<uint8_t> wakeUpCommand = {0x55, 0x55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0};
  Serial1.write(wakeUpCommand.data(), wakeUpCommand.size()); // Use Serial1 to write data

  delay(200);
  if (Serial1.available())
  {
    std::vector<uint8_t> bleDataBuffer;
    while (Serial1.available())
    {
      uint8_t data = Serial1.read();
      bleDataBuffer.push_back(data);
    }

    Serial.print("PN532 -> ");
    for (const auto &byte : bleDataBuffer)
    {
      Serial.printf("%02X ", byte); // Use Serial to print log in HEX
    }
    Serial.println();

    if (!bleDataBuffer.empty())
    {
      return true;
    }
  }
  else
  {
    Serial.println("No response from PN532"); // Use Serial to print log
  }
  return false;
}

void setup()
{
  Serial.begin(115200); // Initialize Serial for debugging
  Serial.println("Starting BLE work!");
  Serial1.begin(115200, SERIAL_8N1, RXD_PIN, TXD_PIN); // Initialize UART for PN532 communication
  
  // Initialize BLE
  BLEDevice::init(BLE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new CustomBLEServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Notify characteristic
  pNotifyCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_NOTIFY,
      BLECharacteristic::PROPERTY_NOTIFY);
  pNotifyCharacteristic->addDescriptor(new BLE2902());

  // Write characteristic
  BLECharacteristic *pWriteCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_WRITE,
      BLECharacteristic::PROPERTY_WRITE);
  pWriteCharacteristic->setCallbacks(new WriteCallback());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE service started!");

  // Activate PN532
  if (activatePN532())
  {
    Serial.println("PN532 activated successfully");
  }
  else
  {
    Serial.println("Failed to activate PN532");
  }
}

void loop()
{
  // Read data from Serial1 RX and send it via BLE notify
  if (Serial1.available())
  {
    std::vector<uint8_t> bleDataBuffer;
    while (Serial1.available())
    {
      uint8_t data = Serial1.read();
      bleDataBuffer.push_back(data);
    }

    Serial.print("PN532 -> ");
    for (const auto &byte : bleDataBuffer)
    {
      Serial.printf("%02X ", byte); // Use Serial to print log in HEX
    }
    Serial.println();

    // Send data to BLE notify characteristic
    pNotifyCharacteristic->setValue(bleDataBuffer.data(), bleDataBuffer.size());
    pNotifyCharacteristic->notify();
  }

  delay(10); // Small delay to avoid overwhelming the loop
}