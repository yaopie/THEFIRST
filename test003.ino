#include<Servo.h>
const int moveStep = 2;
int DSD = 15;
int fromPos;
unsigned long lastTime = 0;
Servo Base,fArm,rArm,Claw;
struct joyStick
{
  Servo temp;
  int reading;
}movement[4] = {{Base},{Claw},{fArm},{rArm}};
void setup() 
{
  // put your setup code here, to run once:
  Base.attach(11);
  delay(100);
  fArm.attach(9);
  delay(100);
  rArm.attach(10);
  delay(100);
  Claw.attach(5);
  delay(100);
  Serial.begin(9600);
  Serial.println("begin to test");
  Base.write(90);
  Claw.write(90);
  fArm.write(90);
  rArm.write(90);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int j = 0;j < 4;j++)
  {
    movement[j].reading = analogRead(A0 + j);
    fromPos = movement[j].temp.read();
  }
      if (millis() - lastTime >= DSD) 
  {
  lastTime = millis();//放在前面和放在后面的效果比较
  for(int j = 0;j < 4;j++)
  {
    if(movement[j].reading <= 256)
  {
    fromPos = movement[j].temp.read();
    movement[j].temp.write(fromPos + moveStep);
  }
   else if(movement[j].reading >= 768)
  {
    fromPos = movement[j].temp.read();
    movement[j].temp.write(fromPos - moveStep);
  }
  }
  }
}
