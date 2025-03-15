#include <BluetoothA2DPSink.h>
// #include <driver/i2s.h>
#include "ESP_I2S.h"

// Define I2S pins for the external MAX98357A DAC
#define I2S_BCK  27    // Bit Clock
#define I2S_WS   14    // Word Select (LR Clock)
#define I2S_DOUT 26    // Data Out

I2SClass i2s;
// Create an instance of the A2DP Sink
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 A2DP Player");

  // Configure I2S pins for external DAC (MAX98357A)
  i2s.setPins(I2S_BCK, I2S_WS, I2S_DOUT);
    if (!i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
      Serial.println("Failed to initialize I2S!");
      while (1); // do nothing
    }

  // Start the Bluetooth A2DP sink with the chosen device name.
  a2dp_sink.start("ESP32_Player");

  Serial.println("Bluetooth A2DP Sink Started!");
}

void loop() {
  // The A2DP library handles incoming audio streaming automatically.

}