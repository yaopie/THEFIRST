#include<Servo.h>
Servo Base,fArm,rArm,Claw;
int BasePos,fArmPos,rArmPos,ClawPos;//舵机运行时的输出值
char Serialcmd;//获取舵机类型
int Datacmd;//获取转动的角度状态
int fromData;//获取转动前的角度状态
int DSD;//舵机运行速度
//范围常量
const int BaseMin = 0;
const int BaseMax = 180;
const int fArmMin = 33;
const int fArmMax = 90;
const int rArmMin =15;
const int rArmMax = 170;
const int ClawMin = 0;
const int ClawMax = 65;
//未完成任务:1.完成系列动作的任务(),跟前面程序有什么不同？使用二位数组只要是实现 已经打包动作的循环操作，少写调用函数的语句
//小巧思：DSD能否完成不同部件速度上的分类？（说是整体速度了）模板上DSD有作为形参设置的量，这样下次再使用时实参DSD的值就不会受到影响，能否作为其中一种思路？
//2.封装函数，不符合指令的输入字母进行提示
//3.舵机速度的控制，设计成可同时输入的多个指令字母
void speedData(Servo&x,int&a,int DSD1)
{
  fromData = x.read();
  a = Datacmd;
  if(fromData < a)
  {
  for(int i = fromData;i <= a;i++)
  {
    x.write(i);
    delay(DSD1);
  }
  }
  else
  {
     for(int i = fromData;i >= a;i--)
  {
    x.write(i);
    delay(DSD1);
  }
  }
}
void reSet()
{
      Datacmd = 90;
      DSD = 15;
      speedData(Base,BasePos,DSD);
      speedData(fArm,fArmPos,DSD);
      speedData(rArm,rArmPos,DSD);
      Datacmd = 65;
      speedData(Claw,ClawPos,DSD);
      Serial.println("reset");
}
void setup() {
  // put your setup code here, to run once:
  Base.attach(10);
  delay(100);
  fArm.attach(11);
  delay(100);
  rArm.attach(3);
  delay(100);
  Claw.attach(6);
  delay(100);
  Serial.begin(9600);
  Serial.println("begin to test");
  reSet();
}
void showData()
{
  Serial.print("Base value:");
  Serial.println(BasePos);
  Serial.print("fArm value:");
  Serial.println(fArmPos);
  Serial.print("rArm value:");
  Serial.println(rArmPos);
  Serial.print("Claw value:");
  Serial.println(ClawPos);
  Serial.print("DSD:");
  Serial.println(DSD);
}
void armDatacmd()
{
  // Serial.print("the statement of servo will be");
  // Serial.println(Datacmd);
  switch(Serialcmd)
    {
      case 'B':
      // speedData = BasePos;
      // BasePos = Datacmd;
      if(Datacmd > BaseMin && Datacmd < BaseMax)
      {
      Serial.print("set Base value:");
      Serial.println(Datacmd);
      speedData(Base,BasePos,DSD);
      break;
      }
      else
      {
        Serial.println("Base position out of limit");
        return;
      }
      case 'F':
      // speedData = fArmPos;
      // fArmPos = Datacmd;
      if(Datacmd > fArmMin && Datacmd <= fArmMax)
      {
      Serial.print("set fArmPos value:");
      Serial.println(Datacmd);
      speedData(fArm,fArmPos,DSD);
      break;
      }
      else
      {
        Serial.println("fArm position out of limit");
        return;
      }
      case 'R':
      // speedData = rArmPos;
      // rArmPos = Datacmd;
      Serial.print("set rArmPos value:");
      Serial.println(Datacmd);
      speedData(rArm,rArmPos,DSD);
      break;
      case 'C':
      // speedData = ClawPos;
      // ClawPos = Datacmd;
      Serial.print("set ClawPos value:");
      Serial.println(Datacmd);
      speedData(Claw,ClawPos,DSD);
      break;
      case 'A':
      showData();
      break;
      case 'I':
      reSet();
      break;
    }
}
void module1()
{
  switch(Serialcmd)
  {
    case 'O'://机械钳子张开的指令
    Datacmd = 0;
    speedData(Claw,ClawPos,DSD);
    break;
    case 'S'://机械钳子关闭的指令
    Datacmd = 65;
    speedData(Claw,ClawPos,DSD);
    break;
    case 'H'://提升整体速度
    if(DSD == 0)
    {
      Serial.println("fastest now");
    }
    else
    {
      DSD -= 15;
      Serial.println("speed up");
    }
    break;
    case 'L'://降低整体速度
    DSD += 15;
    Serial.println("speed down");
    break;
  }
}
void module2()
{
  // Servo&x = Base;
  // Servo&y = rArm;
  // Servo&z = fArm;
  if (Serialcmd == 'x')//同步？ 更加严谨的条件?
  {
    BasePos = Datacmd; 
    rArmPos = Serial.parseInt(); 
    fArmPos = Serial.parseInt(); 
    speedData(Base,BasePos,DSD);
    speedData(rArm,rArmPos,DSD);
    speedData(fArm,fArmPos,DSD);
    Serial.print("x(Base) value ");
    Serial.println(BasePos);
    Serial.print("x(rArm) value ");
    Serial.println(rArmPos);
    Serial.print("x(fArm) value ");
    Serial.println(fArmPos);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    Serialcmd = Serial.read();
    Serial.println(Serialcmd);
    Datacmd = Serial.parseInt();
    Serial.println(Datacmd);
    // BasePos = Base.read()
    // fArmPos = fArm.read()
    // rArmPos = rArm.read()
    // ClawPos = Claw.read()
    armDatacmd();
    module1();
    module2();
  }
    Base.write(BasePos);
    fArm.write(fArmPos);
    rArm.write(rArmPos);
    Claw.write(ClawPos);//这几个为什么在网课的代码优化过程中没有出现，不应该要有让舵机保持的指令吗？
  }
