# ESP32_Album_Player

### Description

This is a Bluetooth Audio Player using ESP32. The system includes an Audio Player and Album card using ESP32. The Album card consists of an ESP32, an SD card slot, and a Wireless charger. The Audio Player consists of ESP32, Audio Stereo DAC, Wireless Power Transmitter, LEDs, and Switches. The Album will be powered by the Wireless Charger supplied by the Audio Player. The Album will read audio files from the SD card and transfer audio streaming to the Audio Player over Bluetooth LE. The Audio Player will receive the Bluetooth streaming from the Album and output it over the stereo microphone. If users take Album away from the Audio Player, the audio streaming will be stopped because the Wireless charging is off.