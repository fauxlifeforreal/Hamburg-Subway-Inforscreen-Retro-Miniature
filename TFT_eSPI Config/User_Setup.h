//                            USER DEFINED SETTINGS
// HamburgSubwayDisplay hardware setup
// Raspberry Pi Pico 2 / RP2350 + Waveshare Pico-ResTouch-LCD-3.5
// 3.5 inch ILI9488 480x320 SPI display-only configuration.

#define USER_SETUP_INFO "HamburgSubwayDisplay Pico-ResTouch-LCD-3.5 ILI9488"
#define DISABLE_ALL_LIBRARY_WARNINGS

#define RPI_DISPLAY_TYPE
#define ILI9488_DRIVER

// TFT_eSPI's ILI9488 driver provides the 480x320 dimensions.
// Do not define TFT_WIDTH/TFT_HEIGHT here, to avoid redefinition warnings.

#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 10
#define TFT_CS    9
#define TFT_DC    8
#define TFT_RST  15
#define TFT_BL   13
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define TFT_SPI_PORT 1
#define SPI_FREQUENCY       7000000
#define SPI_READ_FREQUENCY  2000000

#define SUPPORT_TRANSACTIONS
