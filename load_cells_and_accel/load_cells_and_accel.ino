/*
  This code will integrate our load cell shelf with the accelerometer. We will use the 
  Adafruit_LSM6DSOX libarary for the accelerometers and the HX711 library for the load cells 
*/

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711
#include <Adafruit_LSM6DSOX.h>

#define calibration_factor -705.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define zero_factor 100 //This large value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  11
#define LOADCELL_SCK_PIN  12

HX711 scale;
Adafruit_LSM6DSOX sox;
int run_lc = 0;

void setup() {
  Serial.begin(115200);

  // ---------- Accelerometer -------------

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



  // --------- Load Cells -------------------
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  //scale.set_offset(zero_factor); //Zero out the scale using a previously known zero_factor
  scale.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop() {

  // -------- Accelerometer ----------
  
  // Create Accelerator 
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  // get accel reading
  delayMicroseconds(10000);
  int z = accel.acceleration.z;  
  int threshz = 10;

  // if acceleration is detected, trigger load cell readings
  if(z > threshz || z < -threshz) {
    Serial.print("z: ");Serial.print(accel.acceleration.z);
    Serial.println();
    run_lc = 1;
  }

  // ---------- Load Cells ------------------
  if (run_lc) {
    Serial.print("Reading: ");
    Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
    Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
    Serial.println();
  }
}
