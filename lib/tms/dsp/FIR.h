#pragma once
#include "tms/core/Types.h"
#include <array>
#include <stddef.h>
namespace tms {
// Adapted from ir-postcards/firmware/src/IRConvolverCore.{h,cpp}.
// Copyright (c) 2025 Ben Severns. MIT; see the repository LICENSE.
// Direct mono FIR: y[n] = sum(h[k] * x[n-k]). One tick per sample.
// Capacity is compile-time; coefficients and history are owned, with no heap.
// Default kernel is identity. reset() clears history and preserves the kernel.
// load() copies finite taps, resets history, and rejects invalid kernels without
// changing state. Load/configure outside the audio callback. No normalization,
// clipping, sample-rate conversion, or extra block latency is introduced here.
template<size_t Capacity = 256>
class FIR {
  static_assert(Capacity > 0, "FIR capacity must be positive");
  std::array<float, Capacity> taps_ = {};
  std::array<float, Capacity> history_ = {};
  size_t count_ = 1;
  size_t head_ = 0;
public:
  FIR() { taps_[0] = 1.0f; }
  size_t tapCount() const { return count_; }
  void reset() { history_.fill(0.0f); head_ = 0; }
  bool load(const float* taps, size_t count) {
    if (!taps || count == 0 || count > Capacity) return false;
    for (size_t i = 0; i < count; ++i) if (!isfinite(taps[i])) return false;
    taps_.fill(0.0f);
    for (size_t i = 0; i < count; ++i) taps_[i] = taps[i];
    count_ = count;
    reset();
    return true;
  }
  float tick(float input) {
    head_ = head_ == 0 ? Capacity - 1 : head_ - 1;
    history_[head_] = input;
    float output = 0.0f;
    size_t index = head_;
    for (size_t tap = 0; tap < count_; ++tap) {
      output += taps_[tap] * history_[index];
      if (++index == Capacity) index = 0;
    }
    return output;
  }
  // Exact in-place processing is supported; other overlap is unsupported.
  // Empty blocks succeed without accessing pointers; invalid buffers fail.
  bool processBlock(const float* input, float* output, size_t frames) {
    if (frames == 0) return true;
    if (!input || !output) return false;
    for (size_t i = 0; i < frames; ++i) output[i] = tick(input[i]);
    return true;
  }
};
}
