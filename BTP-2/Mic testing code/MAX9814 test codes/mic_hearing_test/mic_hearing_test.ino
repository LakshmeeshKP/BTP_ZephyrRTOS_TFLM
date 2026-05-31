const int MIC_PIN = 1; // MAX9814 Analog OUT on GPIO 1

const int SAMPLE_RATE = 16000; 
const int RECORD_TIME_SECS = 5;
const unsigned long TOTAL_SAMPLES = (unsigned long)SAMPLE_RATE * RECORD_TIME_SECS;

void setup() {
  Serial.begin(115200);
  analogSetPinAttenuation(MIC_PIN, ADC_11db);
  
  // Wait until Python sends an 'S' character to kick off recording
  while (true) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 'S') {
        break; // Got the start signal
      }
    }
    delay(10); 
  }

  unsigned long samplePeriodUs = 1000000 / SAMPLE_RATE;
  unsigned long nextSampleTime = micros();
  unsigned long samplesSent = 0;

  // Stream loop
  while (samplesSent < TOTAL_SAMPLES) {
    if (micros() >= nextSampleTime) {
      nextSampleTime += samplePeriodUs;

      int rawSample = analogRead(MIC_PIN);
      int16_t pcmSample = (rawSample - 1850) << 4; 

      Serial.write((uint8_t*)&pcmSample, sizeof(pcmSample));
      samplesSent++;
    }
  }
  Serial.flush(); 
}

void loop() {
  // Idle when finished
}