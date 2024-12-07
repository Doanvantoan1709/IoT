#define BLYNK_TEMPLATE_ID "TMPL69odiICtd"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "v1pQHXX8Zxk6K9rd1yQz8kzjtanzz4QQ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Iphone";
char pass[] = "12343210";

const int sensorPin = D6;    
const int buzzerPin = D8;  
const int pumpIN3 = D7;    
const int physicalButtonPin = D5;

bool autoMode = false;     
bool buttonState = false;   
int sensorValue = 0;        
bool lastPhysicalButtonState = HIGH; 

BlynkTimer timer;

unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void controlPumpAndBuzzer(bool state) {
  digitalWrite(pumpIN3, state);
  digitalWrite(buzzerPin, state);
}

void checkSensor() {
  sensorValue = digitalRead(sensorPin);

  if (sensorValue == 0) {
    Blynk.virtualWrite(V3, "Fire Detection");
  } else {
    Blynk.virtualWrite(V3, "Safe");
  }

  if (autoMode) {
    controlPumpAndBuzzer(sensorValue == 0);
  } else {
    controlPumpAndBuzzer(buttonState);
  }

  notifyOnFireAlert();
}

BLYNK_WRITE(V1) {
  autoMode = param.asInt();
}

BLYNK_WRITE(V2) {
  buttonState = param.asInt();
}

static void handlePhysicalButton() {
  bool physicalButtonState = digitalRead(physicalButtonPin);
  if (physicalButtonState == LOW && lastPhysicalButtonState == HIGH) {
    autoMode = false;  
    Blynk.virtualWrite(V1, 0);
    buttonState = !buttonState;
    controlPumpAndBuzzer(buttonState);
  }

  lastPhysicalButtonState = physicalButtonState;
}

void notifyOnFireAlert() {
  if (sensorValue == 0) {
    Serial.println("Warning your area is on fire!!!");
    Blynk.virtualWrite(V4, "Fire Alert in Home");
    Blynk.logEvent("fire_warning", "Warning your area is on fire!!!");
  } 
  else {
    Blynk.virtualWrite(V4, "Home is Safe");
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pumpIN3, OUTPUT);
  pinMode(physicalButtonPin, INPUT_PULLUP);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, checkSensor);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
  timer.run();
  handlePhysicalButton();
}
