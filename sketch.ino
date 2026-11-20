
#define BLYNK_TEMPLATE_ID "TMPL37G5cE1P0"
#define BLYNK_TEMPLATE_NAME "Smart Energy Meter"
#define BLYNK_AUTH_TOKEN "JN1VPGYK_IzsE4_qz-hqzVozacSwYBpI"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

#define LED_PIN 2
#define BUZZER_PIN 4

const float MAX_VOLTAGE = 250.0;
const float MAX_CURRENT = 10.0;

const float OVERLOAD_LIMIT = 1500.0;
const float ELECTRICITY_RATE = 8.0;

float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
float cost = 0;

unsigned long previousMillis = 0;

BlynkTimer timer;

// Read voltage
void readVoltage() {
  int adcValue = analogRead(VOLTAGE_PIN);
  voltage = (adcValue / 4095.0) * MAX_VOLTAGE;
}

// Read current
void readCurrent() {
  int adcValue = analogRead(CURRENT_PIN);
  current = (adcValue / 4095.0) * MAX_CURRENT;
}

// Calculate power
void calculatePower() {
  power = voltage * current;
}

// Calculate energy
void calculateEnergy() {
  unsigned long currentMillis = millis();

  float elapsedHours =
      (currentMillis - previousMillis) / 3600000.0;

  energy += (power * elapsedHours) / 1000.0;

  previousMillis = currentMillis;
}

// Calculate electricity cost
void calculateCost() {
  cost = energy * ELECTRICITY_RATE;
}

// Overload detection
void checkOverload() {
  if (power > OVERLOAD_LIMIT) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// Send readings to Blynk
void sendBlynkData() {

  Blynk.virtualWrite(V0, voltage);
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, power);
  Blynk.virtualWrite(V3, energy);
  Blynk.virtualWrite(V4, cost);

  Blynk.virtualWrite(V5,
      power > OVERLOAD_LIMIT ? 1 : 0);

  Serial.println("Blynk Data Updated!");
}

// Print readings
void printReadings() {

  Serial.println("\n========== SMART ENERGY METER ==========");

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current, 2);
  Serial.println(" A");

  Serial.print("Power: ");
  Serial.print(power, 2);
  Serial.println(" W");

  Serial.print("Energy: ");
  Serial.print(energy, 6);
  Serial.println(" kWh");

  Serial.print("Electricity Cost: Rs. ");
  Serial.println(cost, 2);

  Serial.print("Overload: ");
  Serial.println(power > OVERLOAD_LIMIT ? "YES" : "NO");

  Serial.println("========================================");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  analogReadResolution(12);

  Serial.println("Connecting to WiFi...");

  WiFi.begin("Wokwi-GUEST", "");

  int attempts = 0;

while (WiFi.status() != WL_CONNECTED && attempts < 20) {
  delay(500);
  Serial.print(".");
  attempts++;
}

if (WiFi.status() == WL_CONNECTED) {
  Serial.println("\nWiFi Connected!");
} else {
  Serial.println("\nWiFi Failed!");
}

  Serial.println();
  Serial.println("WiFi Connected!");

  Blynk.config(BLYNK_AUTH_TOKEN);

  Serial.println("Connecting to Blynk...");

  if (Blynk.connect(10000)) {
    Serial.println("Blynk Connected!");
  } else {
    Serial.println("Blynk Connection Failed!");
  }

  previousMillis = millis();

  timer.setInterval(5000L, sendBlynkData);
}



void loop() {

  Blynk.run();
  timer.run();

  readVoltage();
  readCurrent();

  calculatePower();
  calculateEnergy();
  calculateCost();

  checkOverload();

  printReadings();

  delay(500);
}
