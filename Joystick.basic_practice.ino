void setup() {
  // put your setup code here, to run once:
  pinMode(2,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int x = analogRead(A0);
  int y = analogRead(A1);
  if(digitalRead(2) == 0)
  {
    Serial.println("push the button");
    delay(100);
  }
  else
  {
    Serial.print(x);
    Serial.print(',');
    Serial.println(y);
    delay(100);
  }
}
