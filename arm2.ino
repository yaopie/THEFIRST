//是否有编译日志
//录制算法的优化，可以存更长的时间
//录制读取的 是结构体数组好还是二维数组好？
#include<Servo.h>
Servo Base,fArm,rArm,Claw;
//下面两个 播放或者录制时的状态信号
bool rIt = 0; 
bool pIt= 0;
int num;
int DSD = 15;
int fromPos,toPos;
int playFrames = 0;          // 播放到了第几帧
int totalFrames = 0; //总共录制的帧数
int fArray[1][4] ={{90,90,90,65}};
const int minData = 256;
const int maxData = 768;
const int moveStep = 2;
const int debounceDelay = 40;   // 去抖时间（毫秒）
const int recordGap = 50;
const int MAX_FRAMES = 300;    
//下面的三个都是计时器
unsigned long lastRecordTime = 0; 
unsigned long lastPlayTime = 0;
unsigned long lastTime = 0;   
// const int BaseMin = 0;
// const int BaseMax = 180;
// const int ClawMin = 0;
// const int ClawMax = 65;
// const int fArmMin = 33;
// const int fArmMax = 90;
// const int rArmMin =15;
// const int rArmMax = 170;
const int limit[8] ={0,180,0,90,33,90,15,170};
//const int md1,md2,md3;
//int md1Array[md1][4] = {{}{}};
//int md2Array[md2][4] = {{}{}};
//int md3Array[md3][4] = {{}{}};
//byte array[4][MAX_FRAMES];
struct joyStick
{
  Servo temp;
  int reading;
}movement[4] = {{Base},{Claw},{fArm},{rArm}};
struct ArmFrame 
{
    byte baseAngle;
    byte clawAngle;
    byte rArmAngle;
    byte fArmAngle;
}recordedFrames[MAX_FRAMES];
struct button
 {
   int dd;
   int reading;
   int buttonState;           // 当前稳定状态
   unsigned long lastButtonState;       // 上一次读取的状态
   unsigned long buttonPressStart;    
   unsigned long lastDebounceTime; // 上一次状态变化的时间
   bool isLongPressed;  
 }m[2] = 
 {
  {0, 0, HIGH, HIGH,0,0,0}, 
  {2, 0, HIGH, HIGH,0,0,0}
 };
  int pushButton(button m[],int i)
 {
   if (m[i].reading != m[i].lastButtonState) //状态不同的第一关/（存在按下和松开的两种状态  上一次的状态不行，下一次的状态可以也执行不了
   {
    m[i].lastDebounceTime = millis(); // 重置去抖计时器
    m[i].lastButtonState = m[i].reading; //筛掉反复状态不同的情况
   }
   if ((millis() - m[i].lastDebounceTime) > debounceDelay) //状态不同的第二关，状态稳定下来之后跟还没改变的上一次状态再作比较
   {
     // 且稳定的状态和记录的当前状态不同，说明确实改变o
     if (m[i].reading != m[i].buttonState) {
        m[i].buttonState = m[i].reading;
        if (m[i].buttonState == LOW)
        {
          Serial.println("按钮按下!!!");
         // 执行按下后的逻辑
          m[i].buttonPressStart = millis(); // 记录按下时刻（给长按逻辑用）
          m[i].isLongPressed = false; 
        }
        else
      {
        // 释放瞬间：若未触发长按 → 短按
        if (!m[i].isLongPressed)
        {
          Serial.println(F("短按"));
          return (1 + m[i].dd);
        }
      }
    }

    // 长按检测：按住过程中持续判断
    if (m[i].buttonState == LOW && !m[i].isLongPressed
        && (millis() - m[i].buttonPressStart > 1000))
    {
      m[i].isLongPressed = true;
      Serial.println(F("长按！！！"));
      return (2 + m[i].dd);
    }
  }

  return 0;   
}
        // else if( m[i].buttonState == HIGH && (millis() - m[i].buttonPressStart < 1000))
        // {
        //   Serial.println("短按？？？？？？？？？？？？？？？？");
        //   return (1 + m[i].dd); // 返回1：短按
        // }
        // else if (m[i].buttonState == HIGH && (millis() - m[i].buttonPressStart > 1000))
        //   {
        //     //m[i].isLongPressed = true;
        //     Serial.println("长按！！！！！！！！！！！！！！！！！");
        //     return (2 + m[i].dd); // 返回2：长按   
        //   }
  //         else{
  //   if (!m[i].isLongPressed && (millis() - m[i].buttonPressStart < 1000)) {
  //     Serial.println("短按？？？？？？？？？？？？？？？？");
  //   return (1 + m[i].dd); // 返回1：短按
  //  }
  
  //  }
  //       }
        
  
  // }
//      }
//    }
//   return 0; 
//  }
 void speedData(Servo&a)
{
  int fromData = a.read();
  if(fromData < toPos)
  {
  for(int i = fromData;i <= toPos;i++)
  {
    a.write(i);
    delay(DSD);
  }
  }
  else
  {
  for(int i = fromData;i >= toPos;i--)
  {
    a.write(i);
    delay(DSD);
  }
  }
}
    
 void runModule(int array[][4],int n)
{
  for(int j = 0;j < n;j++)
  {
    for(int i = 0;i < 4;i++)
    {
      toPos = array[j][i];
      speedData(movement[i].temp);
    }
  }
}
void toggleRecording() 
{
    if (pIt) return; // 播放中不允许录制
    if (!rIt) {
        // 开始录制：清空旧数据
        rIt = 1;
        totalFrames = 0;
        lastRecordTime = millis();
        Serial.println("开始录制...");
    } else {
        // 结束录制
        rIt = 0;
        Serial.print("录制结束，共录制了 ");
        Serial.print(totalFrames * recordGap / 1000.0);
        Serial.println(" 秒。");
    }
}
void startPlayback()
 {
    if (rIt) return;    // 录制中不允许播放
    if (totalFrames == 0) {     // 没有录制过
        Serial.println("没有可播放的动作！");
        return;
    }
    
    pIt = true;
    playFrames = 0;
    lastPlayTime = millis();
    Serial.println("开始播放...");
}
void Record() {
    if (rIt) {
        // 每 50 毫秒采样一次
        if (millis() - lastRecordTime >= recordGap) {
            lastRecordTime = millis();
            
            // 内存满了就自动停止
            if (totalFrames >= MAX_FRAMES) {
                rIt = 0;
                Serial.println("内存已满");
                return;
            }
            
            // 读取当前实际舵机角度并存入数组
            recordedFrames[totalFrames].baseAngle = Base.read();
            recordedFrames[totalFrames].clawAngle = Claw.read();
            recordedFrames[totalFrames].rArmAngle = rArm.read();
            recordedFrames[totalFrames].fArmAngle = fArm.read();
            //println(Base.read());
            //println(Claw.read());
            //println(rArm.read());
            //println(fArm.read());
            totalFrames++;
        }
    } 
}
void Playback(){
if (pIt) {
        // 每 50 毫秒播放一帧
        if (millis() - lastPlayTime >= recordGap) {
            lastPlayTime = millis();

            if (playFrames >= totalFrames) { // 播放结束
                pIt = false;
                Serial.println("播放完毕。");
                return;
            }

            
            
            Base.write(recordedFrames[playFrames].baseAngle);
            Claw.write(recordedFrames[playFrames].clawAngle);
            rArm.write(recordedFrames[playFrames].rArmAngle);
            fArm.write(recordedFrames[playFrames].fArmAngle);

            playFrames++; 
        }
    }
}
// void module()
// {   
//     switch(num)
//     {
//       case 0:
//       runModule(md1Array,md1);      
//       num ++;
//       break;
//       case 1:
//       runModule(md2Array,md2);
//       num ++;
//       break;
//       case 2:
//       runModule(md3Array,md3);
//       num = 0;
//       break;
//     }
//}
//有没有优化的空间？ num计时器改动？
// void speedData(Servo&s)
// {
   
//   if(fromPos > toPos){
     
    
      
//    }
//         if (millis() - lastTime >= 5) 
//         {
        
//         // 2. 判断方向并只移动 1 度
//         if (fromPos > toPos) {
//             fromPos--; // 每次只减 1 度
//              // 记录这次执行的时间
//         }
//         else if (fromPos < toPos) {
//             fromPos++; // 每次只加 1 度
            
//         }
        
//         // 如果 fromPos == toPos，说明到终点，什么都不做
//          else {
//             return; // 已经到达终点，直接返回，不再更新计时器
//         }
//         s.write(fromPos);
//         lastTime = millis();
// }
        
        
//    else{    
     
//    }
// }


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
  pinMode(2,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  Serial.begin(9600);
  Base.write(90);
  Claw.write(90);
  fArm.write(90);
  rArm.write(90);
  delay(500);
  Serial.println("begin to test");

}

void loop() {
  // put your main code here, to run repeatedly:
  
  for(int j = 0;j < 4;j++)
  {
    movement[j].reading = analogRead(A0 + j);
  }
  if (millis() - lastTime >= DSD) 
  {
  lastTime = millis();//放在前面和放在后面的效果比较
  for(int j = 0;j < 4;j++)
  {
    int minLimit = limit[j * 2];     // 下限
    int maxLimit = limit[j * 2 + 1]; // 上限
    fromPos = movement[j].temp.read();
    if(movement[j].reading <= 256)
  {
    if(fromPos + moveStep < maxLimit)
    {
      //fromPos = movement[j].temp.read();
    movement[j].temp.write(fromPos + moveStep);
    }
  }
   else if(movement[j].reading >= 768)
  {
    if(fromPos - moveStep > minLimit)
    {
    //fromPos = movement[j].temp.read();
    movement[j].temp.write(fromPos - moveStep);
    }
  }
  }
  }
 //应该不能合并，有时间可以试试
    //void speedData()
    // if(movement[j].reading <= minData)
    // {
    //   toPos = fromPos - moveStep;
    //   //speedData(movement[j].temp);
    //   for(int i = fromPos;i >= toPos;i--)
    //   {
    //     if (millis() - lastTime >= 30){
    //       movement[j].temp.write(i);
    //       lastTime = millis();
    //     }
    //   }
    // } else if(movement[j].reading >= maxData)
    // {
    //   toPos = fromPos + moveStep;
    //   //speedData(movement[j].temp);
    //   //if (millis() - lastTime >= 30) {
    //   for(int i = fromPos;i <= toPos;i++)
    //   {
    //     if (millis() - lastTime >= 30){
    //     movement[j].temp.write(i);
    //     lastTime = millis();
    //     }
    //   }
     //}

  
  //可用millis()进行延迟计数？(OK!)

  for(int i = 0;i <= 1;i ++)
  {
    //m[i].reading = digitalRead(i*2+2);//真实引脚
    //模拟机引脚
    m[i].reading = digitalRead(i*2 + 2);
    int k = pushButton(m,i);
    switch(k)
    {
      case 1:
      //开始记录
      toggleRecording();
      Serial.println(k);
      break;
      case 2:
      //开始回放
      startPlayback();
      Serial.println(k);
      break;
      case 3:
      runModule(fArray,1);
      Serial.println(k);
      break;
      case 4:
      //怎么实现三个循环轮换？(OK!)
      //module();
      Serial.println(k);
      break;
    }
   //Serial.print("Base value:");//读数有问题 一直都是93
   //Serial.println(Base.read());
   //Serial.print("fArm value:");
   //Serial.println(fArm.read());//读数有问题，一直都是93
  // Serial.print("rArm value:");
  // Serial.println(rArm.read());
  // Serial.print("Claw value:");
  // Serial.println(Claw.read());
  //Serial.println(isRecording);
    Record();
    Playback();
  }
}
 
