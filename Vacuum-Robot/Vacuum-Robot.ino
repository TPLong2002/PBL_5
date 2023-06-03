#include <Servo.h>
#include <Wire.h> 


int ena = 5;
int in1 = 6;
int in2 = 7;
int in3 = 8;
int in4 = 9;
int enb = 11;
int speed = 150;
int bal=20;

int ServoPin = 10;
Servo myservo; 

int trig = 12;
int echo = 13;

int HighSensorLeft = A3;
int HighSensorRight = A1;
int HighSensorMid = A2;

int ColliderSensorLeft = A4;
int ColliderSensorRight = A0;

bool ZiczacTurnRight = true;

void (*RunningMode)();
void AvoidObjectMode();

int Start;


unsigned long Timer;

unsigned long EndTime;
void setup()
{


  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);

  Serial.begin(9600);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  myservo.attach(ServoPin); 
  myservo.write(90); 

  Wire.begin(7);
  Wire.onReceive(receiveEvent);


  RunningMode = AvoidObjectMode;


  Start = 0;
}


int CalDistance()
{
  unsigned long duration; 
  int distance; 

  digitalWrite(trig,0);   
  delayMicroseconds(2);
  digitalWrite(trig,1);   
  delayMicroseconds(5);   
  digitalWrite(trig,0);   

  duration = pulseIn(echo,HIGH);  
  distance = int((duration/2)*0.034);

  return distance;
}

void Stop()
{
  analogWrite(ena, speed+bal);
  digitalWrite(in1,LOW); 
  digitalWrite(in2,LOW); 

  analogWrite(enb, speed-bal);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
}

void TurnRight(float sec)
{
  Stop();

  analogWrite(ena, speed+bal);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);

  analogWrite(enb, speed-bal);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);

  delay(sec);

  GoStraight();
  
}
void TurnLeft(float sec)
{

  Stop();


  analogWrite(ena, speed+bal);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);

  analogWrite(enb, speed-bal);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);

  delay(sec);

  GoStraight();
  
}
void GoStraight()
{
  analogWrite(ena, speed+bal);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);

  analogWrite(enb, speed-bal);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
}
void GoBack(float sec)
{
  Stop();

  analogWrite(ena, speed+bal);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);

  analogWrite(enb, speed-bal);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);

  delay(sec);

  GoStraight();
}

void HighMove()
{
  GoBack(500);
  TurnRight(500);
}

void ColliderMove()
{
  GoBack(200);
  TurnRight(300);
}

void UltraSonicMove()
{
  GoBack(200);
  TurnRight(300);
}



void ZiczacMode()
{
  int distance = CalDistance();
  if(distance < 8 && distance != 0)
    ZiczacMove();
  Serial.println("Ziczac");
}
void ZiczacMove()
{
  if (ZiczacTurnRight) 
  {
    GoBack(200);
    TurnRight(1300);
    GoStraight();
    delay(300);
    TurnRight(1300);
  }
  else
  {
    GoBack(200);
    TurnLeft(1300);
    GoStraight();
    delay(300);
    TurnLeft(1300);
  }
  ZiczacTurnRight = !ZiczacTurnRight;

  GoStraight();
}


void RandomMode()
{
  int distance = CalDistance();
  if(distance < 8 && distance != 0)
    RandomMove();
    
  Serial.println("Random");
}

int random(int minN, int maxN){
 return minN + rand() % (maxN + 1 - minN);
}
void RandomMove()
{
  srand((int)millis());
  
  GoBack(300);

  int WillTurn = random(0,1);
  if((bool)WillTurn)
    TurnRight(random(1,20)*100);
  else TurnLeft(random(1,20)*100);

  GoStraight();
}


void AvoidObjectMode()
{
  int distance = CalDistance();
  if(distance < 8 && distance != 0)
    AvoidObjectMove();
}
void AvoidObjectMove()
{
  GoBack(200);
  TurnRight(300);
  GoStraight();
}

void loop() 
{
  if(Start != 1)
    return;

  if(millis() > EndTime && EndTime > 0)
  {
    Stop();
    EndTime = 0;
    Start = 0;
  }


  RunningMode();

  int Read_HighSensorRight = digitalRead(HighSensorRight);
  int Read_HighSensorLeft = digitalRead(HighSensorLeft);
  int Read_HighSensorMid = digitalRead(HighSensorMid);
  int Read_ColliderSensorLeft = digitalRead(ColliderSensorLeft);
  int Read_ColliderSensorRight = digitalRead(ColliderSensorRight);

  if(Read_HighSensorRight == 1 || Read_HighSensorLeft || Read_HighSensorMid == 1)
  {
    HighMove();
  }

  if(Read_ColliderSensorLeft == LOW || Read_ColliderSensorRight == LOW)
  {
    ColliderMove();
  }

  delay(100);
}



void receiveEvent(int bytesReceived) 
{

  Stop();

  char Request[bytesReceived];
  for (int i = 0; i < bytesReceived; i++) 
  {
    char ReadCode = (char)Wire.read();
    Request[i] = ReadCode;
    Serial.print(ReadCode);
  }
  Serial.println();

  delay(300);

  if(Request[0] == 'S')
  {
    Start = (int)((char)Request[1]);
    if(Start == 1)
    {
      GoStraight();
      if(Timer > 0)
        EndTime = millis() + Timer*1000;
    }
    else 
    {
      Stop();
      EndTime = 0;
      Timer = 0;
    }
    return;
  }

  if(Request[0] == 'M')
  {
    switch (Request[1]) 
    {
      case 'Z':
        RunningMode = ZiczacMode; 
        break;
      case 'R':
        RunningMode = RandomMode;
        break;
      case 'A':
        RunningMode = AvoidObjectMode;
        break;
    }
    if(Start == 1)
      GoStraight();
    return;
  }

  if(Request[0] == 'T')
  {
    char TimerChar[bytesReceived-1];
    for(int i = 1 ; i<bytesReceived ; i++)
      TimerChar[i-1] = Request[i];
    
    unsigned int time;
    sscanf(TimerChar, "%d", &time);
    Timer = time;
    if(Start == 1)
    {
      EndTime = millis()+Timer*1000;
      GoStraight();
    }
    Serial.println(EndTime);  
    return;
  }
  int SpeedRequest;
  sscanf(Request, "%d", &SpeedRequest);
  speed = SpeedRequest;
  Serial.println(SpeedRequest);  
  if(Start == 1)
    GoStraight();

}
