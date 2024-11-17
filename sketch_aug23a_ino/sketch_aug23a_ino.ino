#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000
#define MAX_SAMPLE_COUNT 60
#define MIN_THRESHOLD_LEVEL 1.0f
// Create a PulseOximeter object
PulseOximeter pox;
int samplecount = 0;
uint32_t tsLastReport = 0;
double heart_rate = 72.0;
double spo2 = 98.0;
// double avg_heart_rate =0;
// double avg_spo2=0;
bool INITIALIZED = false;
// Callback routine is executed when a pulse is detected
void onBeatDetected() {
  // Serial.println("Beat!");
}

void setup() {
  Serial.begin(9600);
  while (!INITIALIZED) {
    Serial.print("Initializing pulse oximeter..");
    // Initialize sensor
    if (!pox.begin()) {
      Serial.println("FAILED");
    } else {
      Serial.println("SUCCESS");
      INITIALIZED = true;
    }
  }


  // Configure sensor to use 7.6mA for LED drive
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback routine
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  // Read from the sensor
  if (samplecount < MAX_SAMPLE_COUNT) {
    pox.update();

    // Grab the updated heart rate and SpO2 levels
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      double delta_heart_rate = pox.getHeartRate();
      double delta_spo2 = pox.getSpO2();
      // Calculating running average
      heart_rate = delta_heart_rate * 0.1 + 0.9 * heart_rate;
      spo2 = delta_spo2 * 0.1 + spo2 * 0.9;

      if (delta_spo2 > MIN_THRESHOLD_LEVEL && delta_heart_rate > MIN_THRESHOLD_LEVEL) {
        // Serial.print("Heart rate:");
        // Serial.print(heart_rate);
        // Serial.print("bpm / SpO2:");
        // Serial.print(spo2);
        // Serial.println("%");
        // avg_heart_rate+=(heart_rate/60);
        // avg_spo2+=(spo2/60);
        // ++samplecount;
        Serial.println(++samplecount);
      }
      tsLastReport = millis();
    }
  } else if (samplecount == MAX_SAMPLE_COUNT) {
    samplecount++;
    Serial.print(heart_rate);
    Serial.print(" : ");
    Serial.print(spo2);
  }
}
