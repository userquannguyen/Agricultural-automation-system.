// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLQnnpBv92"
#define BLYNK_DEVICE_NAME "nha ngu"
#define BLYNK_AUTH_TOKEN "x__qMenChjbWDXaIkQkbnWuQHSB63pjU"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter;
//#define motor1 D0
//#define motor2 D4
int pinValue;
int modeValue;
int Mode;
int motor1mode;
int motor2mode;
int lux;
bool autopush1 = false;
bool autopush2 = false;
const int AirValue = 646;  
const int WaterValue = 277;  
int soilMoistureValue = 0;
int soilmoisturepercent = 0;


//SETUP CONNECTION
BLYNK_WRITE(V2)
{
  modeValue= param.asInt();
}
BLYNK_WRITE(V3)
{
  Mode = param.asInt();
  }
BLYNK_WRITE(V4)
{
  motor1mode = param.asInt();
  }
BLYNK_WRITE(V5)
{
  motor2mode = param.asInt();
  }  


 //SETUP PIN
void setup()
{
  Serial.begin(9600);
  delay(200);
  pinMode(D0,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  digitalWrite(D0,LOW);
  digitalWrite(D4,LOW);
  Wire.begin(D2,D1); //dat D1 la SCL; D2 la SDA
  BlynkEdgent.begin();
  lightMeter.begin();
}



//READ SENSOR
void Readsensor1()
{
  //SENSOR1
  delay(50);
  digitalWrite(D5,HIGH);
  digitalWrite(D6,LOW);
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent =  map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  Blynk.virtualWrite(V0,soilmoisturepercent);}
  //SENSOR2
void Readsensor2()
{
  delay(50);
  digitalWrite(D5,LOW);
  digitalWrite(D6,HIGH);
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent =  map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  Blynk.virtualWrite(V1,soilmoisturepercent);
  }

   
//CONTROL
//AUTO MODE
void Autocontrol(int waterpercent,int airpercent,int motor) //dieu khien tu dong
 {
    Blynk.virtualWrite(V2,LOW);
  if(soilmoisturepercent >= waterpercent && motor == 1)
  {
    Serial.println("No need to water");
    digitalWrite(D0,LOW);
    Blynk.virtualWrite(V4,LOW);
    }
   else if(soilmoisturepercent < airpercent && motor == 1)
   {
    Serial.println("Need water, turn on your pump 1");
    digitalWrite(D0,HIGH);
    Blynk.virtualWrite(V4,HIGH);
    }
   else if(soilmoisturepercent >= waterpercent && motor == 2)
  {
    Serial.println("No need to water");
    digitalWrite(D4,LOW);
    Blynk.virtualWrite(V5,LOW);
    }
   else if(soilmoisturepercent < airpercent && motor == 2)
   {
    Serial.println("Need water, turn on your pump 2");
    digitalWrite(D4,HIGH);
    Blynk.virtualWrite(V5,HIGH);
    }
  }
//HANDLE MODE
void Handlecontrol(int which)
{
  Readsensor1(); //doc cam bien 1
  Readsensor2(); //doc cam bien 2
  switch(which)
  {
    case 1: //chon motor 1
    switch(motor1mode)
    {
      case 0:
      digitalWrite(D0,LOW);
      Serial.println("pump 1 is off");
      break;
      case 1: 
      digitalWrite(D0,HIGH);
      Serial.println("pump 1 is on");
      break;
      }
      break;
    case 2: //chon motor 2
    switch(motor2mode)
    {
      case 0:
      digitalWrite(D4,LOW);
      Serial.println("pump 2 is off");
      break;
      case 1:
      digitalWrite(D4,HIGH);
      Serial.println("pump 2 is on");
      break;
      }
      break;  
  }
}


//CHOOSE MODE
void Choose()
{
  if( Mode == 0 )
  {
    autopush1 = true;
    autopush2 = true;
    Serial.println("You are using AUTO mode");
    Readsensor1();
    Autocontrol(84,30,1); //do am >= 84 thi tat may bom, <30 thi bat may bom, dau tay
    Readsensor2();
    Autocontrol(80,40,2); //do am >= 80 thi tat may bom, <40 thi bat may bom; cai xa lach
    }
  if( Mode == 1 && modeValue == 0) //Che do Handle cho motor 1
  { 
    checkauto1();
    checkauto2();
    autopush1 = false;
    autopush2 = false;
    Handlecontrol(1);
    }
   else if( Mode == 1 && modeValue == 1) //Che do Handle cho motor 2
   {
    Handlecontrol(2);   
    }
  }  
  void checkauto1()
{
  {
  switch(autopush1)
  {
    case(true):
    Blynk.virtualWrite(V4,LOW);
    digitalWrite(D0,LOW);
    break; 
    case(false):
    break;    
  }
  }
}
  void checkauto2()
{
  {
  switch(autopush2)
  {
    case(true):
    Blynk.virtualWrite(V5,LOW);
    digitalWrite(D4,LOW);
    break; 
    case(false):
    break;    
  }
  }
}

//LOOP
void loop() {
  lux = lightMeter.readLightLevel();
  Blynk.virtualWrite(V6,lux);
  Choose();
  BlynkEdgent.run();
} 
