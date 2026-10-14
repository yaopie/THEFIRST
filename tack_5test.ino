#include <Servo.h>

Servo baseServo, fArmServo, rArmServo, clawServo;
int Serialcmd;
const float L1 = 67.7;       // 大臂长度
const float L2 = 78;       // 小臂长度
const float BASE_H = 5.0;   // 底座转轴距离桌面的高度
const int OFFSET_BASE = 90; 
const int OFFSET_FARM = 90; 
const int OFFSET_RARM = 90;
void drawMove(float targetX, float targetY, float targetZ) {
  // 1. 计算底座角度（用 atan2 自动处理象限问题）
  float theta_base = atan2(targetY, targetX);
  
  // 2. 计算平面几何距离
  float r = sqrt(targetX * targetX + targetY * targetY);
  float h = targetZ - BASE_H; // 垂直相对高度
  
  // 3. 计算直线距离 D (用于余弦定理)
  float D = sqrt(r * r + h * h);
  
  // 4. 安全限位（如果目标点太远，机械臂够不到，会出现NaN错误）
  if (D > (L1 + L2) || D < (L2 - L1))
  {
    Serial.println("错误：目标点超出机械臂工作范围！");
    return;
  }

  // 5. 余弦定理求小臂偏折角 theta_2
  float cos_theta2 = (L1*L1 + L2*L2 - D*D) / (2 * L1 * L2);
  cos_theta2 = constrain(cos_theta2, -1.0, 1.0); // 防止浮点误差导致越界
  float theta_2 = PI - acos(cos_theta2); // 弧度制

  // 6. 求大臂角度 theta_1 (通过角度叠加)
  float alpha = atan2(h, r); // 目标点连线与水平面的夹角
  float cos_beta = (D*D + L1*L1 - L2*L2) / (2 * D * L1);
  cos_beta = constrain(cos_beta, -1.0, 1.0);
  float beta = acos(cos_beta);
  float theta_1 = alpha + beta; // 大臂绝对角度（弧度制）

  // 7. 弧度转角度，并加上偏移量
  float angle_base = degrees(theta_base);
  float angle_fArm = degrees(theta_1) ;
  float angle_rArm = degrees(theta_2) ;
  Serial.println(angle_base);
  Serial.println(angle_fArm);
  Serial.println(angle_rArm);
  // 8. 写入舵机（确保角度在 0~180 的安全范围内）
  baseServo.write(constrain(angle_base, 0, 180));
  fArmServo.write(constrain(angle_fArm, 0, 180));
  rArmServo.write(constrain(angle_rArm, 0, 180));
  
  // 更新当前位置
  //currentX = targetX;
  //currentY = targetY;
  //currentZ = targetZ;
}
void setup() {
  // put your setup code here, to run once:
  baseServo.attach(11);
  fArmServo.attach(9);
  rArmServo.attach(13);
  clawServo.attach(5); 
  Serial.begin(9600);
  // baseServo.write(90);
  // clawServo.write(90);
  // fArmServo.write(90);
  // rArmServo.write(90);
  drawMove(50,50,100);
  delay(500);

   
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
  Serialcmd = Serial.read();
  if(Serialcmd == 's')
  {
    for(int i = 0;i <= 100;i++)
    {
    drawMove(i,100,50);
    delay(50);
    }
  }
  if(Serialcmd == 'd')
  {
    for(int i = 100;i >= 0;i--)
    {
    drawMove(i,100,50);
    delay(50);
    }
  }
  if(Serialcmd == 'f')
  {
    for(int i = 100;i >= 0;i--)
    {
    drawMove(0,i,50);
    delay(50);
    }
  }
  if(Serialcmd == 'g')
  {
    for(int i = 0;i <= 100;i--)
    {
    drawMove(0,i,50);
    delay(50);
    }
  }
  if(Serialcmd == 'h')
  {
    for(int i = 50;i >= 0;i--)
    {
    drawMove(0,100,i);
    delay(50);
    }
  }
  if(Serialcmd == 'j')
  {
    for(int i = 0;i <= 50;i++)
    {
    drawMove(0,100,i);
    delay(50);
    }
  }
  }
}
