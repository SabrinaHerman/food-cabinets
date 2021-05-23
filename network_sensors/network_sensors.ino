#include <Arduino_LoRaWAN_ttn.h>
#include "keys.h"
#include <Adafruit_FRAM_I2C.h>
#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711
#include <Adafruit_LSM6DSOX.h>

// ------------ Sensors -----------------------------------------------------------------------------------------------------------------
Adafruit_FRAM_I2C FRAM     = Adafruit_FRAM_I2C();
HX711 scale;
Adafruit_LSM6DSOX sox;


// --------- Global Variables -----------------------------------------------------------------------------------------------------------

// Load Cell Variables
#define calibration_factor -705.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define zero_factor 100 //This large value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  11
#define LOADCELL_SCK_PIN  12

int read_sensors = 0;
int c = 0;
int s = 0;

// IR Variables
float fullDiagonal = 32 * 2.54; // distance from front bottom left corner to the back top right corner of the shelf (conversion from in to cm)
int tl;
int bl;
int tr;
int br;
int topLeft;
int botLeft;
int topRight;
int botRight;
int tl_br;
int tr_bl;
float percentage;


// LORA Variables
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
    virtual bool GetAbpProvisioningInfo(
      AbpProvisioningInfo *pProvisioningInfo
    );
};

cMyLoRaWAN myLoRaWAN {};

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
  Serial.begin(115200);
  pinMode(13, OUTPUT);    
  digitalWrite(13, LOW);
  int read_sensors = 0;
  int c = 0;
  

  // ----------------- FRAM -------------------------------------------------------------------------------------------------------
  int b = FRAM.begin();
  delay(3000);
  if (b) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C FRAM");
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?\r\n");
    Serial.println("Will continue in case this processor doesn't support repeated start\r\n");
  }

  // ------------- LORAWAN --------------------------------------------------------------------------------------------------------
  while (!Serial);
  myLoRaWAN.begin(myPinMap);
  lastTime = millis();
  Serial.println("Serial begin");
  if (myLoRaWAN.IsProvisioned())
    Serial.println("Provisioned for something");
  else
    Serial.println("Not provisioned.");
  myLoRaWAN.SendBuffer(messageBuffer, 4, myStatusCallback, NULL, false, 1);


  // ---------- Accelerometer -----------------------------------------------------------------------------------------------------

  // Check if I2C found
  if (!sox.begin_I2C()) {
    Serial.println("Failed to find LSM6DSOX chip");
    while (1) delay(10);
  }
  Serial.println("LSM6DSOX Found!");

  // Set accelerometer range
  Serial.print("Accelerometer range set to: ");
  switch (sox.getAccelRange()) {
  case LSM6DS_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case LSM6DS_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DS_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DS_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  // Set accelerometer data rate
  Serial.print("Accelerometer data rate set to: ");
  switch (sox.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

     
  digitalWrite(13, HIGH);
  delay(40);
  // --------- Load Cells -----------------------------------------------------------------------------------------------------------
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  //scale.set_offset(zero_factor); //Zero out the scale using a previously known zero_factor
  scale.tare();  //Assuming there is no weight on the scale at start up, reset the scale to 0
  
  digitalWrite(13, LOW);

}


void loop() {
  myLoRaWAN.loop();

  // -------- Accelerometer ----------------------------------------------------------------------------------------------------------
  
  // Create Accelerator 
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  // get accel reading
  delayMicroseconds(10000);
  int z = accel.acceleration.z;  
  int threshz = 10;

//  Serial.println(z);
//  Serial.println(read_sensors);
//  Serial.println((z > threshz || z < -threshz) && !read_sensors);

  // if acceleration is detected and not reading sensors, trigger readings
  if((z > threshz || z < -threshz) && !read_sensors) {
    Serial.print("Accelerometer Motion Detected!");
    Serial.println();
    read_sensors = 1;
  }

  // ---------- Load Cells & IR Sensors --------------------------------------------------------------------------------------------
  
  // start reading sensors
  if (read_sensors) {
    digitalWrite(13, HIGH);

    // Get initial reading
    if (c == 0) {
      Serial.print("Initial scale reading: ");
      s = scale.get_units();
      Serial.print(s, 1); //scale.get_units() returns a float
      Serial.print(" lbs");
      Serial.println();

      // Reading IR sensors
      tl = analogRead(A1);
      bl = analogRead(A2);
      tr = analogRead(A3);
      br = analogRead(A4);

      
//      Serial.println("IR sensor readings: ");
//      Serial.println(tl);
//      Serial.println(bl);
//      Serial.println(tr);
//      Serial.println(br);
//       
      topLeft = (6762/(tl-9))-4;
      botLeft = (6762/(bl-9))-4;
      topRight = (6762/(tr-9))-4;
      botRight = (6762/(br-9))-4;

      Serial.print("Initial percentage filled: ");
      Serial.println(topLeft);

//      Serial.println("IR sensor calcs: ");
//      Serial.println(topLeft);
//      Serial.println(botLeft);
//      Serial.println(topRight);
//      Serial.println(botRight);
        
//      tl_br = (fullDiagonal - topLeft - botRight)/fullDiagonal;
//      tr_bl = (fullDiagonal - topRight - botLeft)/fullDiagonal;
//      percentage = 100.0 * ((tl_br + tr_bl)/2);
//
//      Serial.println("IR sensor calc pt2: ");
//      Serial.println(tl_br);
//      Serial.println(tr_bl);
//        
//      Serial.print("Initial percentage filled: ");
//      Serial.println(percentage);
      Serial.println("Wait for timeout period...");
    }

    // increment counter
    c++;
   
    // If time is up, get final readings
    if(c > 1000){
      Serial.println("Timeout period over!");

      // Read Load cells
      Serial.print("Final scale reading: ");
      s = scale.get_units();
      Serial.print(s, 1); //scale.get_units() returns a float
      Serial.print(" lbs");
      Serial.println();

      // Reading IR sensors
      tl = analogRead(A1);
      bl = analogRead(A2);
      tr = analogRead(A3);
      br = analogRead(A4);
       
      topLeft = 29.988*pow(tl,-1.173);
      botLeft = 29.988*pow(bl,-1.173);
      topRight = 29.988*pow(tr,-1.173);
      botRight = 29.988*pow(br,-1.173);

      topLeft = (6762/(tl-9))-4;
      botLeft = (6762/(bl-9))-4;
      topRight = (6762/(tr-9))-4;
      botRight = (6762/(br-9))-4;

      
      Serial.print("Final percentage filled: ");
      Serial.println(topLeft);

      
//      Serial.println("IR sensor calcs: ");
//      Serial.println(topLeft);
//      Serial.println(botLeft);
//      Serial.println(topRight);
//      Serial.println(botRight);
//        
//      tl_br = (fullDiagonal - topLeft - botRight)/fullDiagonal;
//      tr_bl = (fullDiagonal - topRight - botLeft)/fullDiagonal;
//      percentage = 100.0 * ((tl_br + tr_bl)/2);
//        
//      Serial.print("Final percentage filled: ");
//      Serial.println(percentage);


      // ---------- Send Network Data --------------------------------------------------------------------------------------------
  
      int datasave1 = s; // load cell data
      int datasave2 = topLeft; // IR sensor 
      messageBuffer[0]++;
      messageBuffer[1] = datasave1;
      messageBuffer[2] = datasave2;
      myLoRaWAN.SendBuffer(messageBuffer, 4, myStatusCallback, NULL, false, 1);
//      lastTime = millis();

      // reset
      read_sensors = 0;
      c = 0;
      digitalWrite(13, LOW);
    }
  } //end reading sensors
  
} //end loop


// ---------- LORAWAN & FRAM LAB STUFF -----------------------------------------------------------------------------------------------

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
