#include<Servo.h>
Servo Base,fArm,rArm,Claw;
//下面两个 播放或者录制时的状态信号
bool rIt = 0; 
bool pIt= 0;
int num;
int fromPos,toPos;
const int minData = 256;
const int maxData = 768;
const int moveStep = 10;
const int debounceDelay = 40;   // 去抖时间（毫秒）
const int MAX_FRAMES = 300;    
//下面的两个都是计时器
unsigned long lastRecordTime = 0; 
unsigned long lastPlayTime = 0;   
int playFrames = 0;          // 播放到了第几帧
const int recordGap = 50;    // 录制采样间隔 50ms
int totalFrames = 0; //总共录制的帧数
int DSD = 15;
int fArray[1][4] ={{90,90,90,65}};
const int md1,md2,md3;
int md1Array[md1][4] = {{}{}};
int md2Array[md2][4] = {{}{}};
int md3Array[md3][4] = {{}{}};
//byte array[4][MAX_FRAMES];
struct ArmFrame {
    byte baseAngle;
    byte clawAngle;
    byte rArmAngle;
    byte fArmAngle;
};
struct button
 {
   int dd;
   int reading;
   int buttonState = HIGH;           // 当前稳定状态
   int lastButtonState = HIGH;       // 上一次读取的状态
   unsigned long lastDebounceTime = 0; // 上一次状态变化的时间
 }m[2] = {{0},{2}};
ArmFrame recordedFrames[MAX_FRAMES]; // 录制帧数组 
//bool isRecording = false;   
//bool isPlaying = false;
void toggleRecording() {
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
void startPlayback() {
    if (rIt) return;    // 录制中不允许播放
    if (totalFrames == 0) {     // 没有录制过
        Serial.println("没有可播放的动作！");
        return;
    }
    
    pIt = true;
    playFlames = 0;
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

            
            
            Base.write(recordedFrames[playIndex].baseAngle);
            Claw.write(recordedFrames[playIndex].clawAngle);
            rArm.write(recordedFrames[playIndex].rArmAngle);
            fArm.write(recordedFrames[playIndex].fArmAngle);

            playFrames++; 
        }
    }
}
struct joyStick
{
  Servo temp;
  int reading;
}movement[4] = {{Base},{Claw},{fArm},{rArm}};
 int pushButton(button m[])
 {
   if (m.reading != m.lastButtonState) //状态不同的第一关/（存在按下和松开的两种状态  上一次的状态不行，下一次的状态可以也执行不了
   {m.lastDebounceTime = millis(); // 重置去抖计时器
   m.lastButtonState = m.reading; //筛掉反复状态不同的情况
   }
   if ((millis() - m.lastDebounceTime) > debounceDelay) //状态不同的第二关，状态稳定下来之后跟还没改变的上一次状态再作比较
   {
     // 且稳定的状态和记录的当前状态不同，说明确实改变o
     if (m.reading != m.buttonState) {
       m.buttonState = m.reading;
       if (m.buttonState == LOW) {
         Serial.println("按钮按下!!!");
         // 执行按下后的逻辑
         m.buttonPressStart = millis(); // 记录按下时刻（给长按逻辑用）
         m.isLongPressed = false;  
        
         
        
       }
     }
   }
   else{
    if (!m.isLongPressed && (millis() - m.buttonPressStart < 1000)) {
    return 1 + m.dd; // 返回1：短按
   }
   if (m.buttonState == LOW && !m.isLongPressed && (millis() - m.buttonPressStart > 1000)) {
    m.isLongPressed = true;
    return 2 + m.dd; // 返回2：长按
  }
  return 0;   
 }
// void reSet()//是否可并入runModule里面？(OK!)
// {
//   for(int i = 0;i < 4;i++)
//   {
//     speedData(movement[i].temp,toPos = fArray[i]); 
//   }
// }
void runModule(int array[][4],int n)
{
  for(int j = 0;j < n;j++)
  {
    for(int i = 0;i < 4;i++)
    {
      toPos = Array[j][i];
      speedData(movement[i].temp);
    }
  }
}
void module()
{   
    switch(num)
    {
      case 0:
      runModule();
      num ++;
      break;
      case 1:
      num ++;
      break;
      case 2:
      num = 0;
      break;
    }
//有没有优化的空间？ num计时器改动？
}
void speedData(Servo s)
{
  if(fromPos > toPos){
    if (millis() - lastTime >= 15) {
    
      for(int i = fromPos;i >= toPos;i--)
      s.write(i);
      lastTime = millis();
  }
  else{    
    if (millis() - lastTime >= 15) {
      for(int i = fromPos;i <= toPos;i++)
      s.write(i);
    lastTime = millis();
  }
}
}
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
  Serial.println("begin to test");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  for(int j =2;j < 4;j++)
  {
    movement[j].reading = analogRead(A0 + j);
    Serial.println(movement[j].reading);
    fromPos = movement[j].temp.read();
    //void speedData()
    if(movement[j].reading <= minData)
    {
      toPos = fromPos - moveStep;
      speedData(movement[j].temp);
    }
    else if(movement[j].reading >= maxData)
    {
      toPos = fromPos + moveStep;
      speedData(movement[j].temp);
    }  
  //可用millis()进行延迟计数？(OK!)
  }
  for(int i = 2;i <= 4;i += 2)
  {
    m[i].reading = digitalRead(i);
    switch(pushButton(i))
    {
      case 1:
      
      //开始记录
      toggleRecording();
    }
      case 2:
      //开始回放
      startPlayback();
      case 3:
      reSet();
      case 4:
      //怎么实现三个循环轮换？
      module();
  // Serial.print("Base value:");//读数有问题 一直都是93
  // Serial.println(Base.read());
  // Serial.print("fArm value:");
  // Serial.println(fArm.read());//读数有问题，一直都是93
  // Serial.print("rArm value:");
  // Serial.println(rArm.read());
  // Serial.print("Claw value:");
  // Serial.println(Claw.read());
  //Serial.println(isRecording);
   Record();
   Playback();
}
}
 
