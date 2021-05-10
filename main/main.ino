#include <SoftwareSerial.h>


#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <DHT.h>


#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_STA_NAME "TrueGigatexFiber_2.4G_zSj"
#define WIFI_STA_PASS "6AgW3eWr"
#define DHTPIN 2 // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11 

const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "81cfcffa-fa5c-48a8-b9eb-168d04b14b29" ;//"dea5dfc7-6a78-4725-8c7b-16521349fb73";
const char* mqtt_username = "BPhAZLj4NtwNH6Yc1FXQ2eihBUganDvg" ;//"cd4nvgu6bQ1wZZn7wxQvtWWTb1RdM2W5";
const char* mqtt_password = "oRo55lxyNo)-YbGzhQDEKxJRzPG36PEf" ;//"pU6*WqViA_ZJIeL-i6-z)5EpPE0gms0A";
/*SoftwareSerial s(16,17);*/
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
DHT dht(DHTPIN, DHTTYPE);
int bufNum = 0;
int count = 0;
void setup() {
  Serial2.begin(115200,SERIAL_8N1,16,17);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);

  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  dht.begin();
}

void callback(char* topic, byte* payload, unsigned int length) {
    /*Serial.print("Message arrive [");
    Serial.print(topic);
    Serial.print("]");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    Serial.println(message);
    if (message == "GET") {
        client.publish("@msg/led", "LEDON");
        Serial.println("Send !");
        return;
    }
    if (message == "LEDON") {
        digitalWrite(LED1, 0);
        Serial.println(message);
    }
    else {
        digitalWrite(LED1, 1);
        Serial.println(message);
    }*/
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection…");
        if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe("@msg/led");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void loop() {
   
    /*int num = ;*/
    if (!client.connected()) {
        reconnect();
    }
    int num = 0;


    float humid = dht.readHumidity();
    float temp = dht.readTemperature();
    humid = 3.3 ; temp = 5.5;
    if (isnan(temp) || isnan(humid)) {
      Serial.println("Failed to read from DHT");
    } 
    else {
      Serial.print("Humidity: "); 
      Serial.print(humid);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(temp);
      Serial.println(" *C");
    }
  
    client.loop();
    //String data = "{\"data\": {\"amount\":" + String(num)  + ", \"humidity\":" + String(humidity) + ", \"temperature\":" + String(temperature) + "}}";
    char msg[75] ;
    
    if (Serial2.available()){
      
       int amount = Serial2.read();
       sprintf(msg,"{\"data\": {\"amount\":%d , \"humid\":%f , \"temp\":%f}}",amount,humid,temp);
       //char data[50] =  "{\"data\": {\"amount\":" + itoa(amount)+ "}}" ; //+ ", \"humid\":" + String(humid) + ", \"temp\":" + String(temp)
       client.publish("@shadow/data/update", msg);
    }
    /*String data = "0000";
    Serial.println(data);
    data.toCharArray(msg, (data.length() + 1));*/
    delay(2000);
   }
