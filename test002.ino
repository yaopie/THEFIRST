//int fromData;
#include<Servo.h>
const int moveStep = 2;
int DSD = 15;
int fromData;
unsigned long lastTime = 0;
Servo Base,fArm,rArm,Claw;
const int BaseMin = 0;
const int BaseMax = 180;
const int fArmMin = 33;
const int fArmMax = 90;
const int rArmMin =15;
const int rArmMax = 170;
const int ClawMin = 0;
const int ClawMax = 65;
struct joyStick
{
  Servo temp;
  int reading;
}movement[4] = {{Base},{Claw},{fArm},{rArm}};
void setup() {
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
  Claw.write(65);
  fArm.write(90);
  rArm.write(90);
  delay(500);
}
// int readData()
//   {
//     fromData = movement[i].temp.read();
//   }

void loop() {
  // put your main code here, to run repeatedly:
  movement[0].reading = analogRead(A0);
  movement[1].reading = analogRead(A1);
  movement[2].reading = analogRead(A2);
  movement[3].reading = analogRead(A3);
  if (millis() - lastTime >= DSD) 
  {
  lastTime = millis();//放在前面和放在后面的效果比较
  if(movement[0].reading <= 256)
  {
    fromData = movement[0].temp.read();
    movement[0].temp.write(fromData + moveStep);
  }
  if(movement[0].reading >= 768)
  {
    fromData = movement[0].temp.read();
    movement[0].temp.write(fromData - moveStep);
  }
  if(movement[1].reading <= 256)
  {
    fromData = movement[1].temp.read();
    movement[1].temp.write(fromData + moveStep);
  }
  if(movement[1].reading >= 768)
  {
    fromData = movement[1].temp.read();
    movement[1].temp.write(fromData - moveStep);
  }
  if(movement[2].reading <= 256)
  {
    fromData = movement[2].temp.read();
    movement[2].temp.write(fromData + moveStep);
  }
  if(movement[2].reading >= 768)
  {
    fromData = movement[2].temp.read();
    movement[2].temp.write(fromData - moveStep);
  }
  if(movement[3].reading <= 256)
  {
    fromData = movement[3].temp.read();
    movement[3].temp.write(fromData + moveStep);
  }
  if(movement[3].reading >= 768)
  {
    fromData = movement[3].temp.read();
    movement[3].temp.write(fromData - moveStep);
  }
}
}
