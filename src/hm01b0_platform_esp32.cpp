#include "hm01b0_platform_esp32.h"
#include "hm01b0.h"
#include <Wire.h>
#include "driver/dedic_gpio.h"

#define HM01B0_I2C_ADDR 0x24
#define HM01B0_MCLK_PIN 7

extern "C" {
    dedic_gpio_bundle_handle_t rx_bundle = NULL;
}

hm01b0_status_e esp32_mclk_ctrl(hm01b0_cfg_t* psCfg, bool enable, void* arg) {
    if (enable) {
        // BACK TO 8 MHz! The camera's internal hardware is stable again.
        ledcAttach(HM01B0_MCLK_PIN, 8000000, 1); 
        ledcWrite(HM01B0_MCLK_PIN, 1);            
    } else {
        ledcDetach(HM01B0_MCLK_PIN);
    }
    return HM01B0_ERR_OK;
}

hm01b0_status_e esp32_init(hm01b0_cfg_t* psCfg, void* arg) {
    Wire.begin(17, 18); 
    Wire.setClock(100000); 
    delay(100); 

    pinMode(HM01B0_PIN_PCLK, INPUT);
    pinMode(HM01B0_PIN_HSYNC, INPUT);
    pinMode(HM01B0_PIN_VSYNC, INPUT_PULLUP);
    
    // THE 7-BIT HACK: PCLK is now Bit 0. D1-D7 are Bits 1-7. D0 is sacrificed!
    int data_pins[] = {HM01B0_PIN_PCLK, D1_PIN, D2_PIN, D3_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN};
    for(int i = 0; i < 8; i++) pinMode(data_pins[i], INPUT_PULLUP);

    dedic_gpio_bundle_config_t bundle_config = {
        .gpio_array = data_pins,
        .array_size = 8,
        .flags = {
            .in_en = 1,      
            .in_invert = 0,  
        },
    };
    dedic_gpio_new_bundle(&bundle_config, &rx_bundle);
    
    return HM01B0_ERR_OK;
}

hm01b0_status_e esp32_i2c_write(hm01b0_cfg_t* psCfg, uint16_t reg, uint8_t *val, uint32_t len, void* arg) {
    Wire.beginTransmission(HM01B0_I2C_ADDR);
    Wire.write(reg >> 8); Wire.write(reg & 0xFF);
    for(uint32_t i=0; i<len; i++) Wire.write(val[i]);
    return (Wire.endTransmission() == 0) ? HM01B0_ERR_OK : HM01B0_ERR_I2C;
}

hm01b0_status_e esp32_i2c_read(hm01b0_cfg_t* psCfg, uint16_t reg, uint8_t *val, uint32_t len, void* arg) {
    Wire.beginTransmission(HM01B0_I2C_ADDR);
    Wire.write(reg >> 8); Wire.write(reg & 0xFF);
    if (Wire.endTransmission() != 0) return HM01B0_ERR_I2C;
    Wire.requestFrom((uint16_t)HM01B0_I2C_ADDR, (uint8_t)len);
    for(uint32_t i=0; i<len; i++) if(Wire.available()) val[i] = Wire.read();
    return HM01B0_ERR_OK;
}

hm01b0_if_t esp32_interface = {
    .init = esp32_init, .write = esp32_i2c_write, .read = esp32_i2c_read, 
    .mclk = esp32_mclk_ctrl, .trig = NULL, .deinit = NULL, .arg = NULL
};

hm01b0_cfg_t hm01b0_cfg = { .interface = &esp32_interface };