char cmstring[10];

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  int reading = analogRead(A0);

  int calculated = (6762/(reading-9))-4;

  Serial.println(calculated);

  sprintf(cmstring, "%3d", calculated);

  delay(200);
}
