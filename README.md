# HM01B0_Camera_Driver_ESP32S3
ESP32 HM01B0 Camera Driver
By Saras Sardessai

SparkFun_HM01B0_Camera_ArduinoLibrary modified to work on ESP32 S3 dev board.
The original library here:
https://github.com/sparkfun/SparkFun_HM01B0_Camera_ArduinoLibrary/blob/master/src/hm01b0.c

Hardware Setup on ESP32 S3 dev Board:
SDA 17
SCL 18
MCLK 7
VSYNC 3
HSYNC 5
PCLK 4
D0 1
D1 2
D2 6
D3 15
D4 16
D5 21
D6 47
D7 48

SOME SPECIFICS:
1: 7-bit hack
-- At 8 MHz, a pixel arrives every ~30 CPU clock cycles. Standard digitalRead() or even raw GPIO register reads over the APB bus are too slow and will drop pixels.
To fix this, we utilize the ESP32's dedic_gpio (Dedicated GPIO) which reads pins in exactly 1 clock cycle. Because the ESP32 only has 8 dedicated slots, we sacrifice the D0 data wire (dropping the image from 8-bit to 7-bit grayscale) and map PCLK into that empty slot. This allows the CPU to read the clock trigger and the pixel data simultaneously with zero latency.

2: PingPong USB Streaming
-- Blasting a continuous 2,000,000 baud data stream chokes the computer's USB buffer, resulting in dropped bytes and diagonal image tearing. This driver uses a Ping-Pong approach: The Python script sends a 'c' command, the ESP32 captures exactly one 324x244 frame, transmits it, and waits for the next command.

3: Software Debouncing
-- We bypass the hardware interrupt entirely and use a 10,000-loop software debounce on the raw silicon register to guarantee a clean frame lock.


🚀 Quick Start Guide
1. Flash the Microcontroller
--Open the .ino project in the Arduino IDE.
--Ensure you have the ESP32 board definitions installed.
--Upload the code to your ESP32.

2. Setup the Python Environment
Ensure you have Python 3 installed, then install the required libraries:
-->
pip install opencv-python pyserial numpy

3. Run the viewer
-- Open vidoutput.py (or whatever you named the Python script).
--Update the PORT variable to match your ESP32's COM port (e.g., COM3 on Windows, or /dev/cu.usbserial-XXXX on Mac).
--Run the script.

