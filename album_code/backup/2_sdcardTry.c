#include "BluetoothA2DPSource.h"
#include <math.h> 
#include <SPI.h>
#include <SD.h>


//---------_SD card setting

#define SD_CS_PIN 5   
File wavFile;  



// #define c3_frequency  130.81

BluetoothA2DPSource a2dp_source;

/*
 * get_sound_data
 *
 * This callback is called by the A2DP library when it needs PCM data.
 * It reads raw PCM data from the WAV file.
 *
 * Note: We assume the WAV file is in 44.1kHz, stereo, 16-bit format.
 *       The first 44 bytes are skipped (WAV header).
 *
 * Returns the number of frames provided (1 frame = 4 bytes).
 */
int32_t get_sound_data(uint8_t *data, int32_t byteCount) {
  int bytesRead = wavFile.read(data, byteCount);
  
  // If we reach the end of the file, loop back to the start of audio data.
  if (bytesRead < byteCount) {
    // Rewind: skip the 44-byte header again
    wavFile.seek(44);
    int extra = wavFile.read(data + bytesRead, byteCount - bytesRead);
    bytesRead += extra;
  }
  
  // Each frame is 4 bytes (2 channels * 16 bits per channel)
  return bytesRead / 4;
}


void setup() {
  Serial.begin(115200);


//-------_SD card init
  // Initialize the SD card using SPI
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized.");

  // Open the WAV file (ensure a file named "test.wav" exists on the SD card)
  wavFile = SD.open("/test.wav");
  if (!wavFile) {
    Serial.println("Failed to open /test.wav");
    while (1);
  }
  Serial.println("WAV file opened successfully.");

  // Skip the 44-byte header of the WAV file
  wavFile.seek(44);


  //-------A2DP init--------------




  Serial.println("Starting ESP32 A2DP Album");
  a2dp_source.set_auto_reconnect(false);
  a2dp_source.set_data_callback(get_sound_data);
  // a2dp_source.set_data_callback_in_frames(get_data_frames);
  a2dp_source.set_volume(60);
  a2dp_source.start("ESP32_Player"); 
  Serial.println("Bluetooth A2DP Album Started!"); 
}

void loop() {
  // to prevent watchdog in release > 1.0.6
  // delay(1000);
}
