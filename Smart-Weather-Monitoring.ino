#include <SFE_BMP180.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
SFE_BMP180 pressure;
#define DHTpin 2
#define DHTTYPE DHT11
#define ALTITUDE 1655.0
DHT dht(DHTpin,DHTTYPE);
//DHT dht(0, DHT11);
//double T, P;
//char status;
WiFiClient client;
String apiKey = "UBKEDLRTKHEBLK8X";
const char* ssid = "NoInternet";
const char* pass = "Arkai@123.";
const char* server = "api.thingspeak.com";
void setup() {
Serial.begin(9600);
delay(10);
//bmp.begin();
Wire.begin();
dht.begin();
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
if (pressure.begin())
Serial.println("BMP180 init success");
else
{
Serial.println("BMP180 init fail\n\n");
while(1); // Pause forever.
}
}
void loop() {
//BMP180 sensor
char status;
double T,P,p0,a;
status = pressure.startPressure(3);
if (status != 0)
{
// Wait for the measurement to complete:
delay(status);
status = pressure.getPressure(P,T);
if (status != 0)
{
Serial.print("absolute pressure: ");
Serial.print(P,2);
Serial.print(" mb, ");
Serial.print(P*0.0295333727,2);
Serial.println(" inHg");
}
}
/*status = bmp.startTemperature();
Serial.println(status);
if (status != 0) {
delay(status);
status = bmp.getTemperature(T);
status = bmp.startPressure(3);// 0 to 3
if (status != 0) {
delay(status);
status = bmp.getPressure(P, T);
if (status != 0) {
Serial.println(T);
}
}
}*/
//DHT11 sensor
float h = dht.readHumidity();
Serial.print("Humidity= ");
Serial.println(h);
//delay(100);
float t = dht.readTemperature();
Serial.print("Temperature= ");
Serial.println(t);
if (isnan(h) || isnan(t)) {
Serial.println("Failed to read from DHT sensor!");
return;
}
//Rain sensor
int r = analogRead(A0);
r = map(r, 0, 1024, 0, 100);
r = 100-r;
if (client.connect(server, 80)) {
String postStr = apiKey;
postStr += "&field1=";
postStr += String(t);
postStr += "&field2=";
postStr += String(h);
postStr += "&field3=";
postStr += String(P, 2);
postStr += "&field4=";
postStr += String(r);
postStr += "\r\n\r\n\r\n\r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n\n\n");
client.print(postStr);
Serial.print("Temperature: ");
Serial.println(t);
Serial.print("Humidity: ");
Serial.println(h);
Serial.print("absolute pressure: ");
Serial.print(P, 2);
Serial.println("mb");
Serial.print("Rain: ");
Serial.println(r);
}
client.stop();
delay(1000);
}
