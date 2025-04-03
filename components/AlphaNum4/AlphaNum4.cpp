#include "AlphaNum4.h"
#include "esp_log.h"
#include <string.h> // for memset()

static const char *TAG = "AlphaNum4";

// ──────────────────────────────────────────────────────────────────────────────
// 14-segment Font Table
// 
// This matches the `alphafonttable[]` from Adafruit’s Arduino library. Index by
// ASCII code for letters/numbers. If the display should show a custom pattern
// for other ASCII codes, you can expand or modify this array. 
//
// The 14 segments are assigned as per Adafruit's definition, with the DP (decimal
// point) bit often being the 15th bit. 
// ──────────────────────────────────────────────────────────────────────────────
static const uint16_t alphafonttable[128] = {
  // 0x00-0x0F
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,

  // 0x10-0x1F
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,

  // 0x20 ' ' (space)
  0b0000000000000000, // ' '
  0b0000000000000110, // '!' 
  0b0000001000100000, // '"'
  0b0001001011001110, // '#'
  0b0001001011101101, // '$'
  0b0000110000100100, // '%'
  0b0010001101011101, // '&'
  0b0000010000000000, // '''
  0b0010010000000000, // '('
  0b0000100100000000, // ')'
  0b0011111111000000, // '*'
  0b0001001011000000, // '+'
  0b0000100000000000, // ','
  0b0000000011000000, // '-'
  0b0100000000000000, // '.'
  0b0000110000000000, // '/'

  // 0x30 '0' ... 
  0b0000110000111111, // '0'
  0b0000000000000110, // '1'
  0b0000000011011011, // '2'
  0b0000000010001111, // '3'
  0b0000000011100110, // '4'
  0b0010000001101001, // '5'
  0b0000000011111101, // '6'
  0b0000000000000111, // '7'
  0b0000000011111111, // '8'
  0b0000000011101111, // '9'
  0b0001001000000000, // ':'
  0b0000101000000000, // ';'
  0b0010010000000000, // '<'
  0b0000000011001000, // '='
  0b0000100100000000, // '>'
  0b0001000010000011, // '?'

  // 0x40 '@'
  0b0000001010111011, // '@'
  0b0000000011110111, // 'A'
  0b0001001010001111, // 'B'
  0b0000000000111001, // 'C'
  0b0001001000001111, // 'D'
  0b0000000011111001, // 'E'
  0b0000000001110001, // 'F'
  0b0000000010111101, // 'G'
  0b0000000011110110, // 'H'
  0b0001001000001001, // 'I'
  0b0000000000011110, // 'J'
  0b0010010001110000, // 'K'
  0b0000000000111000, // 'L'
  0b0000010100110110, // 'M'
  0b0010000100110110, // 'N'
  0b0000000000111111, // 'O'
  0b0000000011110011, // 'P'
  0b0010000000111111, // 'Q'
  0b0010000011110011, // 'R'
  0b0000000011101101, // 'S'
  0b0001001000000001, // 'T'
  0b0000000000111110, // 'U'
  0b0000110000110000, // 'V'
  0b0010100000110110, // 'W'
  0b0010110100000000, // 'X'
  0b0001010100000000, // 'Y'
  0b0000110000001001, // 'Z'
  0b0000000000111001, // '['
  0b0010000100000000, // backslash
  0b0000000000001111, // ']'
  0b0000110000000011, // '^'
  0b0000000000001000, // '_'

  // 0x60 '`'
  0b0000000100000000, // '`'
  0b0001000001011000, // 'a'
  0b0010000001111000, // 'b'
  0b0000000011011000, // 'c'
  0b0000100010001110, // 'd'
  0b0000100001011000, // 'e'
  0b0000000001110001, // 'f'
  0b0000010010001110, // 'g'
  0b0001000001110000, // 'h'
  0b0001000000000000, // 'i'
  0b0000000000001110, // 'j'
  0b0011011000000000, // 'k'
  0b0000000000110000, // 'l'
  0b0001000011010100, // 'm'
  0b0001000001010000, // 'n'
  0b0000000011011100, // 'o'
  0b0000000101110000, // 'p'
  0b0000010010000110, // 'q'
  0b0000000001010000, // 'r'
  0b0010000010001000, // 's'
  0b0000000001111000, // 't'
  0b0000000000011100, // 'u'
  0b0010000000000100, // 'v'
  0b0010100000010100, // 'w'
  0b0010100011000000, // 'x'
  0b0010000000001100, // 'y'
  0b0000100001001000, // 'z'
  0b0000100101001001, // '{'
  0b0001001000000000, // '|'
  0b0010010010001001, // '}'
  0b0000010100100000, // '~'
  0b0000000000000000  // DEL (0x7F)
};

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
AlphaNum4::AlphaNum4() {
    _i2c_port = I2C_NUM_0;
    _i2c_addr = HT16K33_DEFAULT_ADDR;
    _initialized = false;
    clear();
}

// ──────────────────────────────────────────────────────────────────────────────
// begin()
//   - Configures the device, turns on oscillator, sets default blink=off, 
//     brightness=max, and clears the display
//   - Returns true if successful, false otherwise
// ──────────────────────────────────────────────────────────────────────────────
bool AlphaNum4::begin(i2c_port_t i2c_port, uint8_t addr) {
    _i2c_port = i2c_port;
    _i2c_addr = addr;
    _initialized = false;
    clear();

    // The calling code must have already called i2c_param_config() and i2c_driver_install()

    // 1) Turn on internal oscillator
    {
        uint8_t cmd = HT16K33_CMD_OSCILLATOR;
        if (i2cWrite(&cmd, 1) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to enable oscillator");
            return false;
        }
    }

    // 2) Blink off by default
    if (setBlinkRate(HT16K33_BLINK_OFF) != ESP_OK) {
        return false;
    }

    // 3) Full brightness (15)
    if (setBrightness(15) != ESP_OK) {
        return false;
    }

    // Clear any garbage
    clear();
    writeDisplay();

    _initialized = true;
    return true;
}

// ──────────────────────────────────────────────────────────────────────────────
// setBlinkRate()
//   - blink_rate can be HT16K33_BLINK_OFF (0), 1, 2, or 3
// ──────────────────────────────────────────────────────────────────────────────
esp_err_t AlphaNum4::setBlinkRate(uint8_t blink_rate) {
    if (blink_rate > 3) {
        blink_rate = 0; // Off if invalid
    }
    uint8_t cmd = HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (blink_rate << 1);
    return i2cWrite(&cmd, 1);
}

// ──────────────────────────────────────────────────────────────────────────────
// setBrightness()
//   - brightness from 0..15
// ──────────────────────────────────────────────────────────────────────────────
esp_err_t AlphaNum4::setBrightness(uint8_t brightness) {
    if (brightness > 15) brightness = 15;
    uint8_t cmd = HT16K33_CMD_BRIGHTNESS | brightness;
    return i2cWrite(&cmd, 1);
}

// ──────────────────────────────────────────────────────────────────────────────
// setDisplayOn()
//   - Actually toggles the display on/off (not just clearing buffer).
// ──────────────────────────────────────────────────────────────────────────────
esp_err_t AlphaNum4::setDisplayOn(bool on) {
    // If on == true, set display bit
    uint8_t cmd = HT16K33_BLINK_CMD;
    if (on) {
        cmd |= HT16K33_BLINK_DISPLAYON;
    }
    return i2cWrite(&cmd, 1);
}

// ──────────────────────────────────────────────────────────────────────────────
// clear()
//   - Clears local display buffer
// ──────────────────────────────────────────────────────────────────────────────
void AlphaNum4::clear(void) {
    memset(_displayBuffer, 0, sizeof(_displayBuffer));
}

// ──────────────────────────────────────────────────────────────────────────────
// writeDisplay()
//   - Copies local buffer to the HT16K33 RAM
// ──────────────────────────────────────────────────────────────────────────────
esp_err_t AlphaNum4::writeDisplay(void) {
    // 1 + 16 bytes: first byte is address pointer, then 2 bytes per row * 8 rows
    uint8_t out[1 + (ALPHANUM4_BUFFSIZE * 2)];
    memset(out, 0, sizeof(out));

    out[0] = 0x00; // HT16K33 display RAM address pointer

    // Pack each 16-bit word into 2 bytes
    for (int i = 0; i < ALPHANUM4_BUFFSIZE; i++) {
        out[1 + 2*i]     = (_displayBuffer[i] & 0xFF);
        out[1 + 2*i + 1] = (_displayBuffer[i] >> 8) & 0xFF;
    }

    return i2cWrite(out, sizeof(out));
}

// ──────────────────────────────────────────────────────────────────────────────
// writeDigitRaw()
//   - Write a raw 16-bit pattern to one of the 4 digit positions
//   - n = 0..3 (left to right), bitmask is the 14-segment + DP bits
// ──────────────────────────────────────────────────────────────────────────────
void AlphaNum4::writeDigitRaw(uint8_t n, uint16_t bitmask) {
    if (n > 3) return;
    _displayBuffer[n] = bitmask;
}

// ──────────────────────────────────────────────────────────────────────────────
// writeDigitAscii()
//   - Writes an ASCII character to digit n. If `dot` is true, also set decimal pt.
// ──────────────────────────────────────────────────────────────────────────────
void AlphaNum4::writeDigitAscii(uint8_t n, uint8_t ascii, bool dot) {
    if (n > 3) return;

    // If ASCII code is out of range of our table, show blank
    if (ascii > 127) {
        _displayBuffer[n] = 0;
        return;
    }

    uint16_t bits = alphafonttable[ascii];

    // If we want the dot (decimal point), set bit 14 (some boards use bit 14, some bit 13).
    // The Adafruit library uses (1 << 14). 
    if (dot) {
        bits |= (1 << 14);
    }
    _displayBuffer[n] = bits;
}

// ──────────────────────────────────────────────────────────────────────────────
// print()
//   - A simple convenience that writes up to 4 ASCII characters to the display
//   - If `str` is shorter than 4, remaining digits are blank
// ──────────────────────────────────────────────────────────────────────────────
void AlphaNum4::print(const char *str) {
    clear();

    for (int i = 0; i < 4; i++) {
        if (str[i] == 0) {
            break; // end of string
        }
        writeDigitAscii(i, (uint8_t)str[i], false);
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// i2cWrite()
//   - Low-level helper: writes `len` bytes in `data[]` to our I2C address
//   - We give a short 100ms timeout. You can increase if needed.
// ──────────────────────────────────────────────────────────────────────────────
esp_err_t AlphaNum4::i2cWrite(const uint8_t *data, size_t len) {
    if (!_initialized) {
        // We won’t fail, but log a warning
        // (If called before begin(), the oscillator may not be on.)
        ESP_LOGW(TAG, "i2cWrite called before init?");
    }
    return i2c_master_write_to_device(_i2c_port, _i2c_addr, data, len, pdMS_TO_TICKS(100));
}
