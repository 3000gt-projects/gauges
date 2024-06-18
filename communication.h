#pragma once

#include <ArduinoJson.h>
#include <cstdint>

#include "ArduinoJson/Array/JsonArray.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"
#include "dto.h"

InMessageType getMessageType(const ArduinoJson::JsonObject &json) {
  uint8_t messageType = json["type"];
  return static_cast<InMessageType>(messageType);
}

GaugeTheme parseGaugeTheme(const ArduinoJson::JsonObject &json) {
  GaugeTheme result;

  result.alertColor = json["alert_color"];
  result.highColor = json["high_color"];
  result.lowColor = json["low_color"];
  result.okColor = json["ok_color"];

  return result;
}

GaugeConfig parseGaugeConfig(const ArduinoJson::JsonObject &json) {
  GaugeConfig result;

  result.name = json["name"];
  result.units = json["units"];
  result.format = json["format"];
  result.min = json["min"];
  result.max = json["max"];
  result.lowValue = json["low_value"];
  result.highValue = json["high_value"];

  return result;
}

Configuration::Display
parseDisplayConfiguration(const ArduinoJson::JsonObject &json) {
  Configuration::Display result;

  for (auto gaugeJson : json["gauges"].as<ArduinoJson::JsonArray>()) {
    result.gauges.push_back(
        parseGaugeConfig(gaugeJson.as<ArduinoJson::JsonObject>()));
  }

  return result;
}

GaugeData parseGaugeData(const ArduinoJson::JsonObject &json) {
  GaugeData result;

  result.currentValue = json["current_value"];

  return result;
}

Data::Display parseDisplayData(const ArduinoJson::JsonObject &json) {
  Data::Display result;

  for (auto gaugeJson : json["gauges"].as<ArduinoJson::JsonArray>()) {
    result.gauges.push_back(
        parseGaugeData(gaugeJson.as<ArduinoJson::JsonObject>()));
  }

  return result;
}

Configuration parseConfiguration(const ArduinoJson::JsonObject &json) {
  Configuration configuration;

  configuration.theme =
      parseGaugeTheme(json["theme"].as<ArduinoJson::JsonObject>());

  configuration.display1 =
      parseDisplayConfiguration(json["display1"].as<ArduinoJson::JsonObject>());
  configuration.display2 =
      parseDisplayConfiguration(json["display2"].as<ArduinoJson::JsonObject>());
  configuration.display3 =
      parseDisplayConfiguration(json["display3"].as<ArduinoJson::JsonObject>());

  return configuration;
}

Data parseData(const ArduinoJson::JsonObject &json) {
  Data data;

  data.display1 =
      parseDisplayData(json["display1"].as<ArduinoJson::JsonObject>());
  data.display2 =
      parseDisplayData(json["display2"].as<ArduinoJson::JsonObject>());
  data.display3 =
      parseDisplayData(json["display3"].as<ArduinoJson::JsonObject>());

  return data;
}