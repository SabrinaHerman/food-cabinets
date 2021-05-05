#include <Adafruit_LSM6DSOX.h>

Adafruit_LSM6DSOX sox;

void setup() {
  Serial.begin(115200);

  // Wait for Serial Monitor
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit LSM6DSOX test!");

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



}

void loop() {
  // put your main code here, to run repeatedly:

  // Create Accelerator 
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

   //Get Data
//  Serial.print(accel.acceleration.x);
//  Serial.print(","); Serial.print(accel.acceleration.y);
//  Serial.print(","); Serial.print(accel.acceleration.z);
//  Serial.println();


  // Only print if meets threshhold

  int x = accel.acceleration.x;
  int y = accel.acceleration.y;
  int z = accel.acceleration.z;
  delayMicroseconds(10000);
  
  int threshxy = 10;
  int threshz = 0.8;
  
//  if(x > threshxy || x < -threshxy) {
//    Serial.print("x: ");Serial.print(accel.acceleration.x);
//    Serial.println();
//  }
//  if(y > threshxy || y < -threshxy) {
//    Serial.print("y: ");Serial.print(accel.acceleration.y);
//    Serial.println();
//  }
  if(z > threshz || z < -threshz) {
    Serial.print("z: ");Serial.print(accel.acceleration.z);
    Serial.println();
  }
  

}
