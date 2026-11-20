#include <DS1302.h>
#include <Servo.h> 
#include <SoftwareSerial.h>


 
int M1_IN1 = 3; // 定义uno的pin 3 向 M1_IN1 输出 
int M1_IN2 = 4; // 定义uno的pin 4 向 M1_IN2 输出
int M2_IN1 = 5; // 定义uno的pin 5 向 M2_IN1 输出
int M2_IN2 = 6; // 定义uno的pin 6 向 M2_IN2 输出
int PWM1 = 10; // 定义uno的pin 10 向 输出M1 PWM
int PWM2 = 11; // 定义uno的pin 11 向 输出M2 PWM
 
void setup() {
//  Serial.begin (9600);
//初始化各IO,模式为OUTPUT 输出模式
pinMode(M1_IN1,OUTPUT);
pinMode(M1_IN2,OUTPUT);
pinMode(M2_IN1,OUTPUT);
pinMode(M2_IN2,OUTPUT);
pinMode(PWM1,OUTPUT); 
pinMode(PWM2,OUTPUT); 
}

void forward()     // 前进
{
  digitalWrite(M1_IN1,HIGH); //给高电平
  digitalWrite(M1_IN2,LOW);  //给低电平
  digitalWrite(M2_IN1,HIGH); //给高电平
  digitalWrite(M2_IN2,LOW);  //给低电平
}

void stop()     // 停止
{
 digitalWrite(M1_IN1,LOW);
 digitalWrite(M1_IN2,LOW);  
 digitalWrite(M2_IN1,LOW);
 digitalWrite(M2_IN2,LOW);
}

void back()     // 停止
{
  digitalWrite(M1_IN1,LOW);
  digitalWrite(M1_IN2,HIGH);  
  digitalWrite(M2_IN1,LOW);
  digitalWrite(M2_IN2,HIGH);  
}
 
void loop() {

  analogWrite(PWM1,50);//设置PWM占空比，0~255，255表示全速运行
  analogWrite(PWM2,50);//设置PWM占空比，0~255，255表示全速运行
  forward(); 
  delay(1000);   //延时1秒
 
  stop();
  delay(3000);  //延时1秒
 
  back(); 
  delay(1000);    
 
}
