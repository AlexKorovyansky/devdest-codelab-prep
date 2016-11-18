#include <Arduino.h>
#include <Ultrasonic.h>
#include <Grove_LED_Bar.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <FirebaseArduino.h>
#include <HealthTracker.h>

#define DEFAULT_TIME_TO_DISCHARGE 1 * 60 * 1000
#define DEFAULT_TIME_TO_RECHARGE 30 * 1000
#define WIFI_SSID "gnet_home"
#define WIFI_PASSWORD "jn104Hi103veg"
#define FIREBASE_HOST "codelab1-ed543.firebaseio.com"
#define FIREBASE_AUTH "FkGmitQaMh1jV5QNvUuXBiE1HjXUd2eNXQH7fUen"

Ultrasonic ultrasonic = Ultrasonic(14);
Grove_LED_Bar groveled_bar = Grove_LED_Bar(13, 12, 0);
HealthTracker health_tracker = HealthTracker();

void setup() {
  Serial.begin(28800);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  groveled_bar.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi: connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi: connected.");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  unsigned time_to_discharge = Firebase.getInt("time_to_discharge");
  if (time_to_discharge <= 0) {
    time_to_discharge = DEFAULT_TIME_TO_DISCHARGE;
  }
  unsigned time_to_recharge = Firebase.getInt("time_to_recharge");
  if (time_to_recharge <= 0) {
    time_to_recharge = DEFAULT_TIME_TO_RECHARGE;
  }
  health_tracker.Configure(time_to_discharge, time_to_recharge);
}

void loop() {
  float health = health_tracker.Tick(ultrasonic.MeasureInCentimeters() < 10);
  groveled_bar.setLevel(health * 10.0f);
  Firebase.setFloat("health", health);
  delay(200);
}
