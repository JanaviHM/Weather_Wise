/*Weather monitoring system with the New Blynk app and ThingSpeak */
//Library files
#define BLYNK_TEMPLATE_ID "TMPL38MUvOtWO"
#define BLYNK_TEMPLATE NAME "WEATHER-WISE"
#define BLYNK_AUTH_TOKEN "ghQ32m80x7xID061b14dyXrIBNGkdjes"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <SFE BMP180.h>
//Object for the BMP188 sensor
SFE BMP180 bmp;
char auth[] = "ghQ32mB0x7xIDO6Ib14dyXrIBNGkdjes";
char ssid[] = "Abin";
char pass[] = "#connect";
DHT dht (D3, DHT11); //(DHT sensor pin, sensor type)
BlynkTimer timer;
//Rain pins
#define rain A0
//Variables for pressure
double T, P;
char status;
WiFiClient client;
String apikey = "P5PAQ3KA6A86TSNT"; // ThingSpeak API key
const char* server "api.thingspeak.com";
// Global variables for sensor values
float temperature, humidity;
int rainSensorValue;

void setup() {
  Serial.begin(9600);
  bmp.begin();
  /*lcd.init();
  lcd.backlight();
  pinMode(light, INPUT);*/
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();
  /*lcd.setCursor(0, 0);
  lcd.print("Weather Monitor");
  lcd.setCursor(4, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();*/
  timer.setInterval(2000L, rainSensor);
  timer.setInterval(2000L, DHT11sensor);
  //timer.setInterval(2000L, LDRsensor);
  // WiFi connection for ThingSpeak
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

//Get the DHT11 sensor values
void DHT11sensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  /*lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(humidity);*/
  sendToThingSpeak();
}
// Get rain sensor values
void rainSensor() {
  rainSensorValue = analogRead(rain);
  rainSensorValue = map(rainSensorValue, 0, 1024, 0,100);
  Blynk.virtualWrite(V2, rainSensorValue);

 /*lcd.setCursor(0, 1);
  lcd.print("R:");
  lcd.print(rainSensorValue);
  lcd.print(" ");*/
}
void sendToThingSpeak() {
  if (client.connect(server, 80)) {  //will return 1 or 0 based on connection succesful or not
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "&field3=";
    postStr += String(P, 2);
    postStr += "&field4=";
    postStr += String(rainSensorValue);
    postStr += "\r\n\r\n\r\n\r\n";

    //sending data to server via client
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);
// Get the pressure values
void pressure() {
  status = bmp.startTemperature();
  if (status != 0) {
    delay(100);  //delay(status)
    status = bmp.getTemperature(T);

    status = bmp.startPressure(3); // 0 to 3
    if (status != 0) {
      delay(100);  //delay(status)
      status = bmp.getPressure(P, T);

      if (status != 0) {
        Blynk.virtualWrite(V3, P);
        /*lcd.setCursor(8, 1);
        lcd.print("P:");
        lcd.print(P);*/
      }
    }
  }
}

// Get the LDR sensor values
/*void LDRsensor() {
  bool value = digitalRead(light);
  WidgetLED LED(V4);

  if (value == 0) {
    LED.on();
  } else {
    LED.off();
  }
}*/
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Absolute Pressure: ");
  Serial.print(P, 2);
  Serial.println("mb");
  Serial.print("Rain: ");
  Serial.println(rainSensorValue);
}
  client.stop();
  delay(1000);
}

void loop() {
  Blynk.run(); // Run Blynk library,should be  called frequntly to process incomming commands
  timer.run(); // Run Blynk timer,must run aleat once in every millisecond
}