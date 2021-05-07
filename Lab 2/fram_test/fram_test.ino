/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.

  The Adafruit_FRAM_I2C library is also BSD-licensed. This code with the
  Adafruit libraries is distributable under the BSD license, but keep in
  mind that most of the Arduino core is LGPL.
 ***************************************************************************/

#include <Adafruit_FRAM_I2C.h>
#include <Adafruit_BME280.h>

Adafruit_FRAM_I2C FRAM     = Adafruit_FRAM_I2C();
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25)

void setup(){
  
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

  char myStr[30];
  for(int i=0; i<29; i++){
    myStr[i] = FRAM.read8(i);
  }
  myStr[29]=0;
  Serial.print("Before: ");
  Serial.println(myStr);

  String t = "Hello, world";
  for(int i=0; i<t.length(); i++){
    FRAM.write8(i, (uint8_t)t.c_str()[i]);
  }

  for(int i=0; i<29; i++){
    myStr[i] = FRAM.read8(i);
  }

  Serial.print("After: ");
  Serial.println(myStr);

  pinMode(13, OUTPUT);
}

void loop(){
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13,0);
  delay(500);
  printValues();
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
    float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.println(alt);
    int integer = (int)(alt);
    int dec = 10000 * (alt - integer);
    Serial.println(integer);
    Serial.println(dec);
   
}
