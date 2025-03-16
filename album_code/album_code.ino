

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

static float volume = 0.3;

// gets called when button on bluetooth speaker is pressed
void button_handler(uint8_t id, bool isReleased){
 if (isReleased) { // Only trigger action when button is released
        Serial.print("Button ID ");
        Serial.print(id);
        Serial.println(" released");

        switch (id) {
            case 68: // PLAYPAUSE
                player.setActive(!player.isActive());
                break;
            case 69: // STOP
                player.stop();
                break;
            case 75: // NEXT
                player.next();
                break;
            case 76: // PREV
                player.previous();
                break;
            case 65: // VOL UP
                volume += 0.1;
                if (volume > 1.0) volume = 1.0; // Ensure max limit
                player.setVolume(volume);
                Serial.print("Volume Up: ");
                Serial.println(volume);
                break;
            case 66: // VOL DOWN
                volume -= 0.1;
                if (volume < 0.0) volume = 0.0; // Ensure min limit
                player.setVolume(volume);
                Serial.print("Volume Down: ");
                Serial.println(volume);
                break;
            default:
                Serial.println("Unknown button event.");
                break;
        }
    }
}


void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);  // Debug , Error  , Warning

  // setup player
  // Setting up SPI if necessary with the right SD pins by calling 
  // SPI.begin(PIN_AUDIO_KIT_SD_CARD_CLK, PIN_AUDIO_KIT_SD_CARD_MISO, PIN_AUDIO_KIT_SD_CARD_MOSI, PIN_AUDIO_KIT_SD_CARD_CS);
  player.setVolume(volume);
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