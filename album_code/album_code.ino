#include "BluetoothA2DPSource.h"
#include <SPI.h>
#include <SD.h>
#include "mp3.h"  

BluetoothA2DPSource a2dp_source;
File sound_file;
// const char* file_name = "/audio.raw";
const char* file_name = "/audio.mp3";
const int sd_ss_pin = 5;
const int frame_size_bytes = sizeof(int16_t) * 2;



//mp3 var

mp3 MP3;                                          // MP3 Class
pcm audio;                                        // raw audio data

// callback used by A2DP to provide the sound data
int32_t get_sound_data(Frame* data, int32_t len) {
  // // the data in the file must be in int16 with 2 channels 
  // size_t result_len_bytes = sound_file.read((uint8_t*)data, len * frame_size_bytes );
  // // result is in number of frames
  // int32_t result_len = result_len_bytes / frame_size_bytes;
  // ESP_LOGD("get_sound_data", "%d -> %d",len );
  // return result_len;

//----------------
  audio = MP3.decode(); 
                         // Decode audio data into pcm class
  // data = (Frame*)audio.interleaved;
  data->channel1 = audio.interleaved[0];
  data->channel2 = audio.interleaved[1];
  int32_t result_len = audio.nBytes / frame_size_bytes;
  return result_len;
}

// Arduino Setup
void setup(void) {
  Serial.begin(115200);

  // Setup SD and open file
  SD.begin(sd_ss_pin);
  sound_file = SD.open(file_name, FILE_READ);
 if (!sound_file) {
    Serial.println("Failed to open /test.wav");
    while (1);
  }
  Serial.println("WAV file opened successfully.");

  MP3.begin(sound_file);
  audio.setSampleNumber(1);  

  // start the bluetooth
  Serial.println("starting A2DP...");
  a2dp_source.start("ESP32_Player", get_sound_data);  
  Serial.println("started A2DP...");
}

// Arduino loop - repeated processing 
void loop() {
}
