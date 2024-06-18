#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "dto.h"

void debug(const String &message) {
#if EnableDebugging
#if Mock
  Serial.println(message);
#else
  ArduinoJson::JsonDocument messageDocument;
  messageDocument["type"] = (uint8_t)OutMessageType::Debug;
  messageDocument["message"] = message;
  String messageString;
  ArduinoJson::serializeJson(messageDocument, messageString);
  Serial.println(messageString);
#endif
#endif
}