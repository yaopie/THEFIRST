#include<SoftwareSerial.h>
SoftwareSerial mySerial(8, 7);//创建软串口的对象 顺序：RX TX
int x,y;
int mode;
int buttonState = HIGH;           // 当前稳定状态
int lastButtonState = HIGH;       // 上一次读取的状态
unsigned long lastDebounceTime = 0; // 上一次状态变化的时间
unsigned long debounceDelay = 50;   // 去抖时间（毫秒）
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(2,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  int reading = digitalRead(2);
  x = analogRead(A0);
  y = analogRead(A1);
  // 如果读取的状态和上次不同（说明有变化）
  if (reading != lastButtonState) 
  {
    lastDebounceTime = millis(); // 重置去抖计时器
  }
  Serial.print(x);
  Serial.print(',');
  Serial.println(y);
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // 且稳定的状态和记录的当前状态不同，说明确实改变了
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        Serial.println("【事件】按钮按下");
        // 执行按下后的逻辑
        mode = (mode == 1)?0:1;
      }
    }
  }
  lastButtonState = reading; 
  if (mode == 1)
  {
  if(x <= 256)
  {
    mySerial.print('C');
  }
  else
  {
    mySerial.print('c');
  }
  if(y <= 256)
  {
    mySerial.print('D');
  }
  else
  {
    mySerial.print('d');
  }
  }
  else
  {
  if(x <= 256)
  {
    mySerial.print('A');
  }
  else
  {
    mySerial.print('a');
  }
  if(y <= 256)
  {
    mySerial.print('B');
  }
  else
  {
    mySerial.print('b');
  }
  }
}
