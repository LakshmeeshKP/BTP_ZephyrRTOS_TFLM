// Test code for MAX9814 on ESP32-S3
const int MIC_PIN = 1;          // GPIO 1 (ADC1_CH0)
const int SAMPLE_WINDOW = 50;   // Sample window width in milliseconds (50ms = 20Hz)

void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for Serial to initialize
  
  // Configure ADC attenuation for full 0-3.3V range
  analogSetPinAttenuation(MIC_PIN, ADC_11db); 
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int signalMax = 0;
  unsigned int signalMin = 4095; // ESP32-S3 ADC is 12-bit (0-4095)

  // Collect data for 50 milliseconds
  while (millis() - startMillis < SAMPLE_WINDOW) {
    unsigned int sample = analogRead(MIC_PIN);

    if (sample < 4095) { // Toss out occasional spurious high readings
      if (sample > signalMax) {
        signalMax = sample; // Save the highest peak
      }
      else if (sample < signalMin) {
        signalMin = sample; // Save the lowest trough
      }
    }
  }

  // Peak-to-Peak amplitude
  unsigned int peakToPeak = signalMax - signalMin;
  
  // Convert to voltage (Approximate)
  double volts = (peakToPeak * 3.3) / 4095.0;

  // Output to Arduino Serial Plotter
  // Blue line is your current audio volume spike, Red line is the raw amplitude 
  Serial.print("Raw_Amplitude:");
  Serial.print(peakToPeak);
  Serial.print(",");
  Serial.print("Volts_p-p:");
  Serial.println(volts * 1000); // Scaled up to easily view alongside raw units
}