#include <TimeLib.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#include <HTTPClient.h>


#define BLYNK_PRINT Serial


#define BLYNK_TEMPLATE_ID "TMPL6aVOocfr5"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Xd-2iVsjtnJMLKnKzTJubvsaCY1pooP1"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>




// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Smarthome";
char pass[] = "Nonnytuss2542";
const char* LINE_NOTIFY_TOKEN = "Xr0EaOYlhVHx0nEYJQC8h2nC8sD1mT40cNRa39QcJa5";



#define LED_PIN_1 2
#define LED_PIN_2 4
#define LED_TV_PIN_3 5
#define FAN_PIN_1 12
#define FAN_PIN_2 13
#define DOOR_PIN_1 18
#define SMOKEFAN_PIN 27

#define TEMPERATURE_SENSOR_PIN 34
#define SMOKE_SENSOR_PIN 36
#define SMOKE_SENSOR_PIN2 35
#define SMOKE_SENSOR_THRESHOLD 1500
#define SMOKE_SENSOR_THRESHOLD2 2100
#define ALARM_PIN 19 
#define DHTPIN 3 
#define DHTTYPE    DHT22 
#define IR_SENSOR_PIN 15

DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
BlynkTimer timer;
//bool fanState = false;



void setup()
{
  
  // Debug console
  
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_TV_PIN_3, OUTPUT);
  pinMode(FAN_PIN_1, OUTPUT);
  pinMode(FAN_PIN_2, OUTPUT);
  pinMode(DOOR_PIN_1, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(SMOKE_SENSOR_PIN, INPUT);
  pinMode(SMOKE_SENSOR_PIN2, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(SMOKEFAN_PIN, OUTPUT);
  

//  Blynk.virtualWrite(V4, fanState ? HIGH : LOW);
  
  
}

void loop()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  int smokeValue = analogRead(SMOKE_SENSOR_PIN);
  int smokeValue2 = analogRead(SMOKE_SENSOR_PIN2);

  
  
  
  Serial.println(smokeValue);
  Serial.println(smokeValue2);
  if (smokeValue > SMOKE_SENSOR_THRESHOLD) {
    sendLineNotify("Smoke Detected! ค่าควันภายในบ้านผิดปกติ");
    activateAlarm();
  }

  if (smokeValue2 > SMOKE_SENSOR_THRESHOLD2) {
    activateFan();
  }
  Blynk.run();
  timer.run();
  Serial.println(event.temperature); 
  Blynk.virtualWrite(V6, event.temperature); 
  Blynk.virtualWrite(V7, smokeValue); 
  Blynk.virtualWrite(V8, smokeValue2);

  
  delay(1000); 
}

void checkSmokeSensor() {
  int smokeValue = analogRead(SMOKE_SENSOR_PIN);

  if (smokeValue > SMOKE_SENSOR_THRESHOLD) {
    sendLineNotify("Smoke Detected!");
    activateAlarm();
  }
}

BLYNK_WRITE(V0)
{
  int value = param.asInt(); // Get value as integer
  digitalWrite(LED_PIN_1, value);
}

BLYNK_WRITE(V1)
{
  int value = param.asInt();
  digitalWrite(LED_PIN_2, value);
}

BLYNK_WRITE(V2)
{
  int value = param.asInt();
  digitalWrite(LED_TV_PIN_3, value);
}

BLYNK_WRITE(V3)
{
  int value = param.asInt();
  digitalWrite(FAN_PIN_1, value);
}

BLYNK_WRITE(V4)
{
  int fanControl = param.asInt();
  if (fanControl == 1) {
    // Turn on fan
    digitalWrite(FAN_PIN_2, HIGH);
  } else {
    // Turn off fan
    digitalWrite(FAN_PIN_2, LOW);
  }
  
//  int value = param.asInt();
//  digitalWrite(FAN_PIN_2, value);
}


BLYNK_WRITE(V5)
{
  int value = param.asInt();
  digitalWrite(DOOR_PIN_1, value);
  digitalWrite(LED_PIN_1, value);
  
  
}

float readTemperature() {
  // Assuming the temperature sensor returns a voltage proportional to temperature
  int sensorValue = analogRead(TEMPERATURE_SENSOR_PIN);
  // Convert sensor value to temperature (adjust the formula according to your sensor)
  float temperature = (sensorValue / 1023.0) * 100.0;
  return temperature;
}

void sendLineNotify(String message) {
  HTTPClient http;
  
  // Your Line Notify API endpoint
  String url = "https://notify-api.line.me/api/notify";
  
  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(LINE_NOTIFY_TOKEN));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Message to send
  int httpResponseCode = http.POST("message=" + message);
  
  if(httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST request: ");
    Serial.println(httpResponseCode);
  }
  
  http.end(); // Free resources
}

void activateAlarm() {
  digitalWrite(ALARM_PIN, HIGH); // Turn on the alarm
  delay(5000); // Keep the alarm on for 5 seconds
  digitalWrite(ALARM_PIN, LOW); // Turn off the alarm
}

void activateFan() {
  digitalWrite(SMOKEFAN_PIN, HIGH); 
  delay(5000); 
  digitalWrite(SMOKEFAN_PIN, LOW); 
}
