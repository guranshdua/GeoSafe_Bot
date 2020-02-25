#include <Servo.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

#include "DHT.h"
#define DHTTYPE DHT11

char auth[] = "";
#define WIFI_SSID "Guransh"
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "" 
#define FIREBASE_AUTH ""

Servo myservo;
int pos = 0;

#define SERVO D4
#define dht_dpin D3
#define SENSORS A0
#define MUX_PIN_A D0
#define MUX_PIN_B D1
#define MUX_PIN_C D2
#define ENB D3
#define MOTORB_1 D7
#define MOTORB_2 D8
#define MOTORA_1 D5
#define MOTORA_2 D6

String fireStatusLed = "";
int a=0,b=0,c=0,s[5];

WidgetTerminal terminal(V1);

DHT dht(dht_dpin, DHTTYPE);

void setup() {
  dht.begin();
  myservo.attach(SERVO);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MUX_PIN_A,OUTPUT);
  pinMode(MUX_PIN_B,OUTPUT);
  pinMode(MUX_PIN_C,OUTPUT);
  pinMode(SENSORS,INPUT);
  pinMode(ENB, OUTPUT);
  pinMode(MOTORB_1, OUTPUT);
  pinMode(MOTORB_2, OUTPUT);
  pinMode(MOTORA_1, OUTPUT);
  pinMode(MOTORA_2, OUTPUT);

  digitalWrite(ENB,HIGH);
  
  Serial.begin(9600);
  delay(1000);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN,LOW);
  }
  
  Serial.println();
  for(int i=0;i<10;i++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN,LOW);
    delay(200);
  }
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(ENB,LOW);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
  
  Blynk.virtualWrite(V1,"Blynk v ", BLYNK_VERSION, ": Device started\n");
  Blynk.virtualWrite(V1,"-------------\n");
}



void loop() {
  
  Blynk.run();
}

BLYNK_WRITE(V0) {
  int Flag=1;
  int incomingByte;
  int y = param[1].asInt();
  int x = param[0].asInt();
    if (y > 750) {  // Forward
    Serial.println("Forward");
    digitalWrite(MOTORB_1,HIGH);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,HIGH);
    delay(1000);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,LOW);
    incomingByte = '1';
  }
  if (y < 250) {  // Backward
    Serial.println("Backward");
    incomingByte = '2';
    digitalWrite(MOTORB_2,HIGH);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORA_1,LOW);
    digitalWrite(MOTORA_2,HIGH);
    delay(1000);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,LOW);
  }
  if (x < 250) {  // Left
    Serial.println("Left");
    digitalWrite(MOTORB_2,HIGH);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,HIGH);
    delay(1000);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,LOW);
    incomingByte = '3';
  }
  if (x > 750) {  // Right
    Serial.println("Right");
    digitalWrite(MOTORB_1,HIGH);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_1,LOW);
    digitalWrite(MOTORA_2,HIGH);
    delay(1000);
    digitalWrite(MOTORB_1,LOW);
    digitalWrite(MOTORB_2,LOW);
    digitalWrite(MOTORA_2,LOW);
    digitalWrite(MOTORA_1,LOW);
    incomingByte = '4';
  }
  if (x == 512 && y == 512  && Flag==1) {  // Full Stop
    incomingByte = '0';
    Flag=0;
  }  
}

BLYNK_WRITE(V2)
{
  int val;
  int button=param.asInt();
  if(button==1)
  {
    Serial.println("HERE");
    myservo.write(90);
    delay(1000);
    digitalWrite(MUX_PIN_A,0);
    digitalWrite(MUX_PIN_B,1);
    digitalWrite(MUX_PIN_C,1);
    delay(200);
    val=analogRead(SENSORS);
    Blynk.virtualWrite(V1,"Moisture Value");
    Blynk.virtualWrite(V1,val);
    delay(200);
    Serial.println("SERVO "+val);
    myservo.write(180);
    Firebase.set("NodeCurrent/Delhi/Soil", val);
    delay(1000);
    
  }
}

BLYNK_WRITE(V3)
{
  int button = param.asInt();
  if(button==1)
  {
  for(int i=0;i<2;i++)
  {
    a=bitRead(i,0);
    b=bitRead(i,1);
    c=bitRead(i,2);

    digitalWrite(MUX_PIN_A,a);
    digitalWrite(MUX_PIN_B,b);
    digitalWrite(MUX_PIN_C,c);

    s[i]=analogRead(SENSORS);
    delay(1000);
  }
  
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& myObject = jsonBuffer.createObject();
  JsonObject& tempTime = myObject.createNestedObject("timestamp");
  myObject["temperature"] = t;
  myObject["humidity"] = h;
  myObject["mq135"]=s[0];
  myObject["mq2"]=s[2];
  myObject["rain"]=s[1];
  tempTime[".sv"] = "timestamp";

 
  fireStatusLed = Firebase.getString("NodeCurrent/Delhi/fireStatusLed" );
  if (fireStatusLed == "ON") {
    Serial.println("Led Turned ON");
    digitalWrite(LED_BUILTIN, LOW);
    Firebase.push("NodeComplete/Delhi", myObject);
    myObject["fireStatusLed"]="ON";
    Firebase.set("NodeCurrent/Delhi/", myObject);
  }
  else if (fireStatusLed == "OFF") {
    Serial.println("Led Turned OFF");
    char buf[200];
    Blynk.virtualWrite(V1,t,h,s[2],s[1],s[0]);
    sprintf(buf,"TEMPERATURE : %d\nHUMIDITY : %d\nSMOKE : %d\nAQI(PPM) : %d\nRAIN : %d",t,h,s[2],s[0],s[1]);
    Serial.println(buf);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    Serial.println("Wrong Credential! Please send ON/OFF");
  } 
  
  delay(6000);
  }
  
}
