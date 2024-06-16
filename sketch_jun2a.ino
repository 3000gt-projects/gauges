#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Arduino.h>
#include <Array.h>
#include <SPI.h>
#include <cstdint>

#include "gauge.h"

#define SerialDebugging true
#define Mock true

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

GFXcanvas16 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);

// declare the display
Adafruit_SSD1351 oled1 =
    Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_pin_cs_ss,
                     OLED_pin_dc_rs, OLED_pin_res_rst);

Adafruit_SSD1351 oled2 = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI,
                                          OLED_pin_cs2_ss, OLED_pin_dc2_rs);

struct Configuration {
  using Display = Array<GaugeConfig, 10>;

  GaugeTheme theme;
  Display display1Gauges;
  Display display2Gauges;
  Display display3Gauges;
} configuration;

struct Data {
  using Display = Array<GaugeData, 10>;

  Display display1Gauges;
  Display display2Gauges;
  Display display3Gauges;
};

void drawDisplay(Adafruit_SSD1351 &display,
                 const Configuration::Display &displayConfiguration,
                 const Data::Display &displayData) {
  canvas.fillScreen(OLED_Backround_Color);

  GaugeVerticalLayout layout{
      .xPos = 0,
      .yPos = 0,
      .margin = 8,
      .maxWidth = SCREEN_WIDTH,
  };

  if (displayConfiguration.size() == displayData.size()) {
    for (size_t gaugeIndex = 0; gaugeIndex < displayConfiguration.size();
         ++gaugeIndex) {
      drawGauge(canvas, displayConfiguration[gaugeIndex], layout,
                configuration.theme, displayData[gaugeIndex]);
    }
  } else {
    for (auto &gaugeConfiguration : displayConfiguration) {
      drawGauge(canvas, gaugeConfiguration, layout, configuration.theme,
                badGaugeData);
    }
  }

  display.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(),
                        canvas.height());
}

void setup() {
#if (SerialDebugging)
  Serial.begin(115200);
#endif

#if (Mock)
  configuration.display1Gauges.push_back(GaugeConfig{
      .name = "COOLANT",
      .units = "C",
      .min = 0,
      .max = 130,
      .lowValue = 60,
      .highValue = 100,
  });

  configuration.display1Gauges.push_back(GaugeConfig{
      .name = "OIL",
      .units = "C",
      .min = 0,
      .max = 150,
      .lowValue = 60,
      .highValue = 120,
  });

  configuration.display1Gauges.push_back(GaugeConfig{
      .name = "IAT",
      .units = "C",
      .min = 0,
      .max = 100,
      .lowValue = 0,
      .highValue = 65,
  });

  configuration.display1Gauges.push_back(GaugeConfig{
      .name = "EGT",
      .units = "C",
      .min = 0,
      .max = 1300,
      .lowValue = 0,
      .highValue = 900,
  });

  configuration.display2Gauges.push_back(GaugeConfig{
      .name = "OIL",
      .units = "bar",
      .min = 0,
      .max = 10,
      .lowValue = 1,
      .highValue = 8,
  });

  configuration.display2Gauges.push_back(GaugeConfig{
      .name = "BOOST",
      .units = "bar",
      .min = -1,
      .max = 2,
      .lowValue = -1,
      .highValue = 1.5,
  });

  configuration.display2Gauges.push_back(GaugeConfig{
      .name = "FUEL",
      .units = "bar",
      .min = 0,
      .max = 10,
      .lowValue = 2,
      .highValue = 5,
  });
#endif

  // settling time
  delay(250);

  canvas.setFont();
  canvas.fillScreen(OLED_Backround_Color);
  canvas.setTextColor(OLED_Text_Color);

  // initialise the SSD1331
  oled1.begin();
  oled2.begin();

  oled1.enableDisplay(true);
  oled2.enableDisplay(true);
}

void loop() {
#if Mock
  auto factor = float(analogRead(26)) / 1024;

  Data data;

  // COOLANT
  data.display1Gauges.push_back(GaugeData{.currentValue = 110 * factor});
  // OIL TEMP
  data.display1Gauges.push_back(GaugeData{.currentValue = 130 * factor});
  // IAT
  data.display1Gauges.push_back(GaugeData{.currentValue = 75 * factor});
  // EGT
  data.display1Gauges.push_back(GaugeData{.currentValue = 1000 * factor});

  // OIL press
  data.display2Gauges.push_back(GaugeData{.currentValue = 9 * factor});
  // BOOST
  data.display2Gauges.push_back(GaugeData{.currentValue = 3 * factor});
  // FUEL PRESS
  data.display2Gauges.push_back(GaugeData{.currentValue = 9 * factor});

#endif

  drawDisplay(oled1, configuration.display1Gauges, data.display1Gauges);
  drawDisplay(oled2, configuration.display2Gauges, data.display2Gauges);
  // drawDisplay(oled3, configuration.display3Gauges, data.display3Gauges);

  // rough 60 FPS target
  delay(10);
}