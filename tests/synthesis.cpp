#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/control/ADSR.h"
#include "tms/dsp/SineOscillator.h"
void require(bool ok, const char* message) {
  if (!ok) { std::fprintf(stderr, "%s\n", message); std::exit(1); }
}
bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }
int main() {
  tms::SineOscillator osc;
  osc.setSampleRate(1000); osc.setFrequency(250);
  const float expected[] = {0, 1, 0, -1, 0};
  for (float v : expected) require(near(osc.tick(), v), "quarter-cycle sine");
  osc.reset(); osc.setFrequency(-250);
  require(near(osc.tick(), 0) && near(osc.tick(), -1), "reverse phase");
  osc.setSampleRate(2000);
  require(near(osc.clock.increment, -0.125f), "rate change preserves Hz");
  osc.reset(); float s, c; osc.tickQuadrature(s, c);
  require(near(s, 0) && near(c, 1), "quadrature alignment");
  osc.clock.reset(-2.25f);
  require(near(osc.clock.phase, 0.75f), "negative reset wraps");
  osc.clock.setSampleRate(0); osc.clock.setFrequency(std::numeric_limits<float>::infinity());
  require(near(osc.clock.sampleRate, 2000) && near(osc.clock.increment, 0), "invalid oscillator inputs");

  tms::ADSR env; env.setSampleRate(1000); env.set(4, 2, 0.5f, 2);
  env.noteOn();
  for (int i = 1; i <= 4; ++i) require(near(env.tick(), i * 0.25f), "attack timing");
  require(near(env.tick(), 0.75f) && near(env.tick(), 0.5f), "decay timing");
  require(near(env.tick(), 0.5f), "sustain hold");
  env.noteOff(); require(near(env.tick(), 0.25f) && near(env.tick(), 0), "release timing");
  require(!env.active() && near(env.tick(), 0), "idle silence");
  env.set(0, 0, 0.3f, 0); env.noteOn();
  require(near(env.tick(), 0.3f), "zero stages skip");
  env.noteOff(); require(near(env.tick(), 0) && !env.active(), "zero release");
  env.set(4, 0, 0.5f, 4); env.noteOn(); env.tick(); env.tick(); env.noteOff();
  require(near(env.tick(), 0.375f), "early release starts at current level");
  env.noteOn(); require(near(env.tick(), 0.53125f), "retrigger starts at current level");
  env.reset(); env.set(2, 0, 0, 0); env.noteOn();
  require(near(env.tick(), 0.5f) && near(env.tick(), 1) && near(env.tick(), 0), "timed attack before zero decay");
  env.reset(); env.set(0, 2, 0, 0); env.noteOn();
  require(near(env.tick(), 0.5f) && near(env.tick(), 0), "zero attack before timed decay");
  std::puts("Synthesis checks passed");
}
