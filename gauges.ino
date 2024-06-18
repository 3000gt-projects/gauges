#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Arduino.h>
#include <SPI.h>
#include <cstdint>

#define EnableDebugging false
#define Mock false

#include "communication.h"
#include "gauge.h"
#include "mock.h"

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

ArduinoJson::JsonDocument configurationMessageDocument;
Configuration configuration;

bool configurationIsEmpty() {
  return configuration.display1.gauges.empty() &&
         configuration.display2.gauges.empty() &&
         configuration.display3.gauges.empty();
}

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

  if (displayConfiguration.gauges.size() == displayData.gauges.size()) {
    for (size_t gaugeIndex = 0; gaugeIndex < displayConfiguration.gauges.size();
         ++gaugeIndex) {
      drawGauge(canvas, displayConfiguration.gauges[gaugeIndex], layout,
                configuration.theme, displayData.gauges[gaugeIndex]);
    }
  } else {
    for (auto &gaugeConfiguration : displayConfiguration.gauges) {
      drawGauge(canvas, gaugeConfiguration, layout, configuration.theme,
                badGaugeData);
    }
  }

  debug("Draw display");
  display.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(),
                        canvas.height());
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);

#if (Mock)
  configuration = mockConfiguration();
#endif

  // settling time
  delay(250);

  // initialise the SSD1331
  oled1.begin();
  oled2.begin();

  oled1.enableDisplay(true);
  oled2.enableDisplay(true);
}

void loop() {
#if Mock
  // rough 60 FPS target
  delay(10);
  auto data = mockData();
#else

  ArduinoJson::JsonDocument messageDocument;

  if (configurationIsEmpty()) {
    messageDocument["type"] = (uint8_t)OutMessageType::NeedGaugeConfig;

    debug("Sending config request");

    String messageString;
    ArduinoJson::serializeJson(messageDocument, messageString);
    debug(messageString);

    debug("Sent config request");

    messageDocument.clear();

    ArduinoJson::deserializeJson(messageDocument, Serial.readStringUntil('\n'));

    auto messageObject = messageDocument.as<ArduinoJson::JsonObject>();

    if (getMessageType(messageObject) == InMessageType::Configuration) {
      configurationMessageDocument = std::move(messageDocument);
      messageDocument.clear();

      configuration = parseConfiguration(configurationMessageDocument["message"]
                                             .as<ArduinoJson::JsonObject>());

      Serial.print("Gauges: ");
      Serial.println(configuration.display1.gauges.size());
    }
    return;
  }

  messageDocument["type"] = (uint8_t)OutMessageType::NeedGaugeData;

  ArduinoJson::serializeJson(messageDocument, Serial);
  Serial.print("\n");

  messageDocument.clear();

  debug("Reading");

  auto json_string = Serial.readStringUntil('\n');
  Serial.print("\n");

  debug("Got String: " + json_string);

  ArduinoJson::deserializeJson(messageDocument, json_string);

  debug("Deserialized");

  auto messageObject = messageDocument.as<ArduinoJson::JsonObject>();

  debug("Got object");

  // reconfigure
  if (getMessageType(messageObject) == InMessageType::Configuration) {
    debug("Reconfigure");

    configurationMessageDocument = std::move(messageDocument);
    messageDocument.clear();

    configuration = parseConfiguration(
        configurationMessageDocument["message"].as<ArduinoJson::JsonObject>());
    debug("Parsed configuration");

    debug("Gauges: " + configuration.display1.gauges.size());
    return;
  }

  if (getMessageType(messageObject) != InMessageType::Data) {
    return;
  }

  debug("Got Data");

  auto data = parseData(messageObject["message"].as<ArduinoJson::JsonObject>());

  debug("Parsed Data");
  debug("Gauges: " + data.display1.gauges.size());

#endif

  drawDisplay(oled1, configuration.display1, data.display1);
  drawDisplay(oled2, configuration.display2, data.display2);
  // drawDisplay(oled3, configuration.display3, data.display3);
}