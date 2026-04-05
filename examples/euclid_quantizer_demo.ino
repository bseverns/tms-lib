#include <Arduino.h>
#include "tms/core/XorShift32.h"
#include "tms/control/EuclideanPattern.h"
#include "tms/control/ScaleQuantizer.h"

tms::EuclideanPattern pattern;
tms::XorShift32 rng;
uint32_t stepCounter = 0;

void setup() {
  Serial.begin(115200);
  pattern.set(16, 5, 2);
  rng.seed(20260404u);
  Serial.println("Euclid + quantizer demo ready");
}

void loop() {
  bool gate = pattern.tick();
  float rawPitch = 48.0f + rng.uniformSigned() * 12.0f;
  float snapped = tms::ScaleQuantizer::nearest(
      rawPitch, 0, tms::ScaleQuantizer::Scale::Minor);

  Serial.printf("step=%02lu gate=%d raw=%.2f snapped=%.2f\n",
                static_cast<unsigned long>(stepCounter), gate ? 1 : 0,
                rawPitch, snapped);

  ++stepCounter;
  delay(125);
}
