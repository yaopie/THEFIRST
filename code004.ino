#include<Servo.h>
Servo Base,fArm,rArm,Claw;
//int BasePos,fArmPos,rArmPos,ClawPos;//舵机运行时的输出值
char Serialcmd;//获取舵机类型

int Datacmd;//获取转动的角度状态
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
//初始状态 1.Base向左拐弯？（面朝机械臂的方位） 2.重复做两次动作？
//未完成任务:1.完成系列动作的任务(),跟前面程序有什么不同？使用二位数组只要是实现 已经打包动作的循环操作，少写调用函数的语句
//小巧思：DSD能否完成不同部件速度上的分类？（说是整体速度了）模板上DSD有作为形参设置的量，这样下次再使用时实参DSD的值就不会受到影响，能否作为其中一种思路？
//2.封装函数，不符合指令的输入字母进行提示
//3.舵机速度的控制，设计成可同时输入的多个指令字母
//4.'V''S''O''I'的限制输入
void reSet()
{
      DSD = 15;
      int reSetArray[4][3] = 
      {
        {'x',90,DSD},
        {'y',90,DSD},
        {'z',90,DSD},
        {'c',65,DSD}
      };
      for(int i = 0;i < 4;i++)
      {
        armDatacmd(reSetArray[i][0],reSetArray[i][1],reSetArray[i][2]);
      }
      
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
  Serial.print("Base value:");//读数有问题 一直都是93
  Serial.println(Base.read());
  Serial.print("fArm value:");
  Serial.println(fArm.read());//读数有问题，一直都是93
  Serial.print("rArm value:");
  Serial.println(rArm.read());
  Serial.print("Claw value:");
  Serial.println(Claw.read());
  Serial.print("DSD:");
  Serial.println(DSD);
}
/*void speedData(Servo&a)
{
  int fromData = a.read();
  if(fromData < Datacmd1)
  {
  for(int i = fromData;i <= Datacmd1;i++)
  {
    temp.write(i);
    delay(DSD1);
  }
  }
  else
  {
  for(int i = fromData;i >= Datacmd1;i--)
  {
    temp.write(i);
    delay(DSD1);
  }
  }
}
*/
void armDatacmd(char Serialcmd1,int Datacmd1,int DSD1)
{
  Servo temp;
  // Serial.print("the statement of servo will be");
  // Serial.println(Datacmd);
  switch(Serialcmd1)
    {
      case 'x':
      // speedData = BasePos;
      // BasePos = Datacmd;
      if(Datacmd1 > BaseMin && Datacmd1 < BaseMax)
      {
      Serial.print("set Base value:");
      Serial.println(Datacmd1);
      temp = Base;
      //speedData(Base);
      break;
      }
      else
      {
        Serial.println("Base position out of limit");
        return;
      }
      case 'y':
      // speedData = fArmPos;
      // fArmPos = Datacmd;
      if(Datacmd1 > fArmMin && Datacmd1 <= fArmMax)
      {
      Serial.print("set fArmPos value:");
      Serial.println(Datacmd1);
      temp = fArm;
      //speedData(fArm);
      break;
      }
      else
      {
        Serial.println("fArm position out of limit");
        return;
      }
      case 'z':
      // speedData = rArmPos;
      // rArmPos = Datacmd;
      Serial.print("set rArmPos value:");
      Serial.println(Datacmd1);
      temp = rArm;
      break;
      case 'c':
      // speedData = ClawPos;
      // ClawPos = Datacmd;
      Serial.print("set ClawPos value:");
      Serial.println(Datacmd1);
      temp = Claw;
      break; 
    }   
    int fromData = temp.read();
  if(fromData < Datacmd1)
  {
  for(int i = fromData;i <= Datacmd1;i++)
  {
    temp.write(i);
    delay(DSD1);
  }
  }
  else
  {
  for(int i = fromData;i >= Datacmd1;i--)
  {
    temp.write(i);
    delay(DSD1);
  }
  }  
  // Serial.println(Base.read());
  // Serial.println(fArm.read());
  // Serial.println(rArm.read());
  // Serial.println(Claw.read());
 
}
void module()//任务二
{
  //int firstArray[][]
}
void Judcmd()
{
  if(Serial.available())
  {
    Serialcmd = Serial.read();
    Serial.println(Serialcmd);
    if(Serialcmd == 'x'||Serialcmd == 'y'||Serialcmd == 'z'||Serialcmd =='c')
    {
      Datacmd = Serial.parseInt();
      Serial.println(Datacmd);
      armDatacmd(Serialcmd,Datacmd,DSD);
    }
    else if(Serialcmd == ','){}
    else{
      switch(Serialcmd)
      {
    case 'O'://机械钳子张开的指令
    armDatacmd('c',0,DSD);
    break;
    case 'S'://机械钳子关闭的指令
    armDatacmd('c',65,DSD);
    break;
    case 'H'://提升整体速度
    {
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
    }
    case 'L'://降低整体速度
    DSD += 15;
    Serial.println("speed down");
    break;
    case 'V':
      showData();
      break;
    case 'I':
      reSet();
      break;
  }
  }
  }
}
void loop() {  // put your main code here, to run repeatedly:
  Judcmd();
}
