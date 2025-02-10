// Constants for signal processing
const int BUFFER_SIZE = 25;     // Increased buffer for better detection
const int QRS_THRESHOLD = 100;  // Minimum amplitude for QRS detection
const int RR_MIN = 200;         // Minimum interval between R peaks (in ms)
const int RR_MAX = 1200;        // Maximum interval between R peaks (in ms)
const float ALPHA = 0.1;        // EMA filter constant

// Variables for signal processing
int readings[BUFFER_SIZE];
int readIndex = 0;
long total = 0;
float filteredValue = 0;
float prevValue = 0;

// Variables for QRS detection
unsigned long lastRPeak = 0;  // Timestamp of last R peak
int RPeakValue = 0;           // Value of last R peak
bool QRSDetected = false;     // Flag for QRS detection
int baselineValue = 0;        // Dynamic baseline value
int noiseLevel = 0;           // Estimated noise level

void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT);
  pinMode(11, INPUT);

  // Initialize arrays
  for (int i = 0; i < BUFFER_SIZE; i++) {
    readings[i] = 0;
  }
}

float filterSignal(int rawReading) {
  // Remove baseline wander
  total = total - readings[readIndex];
  readings[readIndex] = rawReading;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % BUFFER_SIZE;

  // Moving average
  float avgValue = total / BUFFER_SIZE;

  // High-pass filter to emphasize QRS complex
  float highPassValue = rawReading - avgValue;

  // Exponential moving average for smoothing
  filteredValue = (ALPHA * highPassValue) + ((1 - ALPHA) * prevValue);
  prevValue = filteredValue;

  return filteredValue;
}

bool detectQRSComplex(float filteredValue) {
  static int peakCount = 0;
  static unsigned long currentTime;
  currentTime = millis();

  // Update baseline and noise estimates
  if (filteredValue > baselineValue) {
    baselineValue += (filteredValue - baselineValue) / 16;
  } else {
    noiseLevel += (abs(filteredValue - baselineValue) - noiseLevel) / 32;
  }

  // Dynamic threshold based on signal and noise levels
  int threshold = baselineValue + max(QRS_THRESHOLD, noiseLevel * 2);

  // Check for R peak
  if (filteredValue > threshold) {
    unsigned long RR_interval = currentTime - lastRPeak;

    // Verify if this could be a valid R peak
    if (RR_interval > RR_MIN && RR_interval < RR_MAX) {
      if (filteredValue > RPeakValue) {
        RPeakValue = filteredValue;
        QRSDetected = true;
        lastRPeak = currentTime;
        return true;
      }
    }
  } else {
    RPeakValue = 0;  // Reset peak value for next detection
  }

  return false;
}

void loop() {
  if ((digitalRead(10) == 1) || (digitalRead(11) == 1)) {
    Serial.println('!');
    return;
  }

  int rawReading = analogRead(A0);
  float filteredValue = filterSignal(rawReading);

  // Detect QRS complex
  if (detectQRSComplex(filteredValue)) {
    Serial.print("R");  // Mark R peak detection
    Serial.print(",");
  }

  // Output filtered value and detection status
  Serial.println(filteredValue);

  delay(50);  // Increased sampling rate for better detection
}
