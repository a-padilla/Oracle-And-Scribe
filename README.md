# Oracle, Scribe, and Alice

Members:
- Austin
- Bradley
- Torie
- Anthony
- Richie

Repo for the Oracle OCR Glasses and Scribe Refreshable Braille Display

Senior Design Project for CEN3907

Link to [Production Release Document](https://docs.google.com/document/d/1HyzxnghGku2yloXA5wzwHSxyDODf-Q1hCP8bEuT-k8g/edit#heading=h.no4vya8h2jww)

## Usage:
* Oracle
  1. Run the code in the Oracle folder on a Raspberry Pi with the Bullseye OS.
  2. Attach an Arducam Motorized Focus camera and a buzzer to the Pi.
  3. Run the setup script in the Oracle/setup folder to install dependencies.
  4. Run take_pic_and_send.py to take a picture and send the text to the Scribe.

* Scribe
  1. Install the VSCode extension PlatformIO to build code for and upload code to an ESP32 dev board, and install the Arduino IDE to program the Arduino Uno.
  2. Upload the PlatformIO project, 'scribe', to an ESP32.
  3. Upload the arduino program to an Arduino Uno.
  4. Wire up the Arduino Uno, ESP32, PCA9865s, servos, and buttons as shown in the Production Release Document.
  5. Power the device.

* Scribe
  1. On a computer with python, the pybluez module, and a bluetooth adapter, connect the computer to the Scribe via bluetooth.
  2. Run NVDA with the [Speech Logger Addon](https://addons.nvda-project.org/addons/speechLogger.en.html), and set the output to a text file in the Alice folder named "sample.txt"
  3. Run the send_to_scribe.py script in the Alice folder
  4. When connected, press c to capture text from the file and send it to the Scribe.
