/*
  Hamburg Subway Departure Board
  Raspberry Pi Pico 2 + Waveshare Pico-ResTouch-LCD-3.5 style SPI TFT

  Display initialization is intentionally copied from the known-good
  TFT_eSPI Graph_2.ino example. Do not add sketch-local TFT pin defines here;
  the working hardware setup comes from the TFT_eSPI User_Setup.h used by
  Graph_2.ino.
*/

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();                   // Invoke custom library with default width and height

int currentScreen = 0;
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49

#define BOARD_BG      0x0000
#define HEADER_BG     0xF800
#define AMBER_TEXT    0xFBE0
#define GREEN_LINE    0x07E0
#define U1_COLOR      0x07E0
#define U2_COLOR      0xF800
#define U3_COLOR      0x000F
#define U4_COLOR      0xFB60
#define S_GREEN       0x07E0
#define S5_COLOR      0x8010

struct Departure {
  const char *line;
  const char *destination;
  const char *via;
  const char *timeText;
  uint16_t lineColor;
};

Departure departure = {
  "U2",
  u8"MÜMMELMANNSBERG",
  u8"über Berliner Tor, Burgstr., Hamburger Str., Rauhes Haus, Horner Rennbahn",
  u8"fährt sofort",
  U2_COLOR
};

Departure infoDeparture = {
  "U2",
  "INFORMATION",
  u8"Busse der HOCHBAHN und\nU-Bahnen fahren heute nicht!",
  u8"fährt sofort",
  U2_COLOR
};

struct UmlautGlyph {
  uint16_t codepoint;
  const uint8_t *rows;
};

const uint8_t UMLAUT_GLYPH_WIDTH = 8;
const uint8_t UMLAUT_GLYPH_HEIGHT = 16;
const int8_t UMLAUT_GLYPH_Y_OFFSET = 2;

const uint8_t GLYPH_A_UMLAUT[16] = {0x00, 0x28, 0x00, 0x10, 0x28, 0x44, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00};
const uint8_t GLYPH_O_UMLAUT[16] = {0x00, 0x28, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00};
const uint8_t GLYPH_U_UMLAUT[16] = {0x00, 0x28, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00};
const uint8_t GLYPH_AE_UMLAUT[16] = {0x00, 0x28, 0x00, 0x00, 0x00, 0x38, 0x44, 0x04, 0x3C, 0x44, 0x44, 0x3C, 0x00, 0x00, 0x00, 0x00};
const uint8_t GLYPH_OE_UMLAUT[16] = {0x00, 0x28, 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00};
const uint8_t GLYPH_UE_UMLAUT[16] = {0x00, 0x28, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x4C, 0x34, 0x00, 0x00, 0x00, 0x00};

const UmlautGlyph UMLAUT_GLYPHS[] = {
  {0x00C4, GLYPH_A_UMLAUT},
  {0x00D6, GLYPH_O_UMLAUT},
  {0x00DC, GLYPH_U_UMLAUT},
  {0x00E4, GLYPH_AE_UMLAUT},
  {0x00F6, GLYPH_OE_UMLAUT},
  {0x00FC, GLYPH_UE_UMLAUT}
};

void drawText(const char *text, int16_t x, int16_t y, uint8_t font, uint16_t color, uint16_t background = BOARD_BG) {
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(color, background);
  tft.drawString(text, x, y, font);
}

uint16_t readUtf8Codepoint(const char *text, uint16_t &index) {
  uint8_t firstByte = text[index++];

  if (firstByte < 0x80) return firstByte;

  if ((firstByte & 0xE0) == 0xC0) {
    uint8_t secondByte = text[index++];
    return ((firstByte & 0x1F) << 6) | (secondByte & 0x3F);
  }

  return '?';
}

const uint8_t *findUmlautGlyph(uint16_t codepoint) {
  for (uint8_t i = 0; i < (sizeof(UMLAUT_GLYPHS) / sizeof(UMLAUT_GLYPHS[0])); i++) {
    if (UMLAUT_GLYPHS[i].codepoint == codepoint) return UMLAUT_GLYPHS[i].rows;
  }

  return nullptr;
}

int16_t glyphAdvance(uint8_t font, uint8_t textSize) {
  tft.setTextSize(textSize);
  return UMLAUT_GLYPH_WIDTH * textSize;
}

void drawUmlautGlyph(int16_t x, int16_t y, uint8_t font, uint8_t textSize, uint16_t color, const uint8_t *rows) {
  int16_t rowScale = textSize;
  int16_t colScale = textSize;

  for (uint8_t row = 0; row < UMLAUT_GLYPH_HEIGHT; row++) {
    for (uint8_t col = 0; col < UMLAUT_GLYPH_WIDTH; col++) {
      if (rows[row] & (0x80 >> col)) {
        tft.fillRect(x + (col * colScale), y + (row * rowScale), colScale, rowScale, color);
      }
    }
  }
}

void drawUtf8Text(const char *text, int16_t x, int16_t y, uint8_t font, uint8_t textSize, uint16_t color, uint16_t background = BOARD_BG) {
  uint16_t index = 0;
  int16_t cursorX = x;
  int16_t charAdvance = glyphAdvance(font, textSize);

  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(textSize);
  tft.setTextColor(color, background);

  while (text[index] != '\0') {
    uint16_t codepoint = readUtf8Codepoint(text, index);
    const uint8_t *umlautGlyph = findUmlautGlyph(codepoint);

    if (umlautGlyph) {
      tft.fillRect(cursorX, y, charAdvance, tft.fontHeight(font) * textSize, background);
      drawUmlautGlyph(cursorX, y + UMLAUT_GLYPH_Y_OFFSET, font, textSize, color, umlautGlyph);
      cursorX += charAdvance;
    } else if (codepoint < 0x80) {
      char ascii[2] = {(char)codepoint, '\0'};
      tft.drawString(ascii, cursorX, y, font);
      cursorX += tft.textWidth(ascii, font);
    } else {
      tft.drawString("?", cursorX, y, font);
      cursorX += tft.textWidth("?", font);
    }
  }

  tft.setTextSize(1);
}

int16_t utf8TextWidth(const char *text, uint8_t font, uint8_t textSize) {
  uint16_t index = 0;
  int16_t width = 0;
  tft.setTextSize(textSize);
  int16_t charAdvance = glyphAdvance(font, textSize);

  while (text[index] != '\0') {
    uint16_t codepoint = readUtf8Codepoint(text, index);

    if (findUmlautGlyph(codepoint)) {
      width += charAdvance;
    } else if (codepoint < 0x80) {
      char ascii[2] = {(char)codepoint, '\0'};
      width += tft.textWidth(ascii, font);
    } else {
      width += tft.textWidth("?", font);
    }
  }

  return width;
}

void drawUtf8TextRight(const char *text, int16_t rightX, int16_t y, uint8_t font, uint8_t textSize, uint16_t color, uint16_t background = BOARD_BG) {
  drawUtf8Text(text, rightX - utf8TextWidth(text, font, textSize), y, font, textSize, color, background);
}

void copyText(char *destination, const char *source, size_t destinationSize) {
  if (destinationSize == 0) return;

  strncpy(destination, source, destinationSize - 1);
  destination[destinationSize - 1] = '\0';
}

void drawWrappedUtf8Text(const char *text, int16_t x, int16_t y, int16_t maxWidth, uint8_t font, uint8_t textSize, uint16_t color, uint16_t background = BOARD_BG) {
  char line[96] = "";
  char word[28] = "";
  uint8_t wordIndex = 0;
  uint8_t lineHeight = (tft.fontHeight(font) * textSize) + 4;

  for (uint16_t i = 0; ; i++) {
    char current = text[i];
    if (current == '\n') {
      if (wordIndex > 0) {
        word[wordIndex] = '\0';
        char candidate[96];
        snprintf(candidate, sizeof(candidate), "%s%s%s", line, line[0] ? " " : "", word);

        if (line[0] && utf8TextWidth(candidate, font, textSize) > maxWidth) {
          drawUtf8Text(line, x, y, font, textSize, color, background);
          y += lineHeight;
          copyText(line, word, sizeof(line));
        } else {
          copyText(line, candidate, sizeof(line));
        }
      }

      if (line[0]) drawUtf8Text(line, x, y, font, textSize, color, background);
      y += lineHeight;
      line[0] = '\0';
      wordIndex = 0;
      continue;
    }

    bool boundary = (current == ' ' || current == '\0');

    if (!boundary && wordIndex < sizeof(word) - 1) word[wordIndex++] = current;

    if (boundary) {
      word[wordIndex] = '\0';

      if (wordIndex > 0) {
        char candidate[96];
        snprintf(candidate, sizeof(candidate), "%s%s%s", line, line[0] ? " " : "", word);

        if (line[0] && utf8TextWidth(candidate, font, textSize) > maxWidth) {
          drawUtf8Text(line, x, y, font, textSize, color, background);
          y += lineHeight;
          copyText(line, word, sizeof(line));
        } else {
          copyText(line, candidate, sizeof(line));
        }
      }

      wordIndex = 0;
    }

    if (current == '\0') break;
  }

  if (line[0]) drawUtf8Text(line, x, y, font, textSize, color, background);
}

void drawHeader(const Departure &screenDeparture = departure) {
  int16_t screenWidth = tft.width();
  int16_t greenLineY = 58;
  uint8_t headerFont = 2;
  uint8_t lineTextSize = 3;
  int16_t badgeX = 0;
  int16_t badgeY = 0;
  int16_t badgeW = 86;
  int16_t badgeH = greenLineY;
  int16_t badgeRight = badgeX + badgeW;
  int16_t destinationLeft = badgeRight + 14;
  int16_t destinationRight = screenWidth - 10;
  bool isMainDestination = strcmp(screenDeparture.destination, departure.destination) == 0;
  uint8_t destinationTextSize = isMainDestination ? 2 : lineTextSize;

  while (destinationTextSize > 1 && utf8TextWidth(screenDeparture.destination, headerFont, destinationTextSize) > (destinationRight - destinationLeft)) {
    destinationTextSize--;
  }

  int16_t lineTextHeight = tft.fontHeight(headerFont) * lineTextSize;
  int16_t lineTextY = badgeY + ((badgeH - lineTextHeight) / 2);
  int16_t destinationWidth = utf8TextWidth(screenDeparture.destination, headerFont, destinationTextSize);
  int16_t destinationX = destinationLeft + (((destinationRight - destinationLeft) - destinationWidth) / 2);
  int16_t destinationY = isMainDestination
    ? lineTextY + (lineTextHeight - (tft.fontHeight(headerFont) * destinationTextSize))
    : ((destinationTextSize == lineTextSize)
      ? lineTextY
      : badgeY + ((badgeH - (tft.fontHeight(headerFont) * destinationTextSize)) / 2));

  tft.fillRect(0, 0, screenWidth, greenLineY, BOARD_BG);
  tft.fillRect(badgeX, badgeY, badgeW, badgeH, screenDeparture.lineColor);

  tft.setTextSize(lineTextSize);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(BLACK, screenDeparture.lineColor);
  tft.drawString(screenDeparture.line, badgeX + (badgeW / 2), badgeY + (badgeH / 2), headerFont);

  tft.setTextSize(destinationTextSize);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(screenDeparture.lineColor, BOARD_BG);
  drawUtf8Text(screenDeparture.destination, destinationX, destinationY, headerFont, destinationTextSize, screenDeparture.lineColor, BOARD_BG);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);

  drawGreenSeparator(greenLineY);
}

void drawLineBadge(const Departure &departure, int16_t x, int16_t y) {
  tft.fillRect(x, y, 58, 36, departure.lineColor);
  tft.drawRect(x, y, 58, 36, 0xFFFF);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(0xFFFF, departure.lineColor);
  tft.drawString(departure.line, x + 29, y + 18, 4);
  tft.setTextDatum(TL_DATUM);
}

void drawGreenSeparator(int16_t y) {
  int16_t screenWidth = tft.width();
  tft.drawFastHLine(0, y, screenWidth, GREEN_LINE);
  tft.drawFastHLine(0, y + 1, screenWidth, GREEN_LINE);
}

void drawSingleDeparture(const Departure &departure) {
  int16_t screenWidth = tft.width();
  int16_t screenHeight = tft.height();
  int16_t contentTop = 70;

  drawWrappedUtf8Text(departure.via, 12, contentTop, screenWidth - 24, 2, 2, AMBER_TEXT, BOARD_BG);

  drawGreenSeparator(screenHeight - 60);

  drawUtf8TextRight(departure.timeText, screenWidth - 14, screenHeight - 42, 2, 2, GREEN_LINE, BOARD_BG);
  tft.setTextDatum(TL_DATUM);
}

void drawBoard() {
  tft.fillScreen(BOARD_BG);
  drawHeader();
  drawSingleDeparture(departure);
}

void drawInfoScreen() {
  int16_t screenWidth = tft.width();
  int16_t screenHeight = tft.height();

  tft.fillScreen(BOARD_BG);
  drawHeader(infoDeparture);
  drawWrappedUtf8Text(infoDeparture.via, 18, 74, screenWidth - 36, 2, 2, AMBER_TEXT, BOARD_BG);
  drawGreenSeparator(screenHeight - 60);
  drawUtf8TextRight(infoDeparture.timeText, screenWidth - 14, screenHeight - 42, 2, 2, GREEN_LINE, BOARD_BG);
  tft.setTextDatum(TL_DATUM);
}

void redrawCurrentScreen() {
  if (currentScreen == 1) {
    drawInfoScreen();
  } else {
    drawBoard();
  }
}

void setup() {
  tft.begin();
  tft.invertDisplay(true);
  tft.fillScreen(0x0000);
  tft.setRotation(1);

  redrawCurrentScreen();
}

void loop(void) {
}
