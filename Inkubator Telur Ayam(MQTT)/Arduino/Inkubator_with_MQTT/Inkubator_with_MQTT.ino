#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h >
#include <LiquidCrystal_I2C.h>

#define pinDHT D2
#define DHTTYPE DHT22
#define relayPin1 D4
#define relayPin2 D5

// Update these with values suitable for your network.

const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(pinDHT, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int pinPIR = D6;
int suhuT = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  payload[length] = '\0'; 
  String message = (char*)payload; 
  suhuT = message.toFloat();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("dataTetas", "topic");
      //data suhu target dikirim melalui website
      client.subscribe("dataSuhu");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
  lcd.begin();
  lcd.backlight();
  pinMode(pinPIR, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

 float h = dht.readHumidity();
 float t = dht.readTemperature();
 String telur; 

 if(digitalRead(pinPIR)== HIGH){
    telur = "Telur Menetas";
    lcdTetas(telur);
    delay(1000);  
  }else{
    telur = "Tidak Menetas";
    delay(1000);
  }
 
 
  if(suhuT == 0){ 
    digitalWrite(relayPin1, HIGH); 
    digitalWrite(relayPin2, HIGH); 
    delay(1000); 
  }else if((suhuT+1) < t){ 
    digitalWrite(relayPin1, LOW); 
    digitalWrite(relayPin2, HIGH); 
    delay(1000); 
  }else if(suhuT > t){ 
    digitalWrite(relayPin1, LOW ); 
    digitalWrite(relayPin2, LOW); 
    delay(1000); 
  }else{ 
    delay(1000); 
  }
 

  serialTampil(t, h, telur);
  lcdSuhu(t, h);
  String dataPack = String(t) + "," + String(h) + "," + String(telur);
  client.publish("dataTetas",(char*)dataPack.c_str());
  delay(1000);
 
}

void lcdSuhu(float suhu, float kelembaban) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Kelembaban: ");
  lcd.setCursor(11,0);
  lcd.print(kelembaban, 1);
  lcd.setCursor(15,0);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("Suhu: ");
  lcd.setCursor(9,1);
  lcd.print(suhu, 1);
  lcd.setCursor(13,1);
  lcd.print(" *C ");
}

void serialTampil(float suhu, float kelembaban, String t){
  Serial.print(F("Humidity: "));
  Serial.print(kelembaban);
  Serial.print(F("%  Temperature: "));
  Serial.print(suhu);
  Serial.print(F("°C "));
  Serial.print(F("Status: "));
  Serial.println(t);
}

void lcdTetas(String t){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(t);
}
