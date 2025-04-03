#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#include "AlphaNum4.h" // Your library header

// Adjust to your actual pin assignments:
#define I2C_MASTER_SDA 6
#define I2C_MASTER_SCL 7
#define I2C_MASTER_PORT I2C_NUM_0
#define I2C_MASTER_FREQ 100000 // 100kHz

extern "C" void app_main()
{
    // 1) Configure the I2C driver
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ;

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0));

    // 2) Create an AlphaNum4 object
    AlphaNum4 alpha4;

    // 3) Initialize it with our I2C port and address (usually 0x70)
    if (!alpha4.begin(I2C_MASTER_PORT, HT16K33_DEFAULT_ADDR)) {
        printf("Could not initialize alpha numeric display!\n");
        return;
    }

    printf("Display initialized successfully\n");

    // Set maximum brightness
    alpha4.setBrightness(15);
    printf("Brightness set to maximum\n");

    // Clear display first
    alpha4.clear();
    alpha4.writeDisplay();
    printf("Display cleared\n");

    while(true) {
        // Test each digit individually with a simple pattern
        // This will help us identify if there's an issue with specific digits
        for (int i = 0; i < 4; i++) {
            printf("Testing digit %d\n", i);
            alpha4.clear();
            // Light up all segments for this digit
            alpha4.writeDigitRaw(i, 0xFFFF);
            alpha4.writeDisplay();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // Now try a simple test pattern - just light up the first digit
        printf("Testing first digit only\n");
        alpha4.clear();
        alpha4.writeDigitRaw(0, 0xFFFF);  // All segments on for first digit
        alpha4.writeDisplay();
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Try writing a simple number
        printf("Writing '1234'\n");
        alpha4.clear();
        alpha4.writeDigitAscii(0, '1', false);
        alpha4.writeDigitAscii(1, '2', false);
        alpha4.writeDigitAscii(2, '3', false);
        alpha4.writeDigitAscii(3, '4', false);
        alpha4.writeDisplay();
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Try writing a different pattern
        printf("Writing 'TEST'\n");
        alpha4.clear();
        alpha4.writeDigitAscii(0, 'T', false);
        alpha4.writeDigitAscii(1, 'E', false);
        alpha4.writeDigitAscii(2, 'S', false);
        alpha4.writeDigitAscii(3, 'T', false);
        alpha4.writeDisplay();
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Now try the original sequence
        printf("Writing 'HELL'\n");
        alpha4.clear();
        alpha4.writeDigitAscii(0, 'H', false);
        alpha4.writeDigitAscii(1, 'E', false);
        alpha4.writeDigitAscii(2, 'L', false);
        alpha4.writeDigitAscii(3, 'L', false);
        alpha4.writeDisplay();
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Show "ESP!"
        printf("Writing 'ESP!'\n");
        alpha4.clear();
        alpha4.writeDigitAscii(0, 'E', false);
        alpha4.writeDigitAscii(1, 'S', false);
        alpha4.writeDigitAscii(2, 'P', false);
        alpha4.writeDigitAscii(3, '!', false);
        alpha4.writeDisplay();
        vTaskDelay(pdMS_TO_TICKS(2000));        
    }    
    
}
