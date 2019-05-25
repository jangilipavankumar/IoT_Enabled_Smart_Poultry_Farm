#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define SSD1306_LCDHEIGHT 64
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#include "DHT.h"
#include <Servo.h>
Servo myservo;
#define  DHTPIN D0
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
int temp_led=D7;
#define echopin D3
#define trigpin D4
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "pavan";
const char* password = "panti@123";
String command;
#define ORG "qp5k26"
#define DEVICE_TYPE "new"
#define DEVICE_ID "newdevice"
#define TOKEN "123456789"
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup() 
{
  Serial.begin(9600);
  dht.begin();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  Serial.println("");
 }
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(temp_led,OUTPUT);
  pinMode(D3,INPUT);
  pinMode(D4,OUTPUT);
  myservo.attach(D5);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.print("poultry farm");
  Serial.println("poultry farm");
  display.display();
}

void loop() 
{
  
  //gas sensor start
  int analogSensor = analogRead(A0);
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  delay(500);
 if (analogSensor >9)
  {
   display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,10);
   display.print("Please switch on the Exhaust fans");
   display.display();
   Serial.println("Please switch on the Exhaust fans");
  }
else
  {
    }
  delay(100);
  //gas sensor end
  
  //dht11 start
  float t = dht.readTemperature();
  Serial.println(t);
  delay(500);
if(t>35)
  {
   display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,10);
   display.print("Temperature is HIGH");
   display.display();
   Serial.println("Temperature is HIGH");
   digitalWrite(temp_led, HIGH); //Red
   delay(200);
  }
else if(t<25)
  {
   display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,10);
   display.print("Temperature is LOW");
   display.display();
   Serial.println("Temperature is LOW");
  }
else
{
  digitalWrite(temp_led,  LOW);
}
//dht11 end

//ultrasoinc start
  digitalWrite(D4,HIGH);
  delay(1000);
  digitalWrite(D4,LOW);
  int duration = pulseIn(echopin,HIGH);
  int distance = (duration/2)*0.0343;
  Serial.println(distance);
  delay(500);
if(distance<5)
 {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.print("Please fill up the water tanks");
  display.display();
  Serial.println("Please fill up the water tanks");
  delay(500);
 }
else if(distance>10)
 {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.print("Water tank is full");
  display.display();
  Serial.println("Water tank is full");
  delay(500);
 }
 else
 {
  }
 //ultrasoinc end
 
  //servo start
  myservo.write(90);
  delay(2000);
  myservo.write(0);                           
  delay(100);
 //servo end
 PublishData(t, distance, analogSensor);
}
void PublishData(float t, float distance, float analogSensor)
 {
  if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"t\":";
  payload += t;
  payload+="," "\"distance\":";
  payload += distance;
  payload+="," "\"analogSensor\":";
  payload += analogSensor;
  payload += "}}";
  Serial.print("Sending payload: ");
  Serial.println(payload);
if (client.publish(topic, (char*) payload.c_str()))
 {
 Serial.println("Publish ok");
 } 
else 
{
 Serial.println("Publish failed");
 }
 }
