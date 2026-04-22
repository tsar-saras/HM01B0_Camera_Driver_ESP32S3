#include "hm01b0_arduino.h"
#include "hm01b0_platform_esp32.h" 

HM01B0 camera;

void setup() {
  Serial.setTxBufferSize(8192);
  Serial.begin(2000000);
  delay(1000);

  if (camera.begin() != HM01B0_ERR_OK) {
    while (1); 
  }

  hm01b0_set_mode(&hm01b0_cfg, HM01B0_REG_MODE_SELECT_STREAMING, 0);
  hm01b0_cmd_update(&hm01b0_cfg);
}

void loop() {
  // Ping-Pong: Wait for Python to send 'c'
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'c') {
      
      // FREEZE THE OS: Do not let the ESP32 look away for any reason
      portDISABLE_INTERRUPTS();
      
      // Grab exactly 324x244
      hm01b0_blocking_read_oneframe(&hm01b0_cfg, camera.frameBuffer, 79056);
      
      // UNFREEZE THE OS
      portENABLE_INTERRUPTS();

      // Blast the frame to Python
      Serial.print("*RDY*");
      Serial.write(camera.frameBuffer, 79056);
    }
  }
}