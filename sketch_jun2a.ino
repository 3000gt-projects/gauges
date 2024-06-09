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

// SSD1331 color definitions
const uint16_t OLED_Color_Black = 0x0000;
const uint16_t OLED_Color_Blue = 0x001F;
const uint16_t OLED_Color_Red = 0xF800;
const uint16_t OLED_Color_Green = 0x07E0;
const uint16_t OLED_Color_Cyan = 0x07FF;
const uint16_t OLED_Color_Magenta = 0xF81F;
const uint16_t OLED_Color_Yellow = 0xFFE0;
const uint16_t OLED_Color_Warm = 0xFC00;
const uint16_t OLED_Color_White = 0xFFFF;

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

void loop() {
  auto factor = float(analogRead(26)) / 1024;

  canvas1.fillScreen(OLED_Backround_Color);
  {
    Gauge gauge;
    gauge.name = "COOLANT";
    gauge.units = "C";
    gauge.min = 0;
    gauge.max = 130;
    gauge.current = 90 * factor;
    drawGauge(canvas1, gauge, 0, 0, 128, OLED_Color_Warm);
  }

  {
    Gauge gauge;
    gauge.name = "OIL";
    gauge.units = "C";
    gauge.min = 0;
    gauge.max = 150;
    gauge.current = 90 * factor;
    drawGauge(canvas1, gauge, 0, 25, 128, OLED_Color_Warm);
  }

  {
    Gauge gauge;
    gauge.name = "IAT";
    gauge.units = "C";
    gauge.min = 0;
    gauge.max = 100;
    gauge.current = 55 * factor;
    drawGauge(canvas1, gauge, 0, 50, 128, OLED_Color_Warm);
  }

  {
    Gauge gauge;
    gauge.name = "EGT";
    gauge.units = "C";
    gauge.min = 0;
    gauge.max = 1300;
    gauge.current = 900 * factor;
    drawGauge(canvas1, gauge, 0, 75, 128, OLED_Color_Warm);
  }

  oled.drawRGBBitmap(0, 0, canvas1.getBuffer(), canvas1.width(),
                     canvas1.height());

  canvas1.fillScreen(OLED_Backround_Color);
  {
    Gauge gauge;
    gauge.name = "OIL";
    gauge.units = "bar";
    gauge.min = 0;
    gauge.max = 10;
    gauge.current = 9 * factor;
    drawGauge(canvas1, gauge, 0, 0, 128, OLED_Color_Warm);
  }

  {
    Gauge gauge;
    gauge.name = "BOOST";
    gauge.units = "bar";
    gauge.min = -1;
    gauge.max = 2;
    gauge.current = 2.1 * factor - 1;
    drawGauge(canvas1, gauge, 0, 25, 128, OLED_Color_Warm);
  }

  {
    Gauge gauge;
    gauge.name = "FUEL";
    gauge.units = "bar";
    gauge.min = 0;
    gauge.max = 10;
    gauge.current = 9 * factor;
    drawGauge(canvas1, gauge, 0, 50, 128, OLED_Color_Warm);
  }

  oled2.drawRGBBitmap(0, 0, canvas1.getBuffer(), canvas1.width(),
                      canvas1.height());

  // no need to be in too much of a hurry
  delay(16);
}