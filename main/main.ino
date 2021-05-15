#include <Adafruit_SSD1306.h>

#include <SoftwareSerial.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <DHT.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <SPI.h>
#include <Wire.h>

#include <PubSubClient.h>

#define WIFI_STA_NAME "TrueGigatexFiber_2.4G_zSj"
#define WIFI_STA_PASS "6AgW3eWr"
#define DHTPIN 2 // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define OLED_RESET 4

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
  
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "81cfcffa-fa5c-48a8-b9eb-168d04b14b29" ;//"dea5dfc7-6a78-4725-8c7b-16521349fb73";
const char* mqtt_username = "BPhAZLj4NtwNH6Yc1FXQ2eihBUganDvg" ;//"cd4nvgu6bQ1wZZn7wxQvtWWTb1RdM2W5";
const char* mqtt_password = "oRo55lxyNo)-YbGzhQDEKxJRzPG36PEf" ;//"pU6*WqViA_ZJIeL-i6-z)5EpPE0gms0A";


Adafruit_SSD1306 display(OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
DHT dht(DHTPIN, DHTTYPE);
int amount = 40;
int bufNum = 0;
int count = 0;
void setup() {
  Serial2.begin(115200,SERIAL_8N1,16,17);
  Serial2.setTimeout(1);
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
  setupLed();
  dht.begin();


}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrive [");
    Serial.print(topic);
    Serial.print("]");
    Serial2.print('1');
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    Serial.println(message);

}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection…");
        if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe("@msg/fill");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void loop() {
   

    if (!client.connected()) {
        reconnect();
    }
    int num = 0;


    float humid = dht.readHumidity();
    float temp = dht.readTemperature();
    humid = 69;
    temp = 69;
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
    //Serial2.print('1');
    if (Serial2.available()){
      
      auto tmp = Serial2.readString();
      char con[3]="";
      for (int i=0;i<3;i++){
        if (tmp[i]==',') break;
        con[i] = tmp[i];
      }
      amount = atoi(con);
      Serial.print(amount);
       sprintf(msg,"{\"data\": {\"amount\":%d , \"humid\":%f , \"temp\":%f}}",amount,humid,temp);
       //char data[50] =  "{\"data\": {\"amount\":" + itoa(amount)+ "}}" ; //+ ", \"humid\":" + itoa(humid) + ", \"temp\":" + itoa(temp)
       client.publish("@shadow/data/update", msg);

      /*char cstr[tmp.length() + 1];
    strcpy(cstr, tmp.c_str());
      char *amount = strtok(cstr, ",");
       Serial.print(amount[0]); 
       Serial.print(amount); */
    }
    showAmount(amount);
    delay(1000);
   }
void showAmount(int amount){
 // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("WELCOME!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(1);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if (amount > 100){
    display.print("Run Out!");
  }
  else{
     display.print("amount="); display.print(amount);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

}
void setupLed(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
}
