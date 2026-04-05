#include <Arduino.h>
#include "tms/core/Types.h"
#include "tms/control/Param.h"
#include "tms/control/ChaosEngine.h"

tms::Param cutoff;
tms::ChaosEngine chaos;

void setup(){
  Serial.begin(115200);
  tms::ParamSpec spec = {"cutoff_hz", 200.0f, 8000.0f, 1000.0f, tms::Scale::Log, 10.0f};
  cutoff.init(spec);
  chaos.reseed(1234);
  Serial.println("Param demo ready");
}

void loop(){
  // Move the target gently with a pseudo-random walk, then let the smoother
  // chase it. This is a simple zipper-noise lecture in 10 lines.
  float step = (chaos.rand01()-0.5f)*200.0f;
  cutoff.setTarget(cutoff.current() + step);
  float v = cutoff.tick();
  Serial.printf("target=%.1f current=%.1f\n", cutoff.target(), v);
  delay(10);
}
