#include <Arduino.h>
#include "tms/dsp/MSMatrix.h"
#include "tms/dsp/DynamicWidth.h"
#include "tms/dsp/XFeedMatrix.h"

tms::MSMatrix ms;
tms::DynamicWidth width;
tms::XFeedMatrix xfeed;
float phase = 0.0f;

void setup() {
  Serial.begin(115200);
  width.setBaseWidth(0.65f);
  width.setDynAmount(0.5f);
  xfeed.set(0.15f, 0.15f);
  Serial.println("Stereo image demo ready");
}

void loop() {
  float inL = sinf(phase);
  float inR = cosf(phase * 0.7f);
  float m = 0.0f;
  float s = 0.0f;
  ms.encode(inL, inR, m, s);

  // Fake a transient meter with a slow LFO so students can see width move.
  float transientActivity = 0.5f + 0.5f * sinf(phase * 0.25f);
  width.process(m, s, transientActivity);

  float wideL = 0.0f;
  float wideR = 0.0f;
  ms.decode(m, s, wideL, wideR);

  float outL = 0.0f;
  float outR = 0.0f;
  xfeed.process(wideL, wideR, outL, outR);

  Serial.printf("in=(%.2f,%.2f) mid=%.2f side=%.2f width=%.2f out=(%.2f,%.2f)\n",
                inL, inR, m, s, width.lastWidth, outL, outR);

  phase += 0.15f;
  delay(60);
}
