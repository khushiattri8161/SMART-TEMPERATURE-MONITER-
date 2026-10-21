#define BLYNK_TEMPLATE_ID "TMPL3ny_l00a_"
#define BLYNK_TEMPLATE_NAME "Smart Temperature Moniter"
#define BLYNK_AUTH_TOKEN "ZPWOqGv2n32yAkPAOGgitHd-27Mk4aPk"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Wi-Fi credentials
char ssid[] = "ESP32TEST";
char pass[] = "@khushi#%";

// DHT22 configuration
#define DHTPIN 4
#define DHTTYPE DHT11

// LED and buzzer pins
#define GREEN_LED 18
#define YELLOW_LED 19
#define RED_LED 21
#define BUZZER 23

// Temperature thresholds
#define WARNING_TEMP 30.0
#define CRITICAL_TEMP 35.0

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Prevent repeated alerts while status remains unchanged
int lastStatus = -1;

// Send readings and update status
void sendSensorData() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check sensor readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT22 reading failed!");
    return;
  }

  // Display readings in Serial Monitor
  Serial.println("-------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send readings to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);

  int currentStatus;

  // NORMAL CONDITION
  if (temperature <= WARNING_TEMP) {

    currentStatus = 0;

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    Blynk.virtualWrite(V2, "NORMAL");

    Serial.println("Status: NORMAL");
  }

  // WARNING CONDITION
  else if (temperature <= CRITICAL_TEMP) {

    currentStatus = 1;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    Blynk.virtualWrite(V2, "WARNING");

    Serial.println("Status: WARNING");
  }

  // CRITICAL CONDITION
  else {

    currentStatus = 2;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    // Buzzer sounds during critical temperature
    digitalWrite(BUZZER, HIGH);

    Blynk.virtualWrite(V2, "CRITICAL");

    Serial.println("Status: CRITICAL");
  }

  // Trigger an event only when status changes
  if (currentStatus != lastStatus) {

    if (currentStatus == 0) {
      Blynk.logEvent(
        "normal_temp",
        "Temperature is back to normal.");
    }

    else if (currentStatus == 1) {
      Blynk.logEvent(
        "warning_temp",
        "Warning! Temperature has increased.");
    }

    else {
      Blynk.logEvent(
        "critical_temp",
        "CRITICAL! Temperature exceeded 35 degrees Celsius.");
    }

    lastStatus = currentStatus;
  }
}

// Setup
void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 STARTED ");

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  dht.begin();

  Serial.println("Smart Temperature Monitoring System");

  // Connect to Blynk and Wi-Fi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Read sensor every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

// Main loop
void loop() {

  Blynk.run();
  timer.run();
}