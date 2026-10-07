#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "tms/control/BlockParam.h"
#include "tms/control/EuclideanPattern.h"
#include "tms/control/Param.h"
#include "tms/control/ScaleQuantizer.h"
#include "tms/core/ObjectPool.h"
#include "tms/core/DelayLine.h"
#include "tms/dsp/Biquad.h"
#include "tms/dsp/MSMatrix.h"
#include "tms/dsp/CombBank.h"
#include "tms/dsp/Plate.h"

void check(bool ok, const char* message) {
  if (!ok) { std::fprintf(stderr, "%s\n", message); std::exit(1); }
}
bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }
struct Voice { static int live; Voice() { ++live; } ~Voice() { --live; } };
int Voice::live = 0;
int main() {
  tms::Param p;
  p.init({"test", 0, 1, 0, tms::Scale::Linear, 10});
  p.setTarget(2);
  for (int i = 0; i < 441; ++i) p.tick();
  check(near(p.current(), 1.0f - std::exp(-1.0f)), "10 ms must reach 1 - exp(-1)");
  tms::BlockParam ramp;
  ramp.reset(0); ramp.setTarget(1); ramp.beginBlock(4);
  for (int i = 0; i < 4; ++i) check(near(ramp.next(), i * 0.25f), "block ramp samples");
  check(near(ramp.next(), 1), "block ramp endpoint");
  tms::EuclideanPattern pattern;
  for (int steps = 1; steps <= 32; ++steps) {
    for (int fills = 0; fills <= steps; ++fills) {
      pattern.set(steps, fills, steps - 1);
      int hits = 0;
      for (int i = 0; i < steps; ++i) hits += pattern.tick();
      check(hits == fills, "Euclidean pulse count");
    }
  }
  using Q = tms::ScaleQuantizer;
  check(near(Q::nearest(-1, 0, Q::Scale::Major), -1), "negative pitch quantization");
  float m, s, l, r;
  tms::MSMatrix matrix;
  matrix.encode(0.3f, -0.7f, m, s);
  matrix.decode(m, s, l, r);
  check(near(l, 0.3f) && near(r, -0.7f), "M/S round trip");
  static tms::DelayLine delay;
  delay.setMaxDelayMs(1);
  delay.write(1); delay.write(0);
  check(near(delay.read(2), 1) && near(delay.read(1.5f), 0.5f), "fractional delay impulse");
  auto filter = tms::Biquad::makeLowpass(44100, 1000, 0.707f);
  float dc = 0;
  for (int i = 0; i < 4410; ++i) dc = filter.tick(1);
  check(near(dc, 1), "lowpass DC gain");
  static tms::CombBank comb;
  static tms::Plate plate;
  float energy = 0;
  for (int i = 0; i < 44100; ++i) {
    const float impulse = i == 0 ? 1 : 0;
    auto a = comb.process(impulse); auto b = plate.process(impulse);
    check(std::isfinite(a.left) && std::isfinite(b.right), "finite resonator tail");
    check(std::fabs(a.left) < 2 && std::fabs(b.right) < 2, "bounded resonator tail");
    energy += std::fabs(a.left) + std::fabs(b.right);
  }
  check(energy > 0, "resonators produce a tail");
  tms::ObjectPool<Voice, 2> pool;
  auto* first = pool.acquire(); auto* second = pool.acquire();
  check(first && second && !pool.acquire() && Voice::live == 2, "pool capacity");
  pool.release(first); pool.release(second);
  check(Voice::live == 0, "pool destruction");
  std::puts("Teaching block behavior checks passed");
}
