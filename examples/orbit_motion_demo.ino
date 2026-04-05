#include <Arduino.h>
#include "tms/control/BlockParam.h"
#include "tms/control/OrbitPath.h"
#include "tms/dsp/AirLoss.h"
#include "tms/dsp/WowFlutter.h"

tms::OrbitPath orbit;
tms::BlockParam distance;
tms::AirLoss air;
tms::WowFlutter wow;
float phase = 0.0f;

void setup() {
  Serial.begin(115200);
  orbit.setType(tms::OrbitPath::Lemniscate);
  orbit.setRadius(1.0f);
  orbit.setHeight(0.0f);
  distance.reset(0.0f);
  distance.setTarget(1.0f);
  air.setAmount(0.8f);
  wow.reset(2.5f, 0.3f, 0.3f, 7.0f);
  Serial.println("Orbit motion demo ready");
}

void loop() {
  float x = 0.0f;
  float y = 0.0f;
  orbit.getXY(phase, x, y);

  // Use the orbit radius as a fake distance control.
  float distTarget = 0.5f * (sqrtf(x * x + y * y));
  distance.setTarget(distTarget);
  distance.beginBlock(1);
  float dist = distance.next();

  float l = 0.8f;
  float r = 0.8f;
  air.setDistanceNorm(dist);
  air.process(l, r);

  wow.beginBlock(1);
  float delayModMs = wow.tick();

  Serial.printf("x=%.2f y=%.2f dist=%.2f air=(%.2f,%.2f) wow_ms=%.2f\n",
                x, y, dist, l, r, delayModMs);

  phase += 0.01f;
  if (phase >= 1.0f) {
    phase -= 1.0f;
  }
  delay(20);
}
