#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000

// Create a PulseOximeter object
PulseOximeter pox;

// Time at which the last beat occurred
uint32_t tsLastReport = 0;
double heart_rate =72.0;
double spo2=98.0;
// Callback routine is executed when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(9600);

    Serial.print("Initializing pulse oximeter..");

    // Initialize sensor
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;!pox.begin(););
    } else {
        Serial.println("SUCCESS");
    }

	// Configure sensor to use 7.6mA for LED drive
	pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback routine
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
    // Read from the sensor
    pox.update();

    // Grab the updated heart rate and SpO2 levels
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
       double delta_heart_rate=pox.getHeartRate();
       double delta_spo2=pox.getSpO2();
       heart_rate=delta_heart_rate*0.1+0.9*heart_rate;
        spo2=delta_spo2*0.1+spo2*0.9;

        if(delta_spo2 > 1.0f && delta_heart_rate > 1.0f ){
          Serial.print("Heart rate:");
          Serial.print(heart_rate);
          Serial.print("bpm / SpO2:");
          Serial.print(spo2);
          Serial.println("%");
        }
        else{
          heart_rate =72.0;
          spo2=98.0;
        }
       

       tsLastReport = millis();
    }
}



