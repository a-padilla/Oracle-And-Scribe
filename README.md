# Oracle and Scribe

Repo for the Oracle OCR Glasses and Scribe Refreshable Braille Display

Senior Design Project for CEN3907

Link to [Prototype Document](https://docs.google.com/document/d/1BfPdSQ7TWxKBnTBEq4VutSzuxY3tkR3-Mizs5abLkAM/edit?usp=sharing)

## Usage:
* Oracle
  1. Run the code in the Oracle folder on a Raspberry Pi with the Bullseye OS.
  2. Attach an Arducam Motorized Focus camera and a buzzer to the Pi.
  3. Run the setup script in the Oracle/setup folder to install dependencies.
  4. Run take_pic_and_send.py to take a picture and send the text to the Scribe.

* Scribe
  1. Install the VSCode extension PlatformIO to build code for and upload code to an ESP32 dev board.
  2. Make a PlatformIO project and upload main.cpp and scribe.cpp to the src folder of the project; upload scribe.h to the include folder.
  3. Build the project.
  4. Set up breadboard with the hardware detailed in the Prototype Document linked above and pin numbers specified in Scribe.h.
  5. Upload the project to the ESP32 dev board.
