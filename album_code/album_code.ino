

//remove this, to find issues regarding mp3 decoding
#define HELIX_LOGGING_ACTIVE false

// //----CLIENT SETUP
// #define PIN_AUDIO_KIT_SD_CARD_CLK 16
// #define PIN_AUDIO_KIT_SD_CARD_MISO 28
// #define PIN_AUDIO_KIT_SD_CARD_MOSI 26
// #define PIN_AUDIO_KIT_SD_CARD_CS 25


//----LOCAL SETUP
#define PIN_AUDIO_KIT_SD_CARD_CLK 18
#define PIN_AUDIO_KIT_SD_CARD_MISO 19
#define PIN_AUDIO_KIT_SD_CARD_MOSI 23
#define PIN_AUDIO_KIT_SD_CARD_CS 5

#include "AudioTools.h"
#include "AudioTools/AudioLibs/A2DPStream.h"
#include "AudioTools/Disk/AudioSourceSDFAT.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

uint32_t prev_time = 0;
uint32_t prev_time_btn = 0;

const char *startFilePath="/";
const char* ext="mp3";
AudioSourceSDFAT source(startFilePath, ext); // , PIN_AUDIO_KIT_SD_CARD_CS);
A2DPStream out;
MP3DecoderHelix decoder;
AudioPlayer player(source, out, decoder);

static float volume = 0.3;
static bool isplay = true;
static bool tempplay = false;

// Create a queue to store button events
QueueHandle_t buttonQueue;


// void IRAM_ATTR button_handler(uint8_t id, bool isReleased) {
void button_handler(uint8_t id, bool isReleased) {
  // Serial.println("BTN event received before RELEASED -------->");
    if (isReleased) {
        Serial.println("BTN event received---------------->");
        xQueueSendFromISR(buttonQueue, &id, NULL);
    }
}



void processButtonEvents() {
    uint8_t id;
    bool file_active = false;

    // Check if there's a button event in the queue
    if (xQueueReceive(buttonQueue, &id, 0) == pdTRUE) {
        Serial.print("Processing Button ID: ");
        Serial.println(id);
        switch (id) {
            case 68: // PLAYPAUSE
                isplay = !isplay;
                if(isplay)
                  Serial.println("initiate playing");
                else
                  Serial.println("initiate pause");
                player.setActive(isplay);
                break;
            case 69: // STOP
                player.stop();
                break;
            case 75: // NEXT
                Serial.println("initiating next song...");
                file_active = player.next();
                Serial.println(file_active);
                break;
            case 76: // PREV
            Serial.println("initiating Prev song...");
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
  SPI.begin(PIN_AUDIO_KIT_SD_CARD_CLK, PIN_AUDIO_KIT_SD_CARD_MISO, PIN_AUDIO_KIT_SD_CARD_MOSI, PIN_AUDIO_KIT_SD_CARD_CS);
  player.setVolume(volume);
  // player.setAutoNext();
  // player.setAutoFade(false);
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
}

void loop() {

  player.copy();

  if(millis() - prev_time > 500){
    processButtonEvents(); // Process button events safely in the loop
    prev_time = millis();
  }

  if((!isplay) && (millis() - prev_time_btn > 500)){  
    player.setVolume(0);
    player.setActive(true);
    tempplay = true;
    prev_time_btn = millis();
  }

    if((tempplay == true) && (millis() - prev_time_btn > 2)){  
    player.setActive(false);
    tempplay = false;
    prev_time_btn = millis();

  }

  if(isplay && !player.isActive())
    isplay = false;

  if(isplay)
    player.setVolume(volume);
  
}