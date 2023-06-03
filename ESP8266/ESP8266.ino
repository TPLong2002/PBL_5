
// Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>

char auth[] = "W3hqyyV7yffiOxAZhU3TIDipnX5K6Bw7"; //Enter your Blynk auth token
char ssid[] = "Test"; //Enter your WIFI name
char pass[] = "23456789"; //Enter your WIFI passowrd

unsigned long StartTime;
unsigned long EndTime;

void setup() {
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Serial.begin(9600);
}

// Get the joystick values
BLYNK_WRITE(V2)
{
  // Led = param.asInt();
  if(param.asInt() == 0)
    return;
  
  Wire.beginTransmission(7);
  Wire.write("MZ"); //Mode Ziczac
  Blynk.virtualWrite(V3, "Di chuyển Zic zac");
  Wire.endTransmission();
  
  BlinkLed(100);
}
BLYNK_WRITE(V1)
{
  // Led = param.asInt();
  if(param.asInt() == 0)
    return;
  
  Wire.beginTransmission(7);
  Wire.write("MR"); //Mode Random
  Blynk.virtualWrite(V3, "Di chuyển bất kì");
  Wire.endTransmission();

  BlinkLed(100);
}
BLYNK_WRITE(V0)
{
  // Led = param.asInt();
  if(param.asInt() == 0)
    return;
  
  Wire.beginTransmission(7);
  Wire.write("MA"); //Mode Avoid
  Blynk.virtualWrite(V3, "Né tránh vật cản");
  Wire.endTransmission();

  BlinkLed(100);
}
BLYNK_WRITE(V4)
{
    // Led = param.asInt();
  int speed = param.asInt();
  char str[10];
  sprintf(str, "%d", speed);

  Wire.beginTransmission(7); 
  Wire.write(str); 
  Wire.endTransmission();

  BlinkLed(100);
}
BLYNK_WRITE(V5)
{
  int Start = param.asInt();
  char str[2] = {'S',(char)Start};

  Wire.beginTransmission(7); 
  Wire.write(str);
  Wire.endTransmission();

  BlinkLed(100);
}

BLYNK_WRITE(V6)
{
  unsigned int Second = param.asInt();
  
  if(Second == 0) return;

  char SecValue[10];

  sprintf(SecValue, "%d", Second);
  char SendValue[11];
  SendValue[0] = 'T';
  for(int i = 1 ; i<11 ; i++)
  {
    SendValue[i] = SecValue[i-1];
  }

  Blynk.logEvent("tick_hour","You just set time successfully");

  StartTime = millis();
  EndTime = StartTime + Second*1000 + 300;

  Wire.beginTransmission(7); 
  Wire.write(SendValue);
  Wire.endTransmission();
}

void BlinkLed(int time)
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(time);
  digitalWrite(LED_BUILTIN, LOW);
  delay(time);
}

void loop()
{
  Blynk.run();// Run the blynk function

  if(millis() > EndTime && EndTime > 0)
  {
    EndTime = 0;
    Blynk.logEvent("tick_hour","Auto cleaner time out");
  }
  
}


