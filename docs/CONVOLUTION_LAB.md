# Third expansion: short FIR convolution

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_convolution build/convolution.wav
```

Listen to four two-second comparisons. Each section repeats the same impulse
and three seeded noise bursts, so changes come from the kernel rather than input.

| Time | Kernel | What to observe |
| --- | --- | --- |
| 0–2 s | Identity `{1}` | Unchanged input |
| 2–4 s | Eight-tap moving average | Softer high frequencies; impulse spreads over eight samples |
| 4–6 s | Difference `{0.5,-0.5}` | High-frequency emphasis; constant signals cancel |
| 6–8 s | Sparse reflections at 0, 10, 25, and 45 ms | Impulse becomes four clicks; noise gains short echoes |

These are synthetic teaching kernels, not captured room responses. The final
section uses a 2048-tap capacity to make reflections audible; the ordinary
header default is 256 taps, matching the source project's short FIR scale.

## The mechanism

Read [FIR.h](../lib/tms/dsp/FIR.h). For each sample the algorithm stores the new
input in a circular history buffer, then multiplies each delayed input by its
corresponding coefficient and adds the products:

`y[n] = h[0]x[n] + h[1]x[n-1] + ... + h[K-1]x[n-K+1]`

An impulse contains one nonzero sample. Feeding it through the filter therefore
returns the coefficients themselves: the kernel is the impulse response.
A moving average is convolution just as much as a short room response is.

```cpp
tms::FIR<256> filter;
const float response[] = {0.7f, 0.0f, 0.2f, 0.1f};
if (filter.load(response, 4)) {
  float wet = filter.tick(input); // once per output sample
}
```

Try changing the reflection positions/signs or averaging length. To study gain,
compare `{1}` with `{2}` using quiet input. To study delay, compare `{1}` with
`{0,0,1}`. The FIR deliberately does not normalize or clip its output.

## Contract and limits

- Mono float input/output; taps are linear gains and delays are whole samples.
  `h[0]` acts on the current input. Tick once per sample; block processing calls
  the same tick and retains history across block boundaries.
- Default construction is identity. `load` copies the kernel and clears history.
  Null, empty, oversized, or non-finite kernels are rejected with state intact.
  Oversized kernels are rejected rather than silently truncated.
- `reset` clears history while preserving coefficients. Loading and resetting
  should occur outside the audio callback and in the same processing context,
  or with external synchronization. Replacing a kernel clears its tail and may
  cause an audible discontinuity; smooth IR switching is a future layer.
- `processBlock` supports exact in-place operation. Other buffer overlap is
  unsupported. Zero-frame blocks succeed without pointer access; null buffers
  for nonempty blocks fail without advancing state.
- Storage is `2 * Capacity * sizeof(float)` plus counters. At 256 taps this is
  2048 bytes for samples/taps; at 2048 taps it is 16,384. No heap allocation.
- Work is proportional to active tap count per output sample. There is no extra
  block-buffer latency, but the kernel itself can introduce delay. This is
  direct convolution, not FFT/partitioned convolution. The desktop example's
  2048-tap kernel is not a claim of real-time Teensy performance.
- The kernel must be prepared for the processing sample rate. There is no
  automatic IR resampling or WAV decoding. At 44.1 kHz a 256-sample kernel spans
  about 5.8 ms; this demonstrates short responses and FIR filters, not long room
  reverberation. Stereo uses two independent instances.
- Finite, appropriately scaled audio is expected. Large input/tap gains can
  overflow; the conservative output bound is `maxInput * sum(abs(taps))`.
  Keep output gain control separate from the convolution mechanism.

## Provenance and verification

Adapted from MIT-licensed `ir-postcards/firmware/src/IRConvolverCore.h` and
`IRConvolverCore.cpp`, specifically the circular history and `convolveSample`
weighted sum. Copyright (c) 2025 Ben Severns; the repository MIT license retains
that notice. The adaptation keeps one kernel and one channel with compile-time
capacity. It omits slot/catalog management, predelay, width, tone filters,
freeze, and ceiling. Existing tms blocks can supply those compositional layers.

The source's `tests/test_ir_convolver_core.cpp` was inspected as context.
`tests/fir.cpp` checks identity, impulse response, delayed taps, gain, wrap-around,
a direct reference convolution, split and in-place blocks, reset/replacement,
one-tap capacity, and invalid kernels/buffers.
