#include <BluetoothA2DPSink.h>
#include "ESP_I2S.h"

// Define I2S pins for the external MAX98357A DAC
#define I2S_BCK  27    // Bit Clock
#define I2S_WS   14    // Word Select (LR Clock)
#define I2S_DOUT 26    // Data Out



// Define Player Control Buttons
#define PLAY_PAUSE  36
#define PREV        39
#define NEXT        34
#define STOP        35
#define VOL_UP      32
#define VOL_DOWN    33



// Temp
// #define PLAY_PAUSE  32
// #define PREV        35
// #define NEXT        34
// #define STOP        33
// #define VOL_UP      36
// #define VOL_DOWN    39

#define NUM_BUTTONS 6

//----------------------------

I2SClass i2s;
BluetoothA2DPSink a2dp_sink(i2s);

// ----Button Functions-------
// Define Player Control Buttons
const int buttonPins[NUM_BUTTONS] = {PLAY_PAUSE, PREV, NEXT, STOP, VOL_UP, VOL_DOWN};
void togglePlayPause();
void playPrevious();
void playNext();
void stopPlayback();
void volumeUp();
void volumeDown();

// Function pointers for respective actions
void (*buttonActions[NUM_BUTTONS])() = {
    togglePlayPause, playPrevious, playNext, stopPlayback, volumeUp, volumeDown
};

// Button state tracking
bool prevButtonStates[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastPressTimes[NUM_BUTTONS] = {0};

// Debounce time
const unsigned long debounceDelay = 400;
//----------------------------

// Flag to track playback state
bool isPaused = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 A2DP Player");

  // Configuring Player's Buttons as INPUT with internal pull-up
  pinMode(PLAY_PAUSE, INPUT_PULLUP);
  pinMode(PREV, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DOWN, INPUT_PULLUP);

  // Configure I2S pins for external DAC (MAX98357A)
  i2s.setPins(I2S_BCK, I2S_WS, I2S_DOUT);
  if (!i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // Halt execution
  }



  // Start the Bluetooth A2DP sink
  a2dp_sink.start("ESP32_Player");
  delay(1000);  //temp delay
  Serial.println("Bluetooth A2DP Sink Started!");
}

void loop() {
  checkButtons();
}


void checkButtons() {
    unsigned long currentMillis = millis();
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        bool currState = digitalRead(buttonPins[i]);

        if (prevButtonStates[i] == HIGH && currState == LOW && 
            (currentMillis - lastPressTimes[i] > debounceDelay)) {
            
            lastPressTimes[i] = currentMillis; // Update debounce time
            buttonActions[i](); // Call the respective function
        }

        prevButtonStates[i] = currState; // Update previous state
    }
}

// Play/Pause toggle function
void togglePlayPause() {
    // if (isPaused) {
    //     Serial.println("Playing...");
    //     a2dp_sink.play();
    // } else {
    //     Serial.println("Pausing...");
    //     a2dp_sink.pause();
    // }
    // isPaused = !isPaused;
    Serial.println("Playing/Pause...");
    a2dp_sink.play();
}

void playPrevious() {
    Serial.println("Previous Track...");
    a2dp_sink.previous();
}

void playNext() {
    Serial.println("Next Track...");
    a2dp_sink.next();
}

void stopPlayback() {
    Serial.println("Stopping...");
    a2dp_sink.stop();
}

void volumeUp() {
    Serial.println("Volume Up...");
    a2dp_sink.volume_up();
}

void volumeDown() {
    Serial.println("Volume Down...");
    a2dp_sink.volume_down();
}
