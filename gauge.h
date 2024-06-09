#include <Adafruit_GFX.h>

struct Gauge {
  const char *name = "undefined";
  const char *units = "undefined";
  float min = 0;
  float max = 100;
  float current = 0;
};

void drawGauge(GFXcanvas16 &canvas, const struct Gauge &gauge, int16_t x0,
               int16_t y0, int16_t maxW, uint16_t color) {
  const int16_t tallNotchH = 6;
  const int16_t smallNotchH = 4;
  const int16_t textH = 10;

  const int16_t maxH = tallNotchH + textH;

  canvas.setTextColor(color);
  canvas.setTextSize(1);
  canvas.setCursor(x0, y0);
  canvas.print(gauge.name);
  canvas.print(' ');
  canvas.print(gauge.current);
  canvas.print(' ');
  canvas.print(gauge.units);

  int16_t w = (maxW / 10) * 10;
  y0 += maxH;

  float factor = constrain(
      (gauge.current - gauge.min) / (gauge.max - gauge.min), 0.f, 1.f);
  int16_t currentW = factor * w;

  canvas.fillRect(x0, y0, w + 1, 2, color);
  canvas.fillRect(x0, y0, currentW, -tallNotchH, color);

  auto drawNotch = [&](int16_t x, bool tall) {
    canvas.drawFastVLine(x, y0, tall ? -tallNotchH : -smallNotchH, color);
  };

  for (int16_t x = x0; x <= x0 + w; x += w / 10) {
    drawNotch(x, (x - x0) % 5 == 0);
  }
}