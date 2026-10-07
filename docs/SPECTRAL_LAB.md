# Sixth expansion: frames, overlap-add, and spectral hold

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_spectral build/spectral_hold.wav
```

Listen to four two-second sections. In each one the source changes from a
440/880 Hz pair to a 660/1320 Hz pair at 0.75 seconds into the section.

| Time | Mode | What to observe |
| --- | --- | --- |
| 0–2 s | Live spectral reconstruction | The source change comes through |
| 2–4 s | Hold enabled 0.5 s into section | Captured tone continues through the source change |
| 4–6 s | Same hold with 0.35 s decay constant | Frozen tone fades away |
| 6–8 s | Hold, then release 1.25 s into section | Frozen tone gives way to the new live source |

Source changes and section boundaries are faded for the comparison. The kernels
are mono; there is no stereo motion/blur in this first spectral lab.

## Read three layers

[FFT.h](../lib/tms/spectral/FFT.h) is a radix-2 complex FFT. It transforms two
fixed arrays in place. The forward transform uses the negative exponential and
no normalization; the inverse divides by N. A real cosine has matching positive
and negative frequency bins.

[SpectralFrame.h](../lib/tms/spectral/SpectralFrame.h) gathers overlapping
frames, applies an analysis window, transforms, calls a spectrum processor,
inverts, applies a synthesis window, and adds into a future output ring. A
second ring accumulates window weights, so output is normalized per sample.
The no-op processor reconstructs the original waveform after a fixed delay.
Zero-padding before startup allows the initial samples to reconstruct too.

The analysis and synthesis windows are square roots of a **periodic Hann**:
`0.5 - 0.5*cos(2*pi*index/N)`. Their product is Hann. The symmetric Hann used by
`GrainVoice` instead divides phase by `N-1` to make both endpoint samples zero.
These windows serve different framing purposes.

[SpectralHold.h](../lib/tms/spectral/SpectralHold.h) captures magnitudes and
phase advances on the next complete frame after hold is requested. It estimates
phase advance from consecutive analyses: nominal bin advance plus the wrapped
residual between observed phases. Held frames keep advancing those phases,
rather than restarting the same waveform frame. Conjugate symmetry is restored
before inversion, and DC/Nyquist imaginary parts are zeroed for real output.

```cpp
tms::SpectralHold<> hold; // 512-sample frame, 128-sample hop
hold.setSampleRate(44100);
hold.setDecaySeconds(0); // infinite hold
hold.setHold(true);      // capture next completed frame
float wet = hold.tick(input); // once per sample
```

For custom spectral experiments, use `SpectralFrame::tick(input, processor)`.
The callback receives full real/imaginary FFT arrays once per hop. Apply gains
to conjugate bin pairs to preserve real output; keep DC and Nyquist real.
The callback must do bounded work and avoid allocation/locks.

## Contracts and practical limits

- N must be a power of two, at least four. Hop must divide N and be no greater
  than N/2. Defaults are N=512 and Hop=128. Both are compile-time choices.
- Streaming latency is exactly N samples: about 11.61 ms at 44.1 kHz by default.
  Frame processing runs every Hop ticks. FFT bin spacing is sampleRate/N,
  approximately 86.13 Hz by default; phase advance helps sustain off-bin tones.
- Hold takes effect at the next completed frame, up to Hop ticks after the
  request. The capture frame itself is unchanged. Subsequent held frames ignore
  live input; held magnitudes and phase advances are retained. Holding before
  useful input has arrived can capture silence or a startup transient.
- Release resumes the unmodified live spectrum on the next frame. Previously
  scheduled held frames overlap with it in the output ring, so transitions take
  additional frame time. Rapid toggles between frame boundaries are sampled
  at the frame cadence. This is not a click-free IR-style transition system.
- Decay is an exponential magnitude time constant in seconds: after one time
  constant the magnitude is approximately 1/e, not zero. Zero means infinite
  retention. Invalid/non-finite or negative decay values are rejected. Changing
  sample rate adjusts the decay coefficient; frame and hop lengths stay fixed
  in samples. Invalid/nonpositive rates are rejected without changing settings.
- `SpectralFrame::reset` clears rings and analysis state. `SpectralHold::reset`
  also clears the capture and hold request while preserving rate/decay settings.
  Configure/reset/tick in one processing context or synchronize externally.
- Each `SpectralFrame` stores 8*N floats plus counters. The hold layer adds
  four arrays of N/2+1 floats and scalar state: default float-array storage is
  20,496 bytes, excluding counters/padding. No heap allocation. Use static
  storage for embedded instances and budget the whole patch.
- FFTs and synthesis run in bursts at frame boundaries; average CPU alone is
  insufficient to assess callback deadlines. Hardware performance is unmeasured.
  Output is not clipped/normalized for modified spectral gains, beyond overlap
  weight normalization. Finite, sensibly scaled input is expected.
- Freeze captures a spectrum, not a time-domain loop. Noise/transients can
  become tonal or phasey, and phase estimates in weak bins may be unstable.
  This deliberately omits bin shifting, stereo crossmix, phase blur, and dynamic
  FFT sizes from the source machine. It is not a transparent general time stretcher.

## Provenance and verification

MIT-licensed `fog-bank/src/SpectralFrame.h` and `SpectralFrame.cpp` supplied the
radix-2 FFT, windowed analysis/synthesis structure, overlap weights, magnitude
retention, and phase-advance ideas. Copyright (c) 2025 Ben Severns is retained
in the headers and repository MIT license. This adaptation separates the FFT,
framing, and hold into individual headers, uses compile-time mono storage and
periodic square-root Hann windows, and provides explicit capture/release.
It omits the source's stereo blur, trim/warmup, continuous hold blend, and scan.

`tests/spectral.cpp` checks known FFT bins, complex forward/inverse roundtrip,
reconstruction at three frame/hop settings including startup and drained tail,
impulse latency, bin-centered/off-bin held tones, independence from live input,
decay, release, reset, and invalid settings. These are native algorithm checks,
not a claim of measured real-time performance on a board.
