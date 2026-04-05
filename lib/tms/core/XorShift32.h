#pragma once
#include <stdint.h>
namespace tms {
// Adapted from seedbox/src/util/RNG.h.
// Tiny deterministic PRNG for repeatable modulation, jitter, and pattern demos.
struct XorShift32 {
  uint32_t state = 2463534242u;

  inline void seed(uint32_t value) { state = value ? value : 2463534242u; }

  inline uint32_t nextU32() {
    uint32_t x = state ? state : 2463534242u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
  }

  inline float uniform01() {
    return (nextU32() >> 8) * (1.0f / 16777216.0f);
  }

  inline float uniformSigned() { return (uniform01() * 2.0f) - 1.0f; }
};
}
