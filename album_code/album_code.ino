

//remove this, to find issues regarding mp3 decoding
// #define HELIX_LOGGING_ACTIVE false

#include "AudioTools.h"
#include "AudioTools/AudioLibs/A2DPStream.h"
#include "AudioTools/Disk/AudioSourceSDFAT.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

const char *startFilePath="/";
const char* ext="mp3";
AudioSourceSDFAT source(startFilePath, ext); // , PIN_AUDIO_KIT_SD_CARD_CS);
A2DPStream out;
MP3DecoderHelix decoder;
AudioPlayer player(source, out, decoder);

// gets called when button on bluetooth speaker is pressed
void button_handler(uint8_t id, bool isReleased){
  if (isReleased) {
    Serial.print("button id ");
    Serial.print(id);
    Serial.println(" released");
  }
}


void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);  // Debug , Error  , Warning

  // setup player
  // Setting up SPI if necessary with the right SD pins by calling 
  // SPI.begin(PIN_AUDIO_KIT_SD_CARD_CLK, PIN_AUDIO_KIT_SD_CARD_MISO, PIN_AUDIO_KIT_SD_CARD_MOSI, PIN_AUDIO_KIT_SD_CARD_CS);
  player.setVolume(0.3);
  // player.setAutoNext();
  // player.previous();
  // player.next();
  player.play();
  player.begin();

  // setup output - We send the test signal via A2DP - so we conect to a Bluetooth Speaker
  auto cfg = out.defaultConfig(TX_MODE);
  
  
  cfg.silence_on_nodata = true; // prevent disconnect when there is no audio data
  cfg.name = "ESP32_Player";  // set the device here. Otherwise the first available device is used for output
  //cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
  out.set_button_callback(button_handler);
  out.begin(cfg);
  // BluetoothA2DPSource a2dp_source = out.source();
  // a2dp_source.set_avrc_passthru_command_callback(button_handler);


}

void loop() {
  player.copy();
}