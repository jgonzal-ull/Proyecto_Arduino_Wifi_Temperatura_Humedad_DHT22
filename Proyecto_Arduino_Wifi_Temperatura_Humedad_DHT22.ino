#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "DHT.h"

#define DHTPIN D4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;
const char* ssid = "MOVI_WIFI";
const char* password = "94512900";
// test test 4 test5

void setup() 
{
//  pinMode(BUILTIN_LED, OUTPUT); // Onboard LED
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setPassword((const char *)"123");
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() 
{
  ArduinoOTA.handle();
//  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
//  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  Serial.println("OTA rules....Wifi Wifi");
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  enviasensores(h,t);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  delay(60000);
}



void enviasensores(float h, float t) //client function to send/receive GET request data.
{
  String cad;
  String result;
    
  if (client.connect("emoncms.org", 80)) {  //starts client connection, checks for connection
    cad = "GET /input/post.json?json={TemperaturaBlanca:";
    cad += t;
    cad += "}&apikey=7cbc65d85afc0b03176db5c57f178feb";
    Serial.println(cad);
    client.println(cad);
    client.println("Host: emoncms.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }


  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  Serial.println(result);


  if (client.connect("emoncms.org", 80)) {  //starts client connection, checks for connection
    cad = "GET /input/post.json?json={HumedadBlanca:";
    cad += h;
    cad += "}&apikey=7cbc65d85afc0b03176db5c57f178feb";
    Serial.print(cad);
    client.println(cad);
    client.println("Host: emoncms.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  result = "";
  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  Serial.println(result);
}
  
