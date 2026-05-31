import serial
import wave
import time

# --- CONFIGURATION ---
PORT = 'COM11'
BAUD_RATE = 115200
SAMPLE_RATE = 16000
RECORD_SECS = 5
OUTPUT_FILENAME = "C:\\Users\\laksh\\Downloads\\IITB-Acads\\EE\\BTP-1\\MAX9814 test codes\\esp32_recording.wav"
# ---------------------

TOTAL_SAMPLES = SAMPLE_RATE * RECORD_SECS
TOTAL_BYTES = TOTAL_SAMPLES * 2 

print(f"Connecting to {PORT}...")
try:
    # Open port and explicitly clear hardware buffers
    ser = serial.Serial(PORT, BAUD_RATE, timeout=2)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    time.sleep(2) # Give the connection plenty of time to stabilize
    
    print("Sending start trigger to ESP32-S3...")
    ser.write(b'S') # Send the handshake trigger byte
    ser.flush()
    
    print("Recording started! Speak into the microphone...")
    raw_audio_data = bytearray()

    # Read binary blocks safely
    while len(raw_audio_data) < TOTAL_BYTES:
        if ser.in_waiting > 0:
            chunk = ser.read(ser.in_waiting)
            raw_audio_data.extend(chunk)
            
            progress = (len(raw_audio_data) / TOTAL_BYTES) * 100
            print(f"Progress: {min(progress, 100):.1f}%", end='\r')

    print("\nRecording finished! Packaging into WAV format...")
    ser.close()

    # Write the WAV structure
    with wave.open(OUTPUT_FILENAME, 'wb') as wav_file:
        wav_file.setnchannels(1)      
        wav_file.setsampwidth(2)      
        wav_file.setframerate(SAMPLE_RATE)
        wav_file.writeframes(raw_audio_data)

    print(f"Success! Saved as '{OUTPUT_FILENAME}'")

except serial.SerialException as e:
    print(f"\nSerial Error: {e}")
