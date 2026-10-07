#include <cstdio>
#include "tms/control/EnvelopeFollower.h"
#include "tms/dsp/Biquad.h"

int main() {
  auto filter = tms::Biquad::makeLowpass(tms::kSampleRate, 1000.0f, 0.707f);
  tms::EnvelopeFollower envelope;
  std::puts("sample,input,filtered,envelope");
  for (int i = 0; i < 4410; ++i) {
    const float input = i == 0 ? 1.0f : 0.0f;
    const float filtered = filter.tick(input);
    const float level = envelope.tick(filtered);
    if (i < 16 || i % 441 == 0)
      std::printf("%d,%.6f,%.6f,%.6f\n", i, input, filtered, level);
  }
}
