#pragma once
#include "tms/core/Types.h"
#include <stddef.h>
namespace tms {
// Non-owning mono float samples. The caller keeps storage alive and immutable
// during playback. Slice bounds are [begin,end), expressed in source samples.
struct SampleView {
  const float* data = nullptr;
  size_t size = 0;
  SampleView slice(size_t begin, size_t end) const {
    if (!data || begin >= end || end > size) return {};
    return {data + begin, end - begin};
  }
  // Equal-sized slices; the last slice receives the remainder (lofi-sampler).
  SampleView equalSlice(size_t index, size_t count) const {
    if (!count || index >= count) return {};
    size_t width = size / count;
    size_t begin = index * width;
    return slice(begin, index + 1 == count ? size : begin + width);
  }
  // Fractional read. One-shot reads clamp endpoints; loop reads interpolate
  // across the seam. Double position avoids losing sub-sample resolution on
  // longer buffers. Empty views and non-finite positions return silence.
  float read(double position, bool loop = false) const {
    if (!data || !size || !isfinite(position)) return 0.0f;
    if (loop) {
      position = fmod(position, static_cast<double>(size));
      if (position < 0) position += size;
    } else {
      if (position <= 0) return data[0];
      if (position >= static_cast<double>(size - 1)) return data[size - 1];
    }
    size_t first = static_cast<size_t>(position);
    size_t second = first + 1;
    if (second == size) second = loop ? 0 : first;
    float fraction = static_cast<float>(position - first);
    return data[first] + (data[second] - data[first]) * fraction;
  }
};
}
