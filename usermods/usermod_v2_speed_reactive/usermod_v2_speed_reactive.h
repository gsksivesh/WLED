#pragma once

#include "wled.h"

static const char _data_FX_MODE_SPEED_REACTIVE[] PROGMEM = "Speed Reactive@!;!,!;!";

uint16_t mode_speed_reactive(void)
{
  static int8_t speed = 0; // Simulated speed

  // Weighted random delta (40% increase, 30% decrease, 30% stay)
  int r = random(0, 10); // 0–9
  int8_t delta = 0;

  if (r < 4)
  {
    delta = 1; // 40% chance
  }
  else if (r < 7)
  {
    delta = -1; // 30% chance
  }
  else
  {
    delta = 0; // 30% chance
  }

  speed += delta;
  speed = constrain(speed, 0, 120);

  uint16_t len = SEGLEN;
  if (len == 0)
  {
    Serial.println("[SpeedReactive] LED length is zero. Exiting effect.");
    return 20;
  }

  Serial.println("----");
  Serial.printf("[SpeedReactive] Simulated Speed: %d km/h\n", speed);
  // Serial.printf("[SpeedReactive] Total LEDs: %d\n", len);

  float ledsPerKmph = (float)len / 30.0;

  uint16_t blueLEDs = (speed <= 30) ? min((uint16_t)(speed * ledsPerKmph), len) : 0;
  uint16_t greenLEDs = (speed > 30) ? min((uint16_t)((speed - 30) * ledsPerKmph), len) : 0;
  uint16_t orangeLEDs = (speed > 60) ? min((uint16_t)((speed - 60) * ledsPerKmph), len) : 0;
  uint16_t redLEDs = (speed > 90) ? min((uint16_t)((speed - 90) * ledsPerKmph), len) : 0;

  if (speed <= 30)
  {
    SEGMENT.fill(RGBW32(255, 255, 255, 0));
    for (uint16_t i = 0; i < blueLEDs && i < len; i++)
    {
      SEGMENT.setPixelColor(i, RGBW32(0, 0, 255, 0));
    }
  }
  else if (speed > 30 && speed <= 60)
  {
    SEGMENT.fill(RGBW32(0, 0, 255, 0));
    for (uint16_t i = 0; i < greenLEDs && i < len; i++)
    {
      SEGMENT.setPixelColor(i, RGBW32(0, 255, 0, 0));
    }
  }
  else if (speed > 60 && speed <= 90)
  {
    SEGMENT.fill(RGBW32(0, 255, 0, 0));
    for (uint16_t i = 0; i < orangeLEDs && i < len; i++)
    {
      SEGMENT.setPixelColor(i, RGBW32(255, 128, 0, 0));
    }
  }
  else
  {
    SEGMENT.fill(RGBW32(255, 128, 0, 0));
    for (uint16_t i = 0; i < redLEDs && i < len; i++)
    {
      SEGMENT.setPixelColor(i, RGBW32(255, 0, 0, 0));
    }
  }

  return 100;
}

class SpeedReactiveUsermod : public Usermod
{

private:
  static const char _name[];
  bool initDone = false;
  unsigned long lastTime = 0;
  static const char _enabled[];
  bool enabled = false;

public:
  void setup() override
  {
    strip.addEffect(255, &mode_speed_reactive, _data_FX_MODE_SPEED_REACTIVE);
    initDone = true;
  }

  void loop() override
  {
    if (!enabled || strip.isUpdating())
      return;

    // do your magic here
    if (millis() - lastTime > 1000)
    {
      lastTime = millis();
    }
    // print if GPS data is available
  }
  void handleOverlayDraw() override
  {
  }

  void addToConfig(JsonObject &root) override
  {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    top[FPSTR(_enabled)] = enabled;
  }

  bool readFromConfig(JsonObject &root) override
  {
    JsonObject top = root[FPSTR(_name)];
    bool configComplete = !top.isNull();
    configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
    return configComplete;
  }
  uint16_t getId() override
  {
    return USERMOD_ID_SPEED_REACTIVE;
  }
};

const char SpeedReactiveUsermod::_name[] PROGMEM = "Speed Reactive";
const char SpeedReactiveUsermod::_enabled[] PROGMEM = "enabled";
