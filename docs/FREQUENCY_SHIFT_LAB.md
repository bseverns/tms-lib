# Fifth expansion: frequency shifting

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_frequency_shift build/frequency_shift.wav
```

Listen to four two-second sections of the same 600/1200/1800 Hz harmonic sound:

| Time | Shift | Resulting main frequencies |
| --- | --- | --- |
| 0–2 s | 0 Hz | 600, 1200, 1800 Hz |
| 2–4 s | +100 Hz | 700, 1300, 1900 Hz |
| 4–6 s | -100 Hz | 500, 1100, 1700 Hz |
| 6–8 s | +300 Hz | 900, 1500, 2100 Hz |

Frequency shifting adds Hz to each component. Pitch transposition multiplies
frequencies by a ratio; compare this lab with the sample-playback lab. The
translated partials no longer retain their original harmonic relationship.

## Two readable mechanisms

[HilbertPair.h](../lib/tms/dsp/HilbertPair.h) makes an approximate analytic pair:
`I` is a delayed copy of the real input; `Q` is its Hilbert transform, so a
cosine input becomes a corresponding sine. This implementation generates a
finite windowed ideal FIR, rather than using precomputed allpass coefficients.
For offset `m` from the center tap, odd offsets have `h[m]=2/(pi*m)`; even
offsets are zero. A symmetric Hann window limits the response length.

[FrequencyShifter.h](../lib/tms/dsp/FrequencyShifter.h) combines that pair with
the existing sine/quadrature oscillator:

`output = I*cos(phase) - Q*sin(phase)`

For a cosine at frequency `f`, the two products cancel one modulation sideband
and retain the component at `f + shift`. Negative oscillator frequency reverses
the translation. The FIR approximation leaves a small unwanted image.

```cpp
tms::FrequencyShifter<> shifter; // 129 taps by default
shifter.setSampleRate(44100);
shifter.setShiftHz(100);
float translated = shifter.tick(input); // once per sample
```

Try 10 Hz shifts for slow changes in harmonic relationships or compare positive
and negative values. A dry/wet blend also creates beating; align the dry branch
to the documented FIR delay if you want a latency-matched comparison.

## Contracts and limits

- Mono float audio; shift and sample rate are Hz. Tick once per output sample.
  There is no hardware dependency or dynamic allocation.
- The default Hilbert FIR has 129 taps and exactly 64 samples of matching real
  delay (about 1.45 ms at 44.1 kHz). Template tap count must be odd and at least
  three. Both paths align to the center-tap delay; quadrature is approximate.
- Zero shift returns the delayed real input, not immediate passthrough. `reset`
  clears both filter histories and oscillator phase, retaining the settings.
  New settings preserve histories/phase; abrupt changes can be audible.
- `setShiftHz` rejects non-finite shifts and shifts at or beyond +/- Nyquist.
  `setSampleRate` rejects nonpositive/non-finite rates or a rate whose Nyquist
  would exclude the current shift. Rejected settings leave state unchanged.
- Configure/reset and process in one context or synchronize externally. Finite,
  sensibly scaled input is expected. There is no output clipping, normalization,
  feedback stage, or automatic DC removal.
- The approximation weakens near DC and Nyquist. Longer FIRs improve the useful
  band at the cost of latency, memory, and CPU. This is a teaching implementation,
  not a full-band precision shifter or a measured real-time hardware guarantee.
- Translation can move frequencies above Nyquist, causing aliasing. A downward
  shift crossing zero folds into a positive audible frequency for real output.
  The setters validate the oscillator, not the input spectrum; choose source
  bandwidth and shift together. No anti-alias filter is supplied automatically.
- Filter storage is `sizeof(float) * (2*Taps + (Taps-1)/2 + 1)` plus counters,
  and the shifter adds oscillator state. Default audio/coefficient storage is
  1292 bytes. Each sample computes a direct FIR dot product and oscillator trig.
  The coefficient-generation constructor belongs in setup, outside the callback.

## Provenance and verification

Original standard FIR/Hilbert and quadrature-modulation implementations,
conceptually related to `FieldKitFX/engine/hilbertTransformer.h`,
`hilbertTransformer.c`, `frequencyShifter.h`, and `frequencyShifter.c`.
Those source files use two allpass branches and a hardware/CV wrapper. The
source repository carries GPL-3.0; neither its code nor its coefficient table
was imported. This version derives its own FIR coefficients and reuses the
MIT tms `FIR`, `hannWindow`, and `SineOscillator` blocks. It does not promise
matching sound, phase, latency, or sideband-mix controls.

`tests/frequency_shifter.cpp` checks the real-branch impulse delay, reset,
zero-shift delayed identity, settings validation, and signed translation using
frequency correlation after settling. At 44.1 kHz, 1 kHz and 4 kHz input tones
with +/-200 Hz shifts must retain amplitude within 2% and suppress the unwanted
sideband below -40 dB. These are specific test points, not a full-band guarantee.
The local 1 kHz result was approximately -55 dB.
