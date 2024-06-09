#include <Adafruit_GFX.h>
#include <cstdint>

#include "colors.h"

struct GaugeTheme {
  uint16_t okColor = OLED_Color_Warm;
  uint16_t lowColor = OLED_Color_Blue;
  uint16_t highColor = OLED_Color_Red;
};

struct GaugeConfig {
  const char *name = "undefined";
  const char *units = "undefined";
  float min = 0;
  float max = 100;
  float lowValue = 10;
  float highValue = 90;
};

struct GaugeData {
  float currentValue;
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

void drawGauge(GFXcanvas16 &canvas, const struct GaugeConfig &config,
               struct GaugeVerticalLayout &layout,
               const struct GaugeTheme &theme, const struct GaugeData &data) {
  const int16_t tallNotchH = 8;
  const int16_t smallNotchH = 6;
  const int16_t textH = 6;
  const int16_t textBottomMargin = 2;
  const int16_t lineH = 2;
  const int16_t indicatorW = tallNotchH;
  const int16_t indicatorLeftMargin = 3;

  const int16_t maxH = tallNotchH + textH + textBottomMargin;

  canvas.setTextColor(theme.okColor);
  canvas.setTextSize(1);
  canvas.setCursor(layout.xPos, layout.yPos);
  canvas.print(config.name);
  canvas.print(' ');
  canvas.print(data.currentValue);
  canvas.print(' ');
  canvas.print(config.units);

  int16_t w =
      ((layout.maxWidth - (indicatorLeftMargin + indicatorW)) / 10) * 10;
  layout.yPos += maxH;

  float factor = constrain(
      (data.currentValue - config.min) / (config.max - config.min), 0.f, 1.f);
  int16_t currentW = factor * w;

  canvas.fillRect(layout.xPos, layout.yPos, w, -lineH, theme.okColor);
  canvas.fillRect(layout.xPos, layout.yPos, currentW, -tallNotchH,
                  theme.okColor);
  {
    int16_t indicatorColor = theme.okColor;
    if (data.currentValue < config.lowValue) {
      indicatorColor = theme.lowColor;
    } else if (data.currentValue > config.highValue) {
      indicatorColor = theme.highColor;
    }
    canvas.fillRect(layout.xPos + w + indicatorLeftMargin, layout.yPos,
                    indicatorW, -tallNotchH, indicatorColor);
  }

  auto drawNotch = [&](int16_t x, bool tall) {
    canvas.drawFastVLine(x, layout.yPos, tall ? -tallNotchH : -smallNotchH,
                         theme.okColor);
  };

  for (int16_t x = layout.xPos; x <= layout.xPos + w; x += w / 10) {
    drawNotch(x, (x - layout.xPos) % 5 == 0);
  }

  layout.yPos += layout.margin;
}