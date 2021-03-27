#include "profiles.h"
#include "light_utils.h"
#include "miniFastLED.h"
#include "string.h"

// An array of basic colors used accross different lighting profiles
// static const uint32_t colorPalette[] = {0xFF0000, 0xF0F00, 0x00F00, 0x00F0F,
// 0x0000F, 0xF000F, 0x50F0F};
static const uint32_t colorPalette[] = {0xcc0000, 0xcccc00, 0x5fcc00, 0x00c7cc,
                                        0x006ecc, 0x0033ff, 0x6900cc, 0xcc00bf};

#define LEN(a) (sizeof(a) / sizeof(*a))

void red(led_t *currentKeyLedColors) {
  setAllKeysColor(currentKeyLedColors, 0xFF0000);
}

void green(led_t *currentKeyLedColors) {
  setAllKeysColor(currentKeyLedColors, 0x00FF00);
}

void blue(led_t *currentKeyLedColors) {
  setAllKeysColor(currentKeyLedColors, 0x0000FF);
}

/* Color bleed test pattern */
void colorBleed(led_t *currentKeyLedColors) {
  for (uint16_t c = 0; c < NUM_COLUMN; ++c) {
    for (uint16_t r = 0; r < NUM_ROW; ++r) {
      const uint16_t i = c + NUM_COLUMN * r;
      if (c == 0) {
        if (r <= 3)
          currentKeyLedColors[i].rgb = 0x0000FF;
        else
          currentKeyLedColors[i].rgb = 0xFF0000;
        continue;
      }
      if (r == 0) {
        if (c <= 10)
          currentKeyLedColors[i].rgb = 0x0000FF;
        else
          currentKeyLedColors[i].rgb = 0xFF0000;
        continue;
      }

      switch (i % 3) {
      case 0:
        currentKeyLedColors[i].rgb = 0x0000FF;
        break;
      case 1:
        currentKeyLedColors[i].rgb = 0x00FF00;
        break;
      case 2:
        currentKeyLedColors[i].rgb = 0xFF0000;
        break;
      }
    }
  }
}

void white(led_t *currentKeyLedColors) {
  /* To get "white" you need to compensate for red/blue switches on board */
  setAllKeysColor(currentKeyLedColors, 0x80ff99);
}

void miamiNights(led_t *currentKeyLedColors) {
  setAllKeysColor(currentKeyLedColors, 0x00979c);
  setModKeysColor(currentKeyLedColors, 0x9c008f);
}

void rainbowHorizontal(led_t *currentKeyLedColors) {
  for (uint16_t i = 0; i < NUM_ROW; ++i) {
    for (uint16_t j = 0; j < NUM_COLUMN; ++j) {
      setKeyColor(&currentKeyLedColors[i * NUM_COLUMN + j], colorPalette[i]);
    }
  }
}

void rainbowVertical(led_t *currentKeyLedColors) {
  for (uint16_t i = 0; i < NUM_COLUMN; ++i) {
    for (uint16_t j = 0; j < NUM_ROW; ++j) {
      setKeyColor(&currentKeyLedColors[j * NUM_COLUMN + i],
                  colorPalette[i % LEN(colorPalette)]);
    }
  }
}

static uint8_t colAnimOffset = 0;
void animatedRainbowVertical(led_t *currentKeyLedColors) {
  for (uint16_t i = 0; i < NUM_COLUMN; ++i) {
    for (uint16_t j = 0; j < NUM_ROW; ++j) {
      setKeyColor(&currentKeyLedColors[j * NUM_COLUMN + i],
                  colorPalette[(i + colAnimOffset) % LEN(colorPalette)]);
    }
  }
  colAnimOffset = (colAnimOffset + 1) % LEN(colorPalette);
}

static uint8_t flowValue[NUM_COLUMN] = {0,  11, 22, 33,  44,  55,  66,
                                        77, 88, 99, 110, 121, 132, 143};
void animatedRainbowFlow(led_t *currentKeyLedColors) {
  for (int i = 0; i < NUM_COLUMN; i++) {
    setColumnColorHSV(currentKeyLedColors, i, flowValue[i], 255, 255);
    if (flowValue[i] >= 179 && flowValue[i] < 240) {
      flowValue[i] = 240;
    }
    flowValue[i] += 3;
  }
}

static uint8_t waterfallValue[NUM_COLUMN] = {0,  10, 20, 30,  40,  50,  60,
                                             70, 80, 90, 100, 110, 120, 130};
void animatedRainbowWaterfall(led_t *currentKeyLedColors) {
  for (int i = 0; i < NUM_ROW; i++) {
    setRowColorHSV(currentKeyLedColors, i, waterfallValue[i], 255, 125);
    if (waterfallValue[i] >= 179 && waterfallValue[i] < 240) {
      waterfallValue[i] = 240;
    }
    waterfallValue[i] += 3;
  }
}

static uint8_t breathingValue = 180;
static int breathingDirection = -1;
void animatedBreathing(led_t *currentKeyLedColors) {
  setAllKeysColorHSV(currentKeyLedColors, 85, 255, breathingValue);
  if (breathingValue >= 180) {
    breathingDirection = -2;
  } else if (breathingValue <= 2) {
    breathingDirection = 2;
  }
  breathingValue += breathingDirection;
}

static uint8_t spectrumValue = 2;
static int spectrumDirection = 1;
void animatedSpectrum(led_t *currentKeyLedColors) {
  setAllKeysColorHSV(currentKeyLedColors, spectrumValue, 255, 125);
  if (spectrumValue >= 177) {
    spectrumDirection = -3;
  } else if (spectrumValue <= 2) {
    spectrumDirection = 3;
  }
  spectrumValue += spectrumDirection;
}

static uint8_t waveValue[NUM_COLUMN] = {0,  0,  0,  10,  15,  20,  25,
                                        40, 55, 75, 100, 115, 135, 140};
static int waveDirection[NUM_COLUMN] = {3, 3, 3, 3, 3, 3, 3,
                                        3, 3, 3, 3, 3, 3, 3};
void animatedWave(led_t *currentKeyLedColors) {
  for (int i = 0; i < NUM_COLUMN; i++) {
    if (waveValue[i] >= 140) {
      waveDirection[i] = -3;
    } else if (waveValue[i] <= 10) {
      waveDirection[i] = 3;
    }
    setColumnColorHSV(currentKeyLedColors, i, 190, 255, waveValue[i]);
    waveValue[i] += waveDirection[i];
  }
}

uint8_t animatedPressedBuf[NUM_ROW * NUM_COLUMN] = {0};

void reactiveFade(led_t *ledColors) {
  for (int i = 0; i < NUM_ROW * NUM_COLUMN; i++) {
    if (animatedPressedBuf[i] > 5) {
      animatedPressedBuf[i] -= 5;
      hsv2rgb(100 - animatedPressedBuf[i], 255, 225, &ledColors[i]);
    } else if (animatedPressedBuf[i] > 0) {
      ledColors[i].p.blue = 0;
      ledColors[i].p.red = 0;
      ledColors[i].p.green = 0;
      animatedPressedBuf[i] = 0;
    }
  }
}

void reactiveFadeKeypress(led_t *ledColors, uint8_t row, uint8_t col) {
  int i = row * NUM_COLUMN + col;
  animatedPressedBuf[i] = 100;
  ledColors[i].p.green = 0;
  ledColors[i].p.red = 0xFF;
  ledColors[i].p.blue = 0;
}

void reactiveFadeInit(led_t *ledColors) {
  // create a quick "falling" animation to make it easier to see
  // that this profile is activated
  for (int i = 0; i < NUM_ROW; i++) {
    for (int j = 0; j < NUM_COLUMN; j++) {
      animatedPressedBuf[i * NUM_COLUMN + j] = i * 15 + 25;
    }
  }
  memset(ledColors, 0, NUM_ROW * NUM_COLUMN * 3);
}

uint8_t pulseBuf[NUM_ROW];

void reactivePulse(led_t *ledColors) {
  uint8_t pulseSpeed = 16;

  for (int i = 0; i < NUM_ROW; i++) {
    if (pulseBuf[i] > 80) {
      pulseBuf[i] -= pulseSpeed;
    } else if (pulseBuf[i] > pulseSpeed) {
      for (int j = 0; j < NUM_COLUMN; j++) {
        ledColors[i * NUM_COLUMN + j].p.blue = (175 + pulseBuf[i]);
        ledColors[i * NUM_COLUMN + j].p.red = 0;
        ledColors[i * NUM_COLUMN + j].p.green = 0;
      }
      pulseBuf[i] -= pulseSpeed;
    } else if (pulseBuf[i] > 0) {
      pulseBuf[i] = 0;
      for (int j = 0; j < NUM_COLUMN; j++) {
        ledColors[i * NUM_COLUMN + j].p.blue = 0;
        ledColors[i * NUM_COLUMN + j].p.red = 0;
        ledColors[i * NUM_COLUMN + j].p.green = 0;
      }
    }
  }
}

void reactivePulseKeypress(led_t *ledColors, uint8_t row, uint8_t col) {
  (void)ledColors;
  (void)col;

  pulseBuf[row] = 80;
}

void reactivePulseInit(led_t *ledColors) {
  for (int i = 0; i < NUM_ROW; i++) {
    pulseBuf[i] = 80 + i * 5;
  }
  memset(ledColors, 0, NUM_ROW * NUM_COLUMN * 3);

/* Terminal cursor */
void lazyMark(led_t *ledColors, int8_t row, int8_t col, const led_t color) {
  if (row >= NUM_ROW || col >= NUM_COLUMN)
    return;
  if (row < 0 || col < 0)
    return;
  ledColors[ROWCOL2IDX(row, col)] = color;
}

int8_t rowBlink = -1;
int8_t termPos;
uint16_t termAnim;
void reactiveTerm(led_t *ledColors) {
  led_t color;
  color.rgb = 0;
  memset(ledColors, 0, NUM_COLUMN * NUM_ROW * sizeof(*ledColors));

  if (termPos < 0) {
    color.p.red = 255;
    lazyMark(ledColors, 0, -termPos, color);
    lazyMark(ledColors, 0, -termPos + 1, color);
    termPos += 2;
    return;
  }

  if (rowBlink != -1) {
      color.p.red = 0;
    color.p.green = 255;
    for (int col = 0; col < NUM_COLUMN; col++) {
      lazyMark(ledColors, rowBlink, col, color);
    }

    rowBlink = -1;
  }

  /* 70*14 times per second */
  termAnim++;
  if (termAnim > 140)
    termAnim = 0;
  int16_t brightness = 0;

  if (termAnim < 70) {
    brightness = termAnim * 51; /* full in 5 frames */
    if (brightness > 255)
      brightness = 255;
  } else {
    /* Starts with 70 */
    brightness = 255 - (termAnim - 70) * 51;
    if (brightness < 0)
      brightness = 0;
  }
  color.p.green = 0;
  color.p.red = brightness;
  lazyMark(ledColors, 0, termPos, color);
}

void reactiveTermKeypress(led_t *ledColors, uint8_t row, uint8_t col) {
  (void)row;
  (void)col;
  if (termPos >= 0) {
    termPos = (termPos + 1);
    if (termPos == 13) {
      termPos = -14;
    }
  }
  termAnim = 0;
  rowBlink = row;
  memset(ledColors, 0, NUM_COLUMN * NUM_ROW * sizeof(*ledColors));
}

void reactiveTermInit(led_t *ledColors) {
  termPos = 0;
  termAnim = 0;
  memset(ledColors, 0, NUM_COLUMN * NUM_ROW * sizeof(*ledColors));
}

/* Heartbeat */

uint8_t euclidDistance[5][14] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
    {2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
    {3, 4, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
    {4, 5, 5, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 14}
};
uint8_t hbWave[] = { 255, 242, 216, 181, 137, 86, 30, 0 };

uint8_t hbHighCycle = 0;
uint8_t hbLowCycle = 0;
uint8_t hbEpicenter = 0;
uint8_t hbMaxDist = 0;
uint8_t hbAnimSpeed = 0;

void animatedHeartbeat(led_t *ledColors) {
  /* High cycle */
  hbHighCycle++;
  switch (hbHighCycle) {
  case 70:
      hbEpicenter = 3;
      hbLowCycle = 0;
      hbMaxDist = 4;
      hbAnimSpeed = 1;
      break;
  case 95:
      hbEpicenter = 5;
      hbLowCycle = 0;
      hbMaxDist = 20;
      hbAnimSpeed = 2;
      break;
  case 200:
      hbHighCycle = 0;
      break;
  }
  /* Clear it */
  memset(ledColors, 0, NUM_COLUMN * NUM_ROW * sizeof(*ledColors));

  led_t background;
  hsv2rgb(HUE_RED, 255, 10, &background);

  for (int8_t row = 0; row < NUM_ROW; row++) {
    for (int8_t col = 0; col < NUM_COLUMN; col++) {
        int8_t cDist = col - hbEpicenter;
        int8_t rDist = row - 2; /* Static epicenter for row */
        if (cDist < 0)
            cDist = -cDist;
        if (rDist < 0)
          rDist = -rDist;
        int8_t distance = euclidDistance[rDist][cDist];
        const int8_t wavePos = hbLowCycle - distance;
        led_t color;
        if (distance < hbMaxDist && wavePos >= 0 && wavePos <= 7) {
            hsv2rgb(HUE_RED, 255, hbWave[wavePos], &color);
            ledColors[ROWCOL2IDX(row, col)] = color;
        } else {
            ledColors[ROWCOL2IDX(row, col)] = background;
        }
    }
  }
  if (hbLowCycle < 32) {
      hbLowCycle += hbAnimSpeed;
  }
}
