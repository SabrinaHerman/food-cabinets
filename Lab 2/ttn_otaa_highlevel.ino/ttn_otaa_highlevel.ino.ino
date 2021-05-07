/*
  Module:  simple_feather.ino
  Function:
        Example app matching the documentation in the project
  README.md, showing how to configure a board explicitly
  Copyright notice and License:
        See LICENSE file accompanying this project.
  Author:
        Terry Moore, MCCI Corporation November 2018
        Alex Coy, ECE 4950 TA for Spring 2021
  Notes:
  This app is not complete -- it only presents skeleton
  code for the methods you must provide in order to
  use this library. However, it compiles!
*/
#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#define FILLMEIN (#Don't edit this stuff. Fill in the appropriate FILLMEIN values.)
#warning "You must fill in your keys with the right values from the TTN control panel"
#endif
#include <Arduino_LoRaWAN_ttn.h>
#include "keys.h"
#include <Adafruit_FRAM_I2C.h>
#include <Adafruit_BME280.h>
Adafruit_FRAM_I2C FRAM     = Adafruit_FRAM_I2C();
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25)
uint64_t lastTime = 0;
static uint8_t messageBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
#ifdef __cplusplus
extern "C"{
#endif
void myStatusCallback(void * data, bool success){
  if(success)
    Serial.println("Succeeded!");
  else
    Serial.println("Failed!");
}
#ifdef __cplusplus 
}
#endif
class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
  public:
    cMyLoRaWAN() {};
  protected:
    // you'll need to provide implementations for each of the following.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    virtual void NetSaveFCntUp(uint32_t uFCntUp) override;
    virtual void NetSaveFCntDown(uint32_t uFCntDown) override;
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    virtual bool GetSavedSessionInfo(
      SessionInfo *pSessionInfo,
      uint8_t *pExtraSessionInfo,
      size_t nExtraSessionInfo,
      size_t *pnExtraSessionActual
    ) override;
    // you should provide a function that returns provisioning info from
    // persistent storage. Called during initialization. If this returns
    // false, OTAA will be forced. If this returns true (as it should for
    // a saved session), then a call with a non-null pointer will get teh
    // filled-in provisioning info.
    virtual bool GetAbpProvisioningInfo(
      AbpProvisioningInfo *pProvisioningInfo
    );
};
// set up the data structures.
cMyLoRaWAN myLoRaWAN {};
// The pinmap. This form is convenient if the LMIC library
// doesn't support your board and you don't want to add the
// configuration to the library (perhaps you're just testing).
// This pinmap matches the FeatherM0 LoRa. See the arduino-lmic
// docs for more info on how to set this up.
const cMyLoRaWAN::lmic_pinmap myPinMap = {
  .nss = 8,
  .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = { 3, 6, cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN },
  .rxtx_rx_active = 0,
  .rssi_cal = 0,
  .spi_freq = 8000000,
};
void setup() {
  // simply pass the pinmap to the begin() method.
  Serial.begin(115200);
  int b = FRAM.begin();
  //Make sure serial has started properly?
  delay(3000);
  //This makes the board freeze: poor optimization?
  //while(!Serial);
  if (b) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C FRAM");
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?\r\n");
    Serial.println("Will continue in case this processor doesn't support repeated start\r\n");
  }

  unsigned bstatus;
    
  // default settings
  bstatus = bme.begin();  
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!bstatus) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
  }
  
  while (!Serial);
  myLoRaWAN.begin(myPinMap);
  lastTime = millis();
  Serial.println("Serial begin");
  if (myLoRaWAN.IsProvisioned())
    Serial.println("Provisioned for something");
  else
    Serial.println("Not provisioned.");
  myLoRaWAN.SendBuffer(messageBuffer, 4, myStatusCallback, NULL, false, 1);
}

void loop() {
  myLoRaWAN.loop();
  if (millis() - lastTime > 60000) {
    float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.println(alt);
    int integer = (int)(alt);
    int dec = 100 * (alt - integer);
    Serial.println(integer);
    Serial.println(dec);
    messageBuffer[0] = integer;
    messageBuffer[1] = dec;
    myLoRaWAN.SendBuffer(messageBuffer, 4, myStatusCallback, NULL, false, 1);
    lastTime = millis();
  }
}
// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
  OtaaProvisioningInfo *pInfo
) {
  if (pInfo) {
    memcpy_P(pInfo->AppEUI, APPEUI, 8);
    memcpy_P(pInfo->DevEUI, DEVEUI, 8);
    memcpy_P(pInfo->AppKey, APPKEY, 16);
  }
  return true;
}
void
cMyLoRaWAN::NetSaveFCntDown(uint32_t uFCntDown) {
  SessionInfo info;
  if(GetSavedSessionInfo(&info, nullptr, 0, nullptr)){
    info.V2.FCntDown = uFCntDown;
    NetSaveSessionInfo(info, nullptr, 0);
  }
}
void
cMyLoRaWAN::NetSaveFCntUp(uint32_t uFCntUp) {
  SessionInfo info;
  if(GetSavedSessionInfo(&info, nullptr, 0, nullptr)){
    info.V2.FCntUp = uFCntUp;
    NetSaveSessionInfo(info, nullptr, 0);
  }
}
void
cMyLoRaWAN::NetSaveSessionInfo(
  const SessionInfo &Info,
  const uint8_t *pExtraInfo,
  size_t nExtraInfo
) {
  // save Info somewhere.
  int i;
  uint8_t * infoBytes;
  Serial.println("Print all SessionInfo bytes to the console");
  infoBytes = (uint8_t *) &Info;
  // write tag 'info' at address 0
  FRAM.write8(0, 'i');
  FRAM.write8(1, 'n');
  FRAM.write8(2, 'f');
  FRAM.write8(3, 'o');
  for (i = 0; i < sizeof(SessionInfo); i++) {
    Serial.print(infoBytes[i], HEX);
    FRAM.write8(i + 4, infoBytes[i]);
  }
  Serial.println();
  Serial.println("Print just the NwkSKey");
#define SKEY_SIZE 16
  infoBytes = (uint8_t *) & (Info.V2.NwkSKey);
  for (i = 0; i < SKEY_SIZE; i++) {
    Serial.print(infoBytes[i], HEX);
  }
  Serial.println();
}
bool cMyLoRaWAN::GetSavedSessionInfo(
  SessionInfo *pSessionInfo,
  uint8_t *pExtraSessionInfo,
  size_t nExtraSessionInfo,
  size_t *pnExtraSessionActual
) {
  //Your implementation goes here
  if (!pSessionInfo) {
    return false;
  } else {
    // check if tag "info" is there
    if (FRAM.read8(0) == 'i' && FRAM.read8(1) == 'n' &&
          FRAM.read8(2) == 'f' && FRAM.read8(3) == 'o') {
      uint8_t* temp = (uint8_t*)pSessionInfo;
      for (int i = 0; i < sizeof(SessionInfo); i++) {
        temp[i] = FRAM.read8(i + 4);
        Serial.print(temp[i], HEX);
      }
      return true;
    }
    else {
      return false;
    }
  }
}
bool cMyLoRaWAN::GetAbpProvisioningInfo(
  AbpProvisioningInfo *pProvisioningInfo
) {
  if (!pProvisioningInfo) {
    return false;
  } else {
    SessionInfo info;
    if (GetSavedSessionInfo(&info, nullptr, 0, nullptr)) {
      //memcpy to copy session info in pProvisioningInfo
      memcpy(pProvisioningInfo->NwkSKey, info.V2.NwkSKey, 16);
      memcpy(pProvisioningInfo->AppSKey, info.V2.AppSKey, 16);
      pProvisioningInfo->DevAddr = info.V2.DevAddr;
      pProvisioningInfo->NetID = info.V2.NetID;
      pProvisioningInfo->FCntUp = info.V2.FCntUp;
      pProvisioningInfo->FCntDown = info.V2.FCntDown;
      return true;
    }
    else {
      return false;
    }
  }
}
