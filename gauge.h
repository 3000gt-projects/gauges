#include <Adafruit_GFX.h>
#include <Arduino.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "CustomFonts/DSEG7_Modern_Bold_12.h"
#include "colors.h"
#include "debug.h"
#include "dto.h"

static const GaugeData offlineGaugeData{
    .currentValue = GaugeData::offlineValue,
};

static const GaugeData badGaugeData{
    .currentValue = GaugeData::badDataValue,
};

struct GaugeVerticalLayout {
  int16_t xPos = 0;
  int16_t yPos = 0;
  int16_t margin = 0;
  int16_t maxWidth = 0;
};

void resetLayout(struct GaugeVerticalLayout &layout, int16_t yPos) {
  layout.yPos = yPos;
}

template <size_t N>
int16_t valueToString(float value, const char *format, char (&out)[N]) {
  return snprintf(out, N, format, value);
}

template <size_t N>
int16_t stringWidth(const char (&str)[N], int16_t glyphWidth,
                    int16_t dotWidth) {
  int16_t result = 0;

  for (char c : str) {
    if (c == '\0')
      return result;
    if (c == '.') {
      result += dotWidth;
      continue;
    }

    result += glyphWidth;
  }

  return result;
}

void drawGauge(GFXcanvas16 &canvas, const struct GaugeConfig &config,
               struct GaugeVerticalLayout &layout,
               const struct GaugeTheme &theme, const struct GaugeData &data) {
  debug("Draw gauge");

  enum Status {
    Alert,
    High,
    Low,
    Ok,
  };

  static uint32_t blinkIntervalMs = 500;

  static const int16_t tallNotchH = 8;
  static const int16_t smallNotchH = 6;
  static const int16_t textH = 12;
  static const int16_t textW = 6;
  static const int16_t valueTextW = 11;
  static const int16_t valueTextDotW = 1;
  static const int16_t textBottomMargin = 2;
  static const int16_t lineH = 2;
  static const int16_t indicatorW = 0;
  static const int16_t indicatorLeftMargin = 3;
  static const int16_t maxH = tallNotchH + textH + textBottomMargin;

  auto advanceLayout = [&layout] { layout.yPos += maxH + layout.margin; };

  Status status;
  int16_t color;

  if (data.currentValue == GaugeData::offlineValue) {
    status = Alert;
    color = theme.alertColor;
  } else if (data.currentValue == GaugeData::badDataValue) {
    status = Alert;
    color = theme.alertColor;
  } else {
    if (data.currentValue < config.lowValue) {
      status = Low;
      color = theme.lowColor;
    } else if (data.currentValue > config.highValue) {
      status = High;
      color = theme.highColor;
    } else {
      status = Ok;
      color = theme.okColor;
    }
  }

  debug("Color: " + color);

  bool show =
      status != Alert && status != High || (millis() / blinkIntervalMs) % 2;

  if (!show) {
    advanceLayout();
    return;
  }

  canvas.setFont();
  canvas.setTextColor(color);
  canvas.setTextSize(1);
  canvas.setCursor(layout.xPos, layout.yPos);
  canvas.print(config.name);
  canvas.print(' ');

  if (data.currentValue == GaugeData::offlineValue) {
    canvas.print("OFFLINE");
  } else if (data.currentValue == GaugeData::badDataValue) {
    canvas.print("BAD DATA");
  } else {
    char valueString[10];
    if (valueToString(data.currentValue, config.format, valueString) < 1) {
      canvas.print("ERROR");
    } else {
      canvas.setFont(&DSEG7_Modern_Bold_12);
      // right alignment
      int16_t textXPos =
          canvas.width() -
          (stringWidth(valueString, valueTextW, valueTextDotW) +
           (strlen(config.units) + 1 /*+ 1 for space before units*/) * textW);
      // custom fonts seem to have coordinate origin at the bottom of the glyphs
      canvas.setCursor(textXPos, layout.yPos + textH);
      canvas.print(valueString);
      canvas.setFont();
      canvas.setCursor(canvas.getCursorX(), layout.yPos);

      canvas.print(' ');
      canvas.print(config.units);
    }
  }

  int16_t w =
      ((layout.maxWidth - (indicatorLeftMargin + indicatorW)) / 10) * 10;
  int16_t xPos = layout.xPos;
  int16_t yPos = layout.yPos + maxH;

  float factor = constrain(
      (data.currentValue - config.min) / (config.max - config.min), 0.f, 1.f);
  int16_t currentW = factor * w;

  canvas.fillRect(xPos, yPos, w, -lineH, color);
  canvas.fillRect(xPos, yPos, currentW, -tallNotchH, color);
  if (indicatorW > 0) {

    canvas.fillRect(xPos + w + indicatorLeftMargin, yPos, indicatorW,
                    -tallNotchH, color);
  }

  auto drawNotch = [&](int16_t x, bool tall) {
    canvas.drawFastVLine(x, yPos, tall ? -tallNotchH : -smallNotchH, color);
  };

  for (int16_t x = xPos; x <= xPos + w; x += w / 10) {
    drawNotch(x, (x - xPos) % 5 == 0);
  }

  advanceLayout();
}