#include <Servo.h>

Servo myservo;  

int ena = 5;
int in1 = 6;
int in2 = 7;
int in3 = 8;
int in4 = 9;
int enb = 11;
int speed = 150;


int trig = 12;
int echo = 13;

void setup()
{

  //thiết lập servo với chân 10
  myservo.attach(10);
  myservo.write(90); // cho servo quay góc 90 độ để quay về trước mặt mỗi khi bắt đầu

  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //khai báo 2 chân trig phát và echo thu của cảm biến siêu âm
  pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
  pinMode(echo,INPUT);

  GoStraight();

  Serial.begin(9600);
}

// tính khoảng cách từ siêu âm tới vật
int CalDistance()
{
  unsigned long duration; // biến đo thời gian
  int distance; 

  digitalWrite(trig,0);   // tắt chân trig
  delayMicroseconds(2);
  digitalWrite(trig,1);   // phát xung từ chân trig
  delayMicroseconds(5);   // xung có độ dài 5 microSeconds
  digitalWrite(trig,0);   // tắt chân trig

  duration = pulseIn(echo,HIGH);  
  distance = int((duration/2)*0.034);

  return distance;
}

void servo(int g){
  for(int i=myservo.read();i<=g;i++){
    myservo.write(i);
    delay(10);
  }
}
void servoback(int g){
  for(int i=g;i>0;i--){
    myservo.write(i);
    delay(10);
  }
}

void Stop()
{
  //động cơ 1 dừng
  analogWrite(ena, speed);
  digitalWrite(in1,LOW); // cực âm
  digitalWrite(in2,LOW); // cực âm

  analogWrite(enb, speed-10);
  digitalWrite(in3,LOW);// cực âm
  digitalWrite(in4,LOW);// cực âm
}

void TurnRight(float sec)
{
  // rẽ phải trong vòng bao nhiêu giây

  //động cơ A - lùi
  analogWrite(ena, speed);
  digitalWrite(in1,LOW);// cực âm
  digitalWrite(in2,HIGH);// cực dương

  //động cơ B - tiến
  analogWrite(enb, speed-10);
  digitalWrite(in3,HIGH);// cực dương
  digitalWrite(in4,LOW);// cực âm

  //chờ n giây
  delay(sec);

  //sau đó đi thẳng
  GoStraight();
  
}
void TurnLeft(float sec)
{
  //rẽ trái trong vòng bao nhiêu giây

  //động cơ A - tiến
  analogWrite(ena, speed);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);

  //động cơ B - lùi
  analogWrite(enb, speed-10);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);

  //chờ n giây
  delay(sec);

  //sau đó đi thẳng
  GoStraight();
  
}
void GoStraight()
{
  //đi thẳng

  //động cơ A - tiến
  analogWrite(ena, speed);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);

  //động cơ B - tiến
  analogWrite(enb, speed-10);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
}
void GoBack(float sec)
{
  //động cơ A - lùi
  analogWrite(ena, speed);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);

  //động cơ B - lùi
  analogWrite(enb, speed-10);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);

  delay(sec);

  GoStraight();
}
void rotate()
{
  //hàm gọi sẽ khi đang đi thẳng và gặp vật cản trước mặt 10cm

  int dis1 = 0;         // khoảng cách từ siêu âm đến vật gần nhất bên trái
  int dis2 = 0;         // khoảng cách từ siêu âm đến vật gần nhất bên phải
  GoBack(300);          //lùi lại 300ms
  Stop();               //dừng lại
  servo(180);           //quay trái từ 90 độ ban đầu tới góc 180 độ
  dis1 = CalDistance(); // tính khoảng cách từ siêu âm đến vật gần nhất bên trái
  servoback(180);       // quay từ 180 độ bên kia đến 180 độ bên này
  dis2 = CalDistance(); // tính khoảng cách từ siêu âm đến vật gần nhất bên phải
  if(dis1<dis2)         //nếu bên trái có vật
    TurnRight(900);     // quay phair
  else TurnLeft(900);    //quay trai'

  myservo.write(90);     // servo trở lại 90 từ góc 180 bên này
}
void loop() 
{
  int distance = CalDistance();//delay 7ms
  Serial.print(millis()/1000);
  Serial.print(", ");
  Serial.println(distance);
  if(distance < 10 && distance != 0)
    rotate();

  delay(100);
}

