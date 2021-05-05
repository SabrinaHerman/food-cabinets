char cmstring[10];

int count = 0;

void setup(void) {
  pinMode(13,OUTPUT);
  Serial.begin(9600);
}

void loop(void) {

  if (count % 2 == 0) {
    digitalWrite(13, HIGH);
    Serial.println("on");
    int reading = analogRead(A1);

    int calculated = (6762/(reading-9))-4;
  
    Serial.println(calculated);
  
    sprintf(cmstring, "%3d", calculated);
  } else {
    digitalWrite(13, LOW);
  }
  
  
//  count++;
  delay(1000);
}
