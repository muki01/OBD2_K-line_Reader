#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    //deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    //deviceConnected = false;
    delay(500);
    pServer->startAdvertising();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    rxValue.erase(std::remove(rxValue.begin(), rxValue.end(), '\n'), rxValue.end());
    rxValue.erase(std::remove(rxValue.begin(), rxValue.end(), '\r'), rxValue.end());
    std::string speedStr = String(SPEED).c_str();
    std::string rpmStr = String(RPM).c_str();
    std::string coolTempStr = String(COOLANT_TEMP).c_str();
    std::string inTempStr = String(INTAKE_TEMP).c_str();

    if (rxValue == "SPEED") {
      pTxCharacteristic->setValue(speedStr);
      pTxCharacteristic->notify();
    } else if (rxValue == "RPM") {
      pTxCharacteristic->setValue(rpmStr);
      pTxCharacteristic->notify();
    } else if (rxValue == "COOLANT_TEMP") {
      pTxCharacteristic->setValue(coolTempStr);
      pTxCharacteristic->notify();
    } else if (rxValue == "INTAKE_TEMP") {
      pTxCharacteristic->setValue(inTempStr);
      pTxCharacteristic->notify();
    }
  }
};

void init_BLE() {
  BLEDevice::init("OBD2 Master");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
}
