#pragma once

#include <Arduino.h>

#include "dto.h"

Configuration mockConfiguration() {
  Configuration configuration;

  configuration.display1.gauges.push_back(GaugeConfig{
      .name = "COOLANT",
      .units = "C",
      .format = "%.0f",
      .min = 0,
      .max = 130,
      .lowValue = 60,
      .highValue = 100,
  });

  configuration.display1.gauges.push_back(GaugeConfig{
      .name = "OIL",
      .units = "C",
      .format = "%.0f",
      .min = 0,
      .max = 150,
      .lowValue = 60,
      .highValue = 120,
  });

  configuration.display1.gauges.push_back(GaugeConfig{
      .name = "IAT",
      .units = "C",
      .format = "%.0f",
      .min = 0,
      .max = 100,
      .lowValue = 0,
      .highValue = 65,
  });

  configuration.display1.gauges.push_back(GaugeConfig{
      .name = "EGT",
      .units = "C",
      .format = "%.0f",
      .min = 0,
      .max = 1300,
      .lowValue = 0,
      .highValue = 900,
  });

  configuration.display2.gauges.push_back(GaugeConfig{
      .name = "OIL",
      .units = "bar",
      .format = "%.2f",
      .min = 0,
      .max = 10,
      .lowValue = 1,
      .highValue = 8,
  });

  configuration.display2.gauges.push_back(GaugeConfig{
      .name = "BOOST",
      .units = "bar",
      .format = "%.2f",
      .min = -1,
      .max = 2,
      .lowValue = -1,
      .highValue = 1.5,
  });

  configuration.display2.gauges.push_back(GaugeConfig{
      .name = "FUEL",
      .units = "bar",
      .format = "%.2f",
      .min = 0,
      .max = 10,
      .lowValue = 2,
      .highValue = 5,
  });

  return configuration;
}

Data mockData() {
  auto factor = float(analogRead(26)) / 1024;

  Data data;

  // COOLANT
  data.display1.gauges.push_back(GaugeData{.currentValue = 110 * factor});
  // OIL TEMP
  data.display1.gauges.push_back(GaugeData{.currentValue = 130 * factor});
  // IAT
  data.display1.gauges.push_back(GaugeData{.currentValue = 75 * factor});
  // EGT
  data.display1.gauges.push_back(GaugeData{.currentValue = 1000 * factor});

  // OIL press
  data.display2.gauges.push_back(GaugeData{.currentValue = 9 * factor});
  // BOOST
  data.display2.gauges.push_back(GaugeData{.currentValue = 3 * factor - 1.f});
  // FUEL PRESS
  data.display2.gauges.push_back(GaugeData{.currentValue = 9 * factor});

  return data;
}