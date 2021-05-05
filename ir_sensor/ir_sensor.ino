char cmstring[10];

int count = 0;

void setup(void) {
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  
  float fullDiagonal = 32 * 2.54; // distance from front bottom left corner to the back top right corner of the shelf (conversion from in to cm)
}

void loop(void) {

  if (count % 2 == 0) {
    digitalWrite(13, HIGH);
    Serial.println("on");
    int tl = analogRead(A1);
    int bl = analogRead(A2);
    int tr = analogRead(A3);
    int br = analogRead(A4);

    int topLeft = (6762/(tl-9))-4;
    int botLeft = (6762/bl-9))-4;
    int topRight = (6762/(tr-9))-4;
    int botRight = (6762/(br-9))-4;

    int tl_br = fullDiagonal - topLeft - bottomRight;
    int tr_bl = fullDiagonal - topRight - bottomLeft
  
    Serial.println(calculated);
  
    sprintf(cmstring, "%3d", calculated);
  } else {
    digitalWrite(13, LOW);
  }
  
  
//  count++;
  delay(1000);
}
