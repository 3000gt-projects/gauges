/*
 * This is an example sketch that shows how to toggle the SSD1331 OLED display
 * on and off at runtime to avoid screen burn-in.
 *
 * The sketch also demonstrates how to erase a previous value by re-drawing the
 * older value in the screen background color prior to writing a new value in
 * the same location. This avoids the need to call fillScreen() to erase the
 * entire screen followed by a complete redraw of screen contents.
 *
 * Written by Phill Kelley. BSD license.
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Arduino.h>
#include <SPI.h>

#include "gauge.h"

#define SerialDebugging true

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// The SSD1351 is connected like this (plus VCC plus GND)
const uint8_t OLED_pin_scl_sck = 18;
const uint8_t OLED_pin_sda_mosi = 19;
const uint8_t OLED_pin_cs_ss = 17;
const uint8_t OLED_pin_cs2_ss = 16;
const uint8_t OLED_pin_res_rst = 21;
const uint8_t OLED_pin_dc_rs = 20;
const uint8_t OLED_pin_dc2_rs = 22;
const uint8_t OLED_pin_res2_rst = 27;

// connect a push button to ...
const uint8_t Button_pin = 2;

// The colors we actually want to use
uint16_t OLED_Text_Color = OLED_Color_Red;
uint16_t OLED_Backround_Color = OLED_Color_Black;

GFXcanvas16 canvas1(SCREEN_WIDTH, SCREEN_HEIGHT);

// declare the display
Adafruit_SSD1351 oled =
    Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_pin_cs_ss,
                     OLED_pin_dc_rs, OLED_pin_res_rst);

Adafruit_SSD1351 oled2 = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI,
                                          OLED_pin_cs2_ss, OLED_pin_dc2_rs);

void setup() {
#if (SerialDebugging)
  Serial.begin(115200);
#endif

  // settling time
  delay(250);

  canvas1.setFont();
  canvas1.fillScreen(OLED_Backround_Color);
  canvas1.setTextColor(OLED_Text_Color);

  // initialise the SSD1331
  oled.begin();
  oled2.begin();

  oled.enableDisplay(true);
  oled2.enableDisplay(true);
}

GaugeTheme theme;

namespace gauges {

GaugeConfig coolant{
    .name = "COOLANT",
    .units = "C",
    .min = 0,
    .max = 130,
};

GaugeConfig oilTemp{
    .name = "OIL",
    .units = "C",
    .min = 0,
    .max = 150,
};

GaugeConfig iat{
    .name = "IAT",
    .units = "C",
    .min = 0,
    .max = 100,
};

GaugeConfig egt{
    .name = "EGT",
    .units = "C",
    .min = 0,
    .max = 1300,
};

GaugeConfig oilPress{
    .name = "OIL",
    .units = "bar",
    .min = 0,
    .max = 10,
};

GaugeConfig boost{
    .name = "BOOST",
    .units = "bar",
    .min = -1,
    .max = 2,
};

GaugeConfig fuelPress{
    .name = "FUEL",
    .units = "bar",
    .min = 0,
    .max = 10,
};

} // namespace gauges

void loop() {
  auto factor = float(analogRead(26)) / 1024;

  GaugeVerticalLayout layout{
      .xPos = 0,
      .yPos = 0,
      .margin = 8,
      .maxWidth = SCREEN_WIDTH,
  };

  canvas1.fillScreen(OLED_Backround_Color);
  {
    GaugeData data;
    data.currentValue = 90 * factor;
    drawGauge(canvas1, gauges::coolant, layout, theme, data);
  }

  {
    GaugeData data;
    data.currentValue = 90 * factor;
    drawGauge(canvas1, gauges::oilTemp, layout, theme, data);
  }

  {
    GaugeData data;
    data.currentValue = 55 * factor;
    drawGauge(canvas1, gauges::iat, layout, theme, data);
  }

  {
    GaugeData data;
    data.currentValue = 900 * factor;
    drawGauge(canvas1, gauges::egt, layout, theme, data);
  }

  oled.drawRGBBitmap(0, 0, canvas1.getBuffer(), canvas1.width(),
                     canvas1.height());

  canvas1.fillScreen(OLED_Backround_Color);
  resetLayout(layout, 0);

  {
    GaugeData data;
    data.currentValue = 9 * factor;
    drawGauge(canvas1, gauges::oilPress, layout, theme, data);
  }

  {
    GaugeData data;
    data.currentValue = 2.1 * factor - 1;
    drawGauge(canvas1, gauges::boost, layout, theme, data);
  }

  {
    GaugeData data;
    data.currentValue = 9 * factor;
    drawGauge(canvas1, gauges::fuelPress, layout, theme, data);
  }

  oled2.drawRGBBitmap(0, 0, canvas1.getBuffer(), canvas1.width(),
                      canvas1.height());

  // no need to be in too much of a hurry
  delay(16);
}