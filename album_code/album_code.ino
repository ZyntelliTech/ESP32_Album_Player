

//remove this, to find issues regarding mp3 decoding
#define HELIX_LOGGING_ACTIVE false

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
static bool isplay = true;

// Create a queue to store button events
QueueHandle_t buttonQueue;


void IRAM_ATTR button_handler(uint8_t id, bool isReleased) {
    if (isReleased) {
        xQueueSendFromISR(buttonQueue, &id, NULL);
    }
}



void processButtonEvents() {
    uint8_t id;

    // Check if there's a button event in the queue
    if (xQueueReceive(buttonQueue, &id, 0) == pdTRUE) {
        Serial.print("Processing Button ID: ");
        Serial.println(id);
        switch (id) {
            case 68: // PLAYPAUSE
                Serial.println("initiating play/pause song...");
                // player.setActive(!player.isActive());
                isplay = !isplay;
                if(isplay)
                  Serial.println("initiate playing");
                else
                  Serial.println("initiate pause");
                player.setActive(isplay);
                Serial.println("initiated play/pause song...");
                break;
            case 69: // STOP
                player.stop();
                break;
            case 75: // NEXT
                Serial.println("initiating next song...");
                player.next();
                Serial.println("initiated next song...");
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
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Error);  // Debug , Error  , Warning

  // setup player
  // Setting up SPI if necessary with the right SD pins by calling 
  // SPI.begin(PIN_AUDIO_KIT_SD_CARD_CLK, PIN_AUDIO_KIT_SD_CARD_MISO, PIN_AUDIO_KIT_SD_CARD_MOSI, PIN_AUDIO_KIT_SD_CARD_CS);
  player.setVolume(volume);
  // player.setAutoNext();
  // player.previous();
  // player.next();
  player.setAutoFade(false);
  player.play();
  player.begin();

  // setup output - We send the test signal via A2DP - so we conect to a Bluetooth Speaker
  auto cfg = out.defaultConfig(TX_MODE);
  
  
  cfg.silence_on_nodata = true; // prevent disconnect when there is no audio data
  cfg.name = "ESP32_Player";  // set the device here. Otherwise the first available device is used for output
  //cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
  out.set_button_callback(button_handler);
  out.begin(cfg);

  // Create a queue to hold button events (max 10)
    buttonQueue = xQueueCreate(10, sizeof(uint8_t));

  // BluetoothA2DPSource a2dp_source = out.source();
  // a2dp_source.set_avrc_passthru_command_callback(button_handler);


}

void loop() {
 
  player.copy();
  processButtonEvents(); // Process button events safely in the loop
}