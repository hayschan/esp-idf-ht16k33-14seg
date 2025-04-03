#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c.h"

// ──────────────────────────────────────────────────────────────────────────────
// I2C address for HT16K33-based alpha-numeric display (default 0x70 if all
// address pins are tied low).
// ──────────────────────────────────────────────────────────────────────────────
#define HT16K33_DEFAULT_ADDR      0x70

// ──────────────────────────────────────────────────────────────────────────────
// HT16K33 Commands
// ──────────────────────────────────────────────────────────────────────────────
#define HT16K33_CMD_OSCILLATOR    0x21   // Turn on internal oscillator
#define HT16K33_BLINK_CMD         0x80
#define HT16K33_BLINK_DISPLAYON   0x01
#define HT16K33_BLINK_OFF         0
#define HT16K33_BLINK_2HZ         1
#define HT16K33_BLINK_1HZ         2
#define HT16K33_BLINK_HALFHZ      3
#define HT16K33_CMD_BRIGHTNESS    0xE0   // 0xE0 to 0xEF

// ──────────────────────────────────────────────────────────────────────────────
// The HT16K33 has 8 rows of 16 bits each (128 bits total). For the 4-character
// alpha display, we only use the lower 4 words (indices [0..3]) to store the
// segment data. Each word is 14 or 15 bits of actual segments + optional DP bit.
// ──────────────────────────────────────────────────────────────────────────────
#define ALPHANUM4_BUFFSIZE        8 // We'll store 8 to match the HT16K33 row count

// ──────────────────────────────────────────────────────────────────────────────
// Class: AlphaNum4
//
// A minimal driver for the Adafruit 14-segment alphanumeric display backpack
// using the HT16K33. This is adapted for ESP-IDF, removing Arduino dependencies.
// ──────────────────────────────────────────────────────────────────────────────
class AlphaNum4 {
public:
    // Constructor (no arguments needed)
    AlphaNum4();

    // Initialize the display
    //  - i2c_port: which ESP-IDF I²C port (commonly I2C_NUM_0 or I2C_NUM_1)
    //  - addr:     7-bit I2C address (default 0x70)
    bool begin(i2c_port_t i2c_port = I2C_NUM_0, uint8_t addr = HT16K33_DEFAULT_ADDR);

    // Set blink rate (HT16K33_BLINK_OFF, 1, 2, 3)
    esp_err_t setBlinkRate(uint8_t blink_rate);

    // Set brightness (0..15)
    esp_err_t setBrightness(uint8_t brightness);

    // Turn the entire display on or off
    esp_err_t setDisplayOn(bool on);

    // Clear our local display buffer (does NOT automatically write)
    void clear(void);

    // Write the entire display buffer to the hardware
    esp_err_t writeDisplay(void);

    // Set one character’s raw 16-bit value
    //  - n = [0..3], the character index from left to right
    //  - bitmask = the raw bits that define the segments
    void writeDigitRaw(uint8_t n, uint16_t bitmask);

    // Write one ASCII character to position n, optionally lighting the decimal point
    //  e.g. writeDigitAscii(0, 'A', true) -> writes 'A' at leftmost digit, with decimal lit
    void writeDigitAscii(uint8_t n, uint8_t ascii, bool dot = false);

    // Optional: if you want a convenience function to fill all 4 digits as ASCII
    // you can provide your own method. Here’s an example "print" method:
    void print(const char *str);

private:
    i2c_port_t _i2c_port;   // which ESP-IDF port (0 or 1)
    uint8_t    _i2c_addr;   // 7-bit address
    bool       _initialized;

    // The HT16K33 has 8 words of display memory; we’ll store them all, even though
    // the alpha-numeric only uses four. Each entry is up to 16 bits.
    uint16_t   _displayBuffer[ALPHANUM4_BUFFSIZE];

    // Low-level helper to write a small buffer to the device
    esp_err_t i2cWrite(const uint8_t *data, size_t len);
};

