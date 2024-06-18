#pragma once

#include <float.h>
#include <stdint.h>

#include <Array.h>

#include "colors.h"

enum class InMessageType : uint8_t {
  None,
  Configuration,
  Data,
};

enum class OutMessageType : uint8_t {
  None,
  NeedGaugeConfig,
  NeedGaugeData,
  Debug,
};

float lesserValue(float value) {
  auto intValue = *(int32_t *)&value;
  --intValue;
  return *(float *)(&intValue);
}

struct GaugeTheme {
  uint16_t okColor = OLED_Color_Warm;
  uint16_t lowColor = OLED_Color_Blue;
  uint16_t highColor = OLED_Color_Red;
  uint16_t alertColor = OLED_Color_Red;
};

struct GaugeConfig {
  const char *name = "???";
  const char *units = "???";
  const char *format = "%.2f";
  float min = 0;
  float max = 100;
  float lowValue = 10;
  float highValue = 90;
};

struct GaugeData {
  inline static const float offlineValue = FLT_MAX;
  inline static const float badDataValue = lesserValue(offlineValue);

  float currentValue;
};

struct Configuration {
  struct Display {
    using Gauges = Array<GaugeConfig, 10>;

    Gauges gauges;
  };

  GaugeTheme theme;
  Display display1;
  Display display2;
  Display display3;
};

struct Data {
  struct Display {
    using Gauges = Array<GaugeData, 10>;

    Gauges gauges;
  };

  Display display1;
  Display display2;
  Display display3;
};