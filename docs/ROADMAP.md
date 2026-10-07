# Scope and next adaptations

`tms-lib` mirrors reusable mechanisms from the DSP machines as readable,
portable headers. It preserves concepts and useful interfaces, rather than
promising identical sound, timing, or feature coverage. The source firmware is
still the reference for full instruments.

The current catalog covers timing, smoothing, rhythm/pitch, delay modulation,
drive, stereo finishing, spatial helpers, and compact resonators. See
[SOURCE_MAP.md](SOURCE_MAP.md) for known origins and [CATALOG.md](CATALOG.md)
for every current header.

## Priorities

| Order | Work | Neighbor reference | Completion evidence |
| --- | --- | --- | --- |
| 1 | Give each block a short units/cadence/reset contract; expand boundary checks | Existing headers and corresponding source tests | Every catalog entry has a documented contract and appropriate behavior check |
| 2 | Extract a true lookahead limiter alongside the clipping exercise | `silt/src/LimiterLookahead.*`, `orbit-looper/src/LimiterLookahead.*` | Documented latency; impulse, ceiling, attack/release tests |
| 3 | Add a minimal record/playback loop with fractional reading and seam crossfade | `orbit-looper/src/OrbitLooper.*` | Wrap/seam tests and a small fixed-buffer example |
| 4 | Implemented: Hann window, single grain voice, fixed-pool scheduler | `seedBox/src/engine/Granular.*` | Window endpoints, bounded reads, deterministic render |
| 5 | Add delay/feedback composition and an actual audio lab | `tide-engine` | Repeatable impulse response and measured board memory |
| 6 | Audit remaining machine families for distinct mechanisms | `tape-harvester`, `rhythm-weave`, `lofi-sampler`, `fog-bank` | Verified source paths, licensing, and an adaptation note per addition |

These are candidate adaptations, not claims that all source files have been
fully audited or that unimplemented blocks already exist here.

## Current limitations

- `LimiterLookahead` clips immediately. It has no lookahead buffer, envelope,
  latency, or release behavior; `releaseMs` is currently metadata.
- `PresetStore` is an interface, not persistence. `ModMatrix` stores routes,
  rather than evaluating them. `MIDIMap` and `LEDBar` are small data helpers.
- `RingBuffer` requires external synchronization across threads/interrupts;
  `volatile` is not a C++ concurrency guarantee.
- Default timing is 44.1 kHz / 128 samples. Some blocks allow a sample-rate
  setter; others use these constants. Do not assume universal rate conversion.
- `DelayLine` stores 88,200 floats (352,800 bytes before counters). `CombBank`
  uses 65,536 bytes for delay samples; `Plate` uses 9,216. Place large instances
  in static/global storage and budget the whole patch before using smaller MCUs.
- Serial sketches visualize simulated signals; they do not supply codec I/O
  or prove audio callback performance. Native tests cover selected behaviors,
  not every block or parameter combination.

## Stepwise expansion progress

1. **Sound generation and envelopes — implemented:** original phase accumulator,
   sine/quadrature oscillator, linear ADSR, synthesis tests, and a WAV-rendering
   drum lab. See [SYNTHESIS_LAB.md](SYNTHESIS_LAB.md).
2. **Sample playback — implemented:** non-owning fractional views, slice boundaries,
   one-shot fades, bidirectional seam crossfades, native checks, and a WAV lab.
   See [SAMPLE_LAB.md](SAMPLE_LAB.md).
3. **Short FIR convolution — implemented:** fixed-capacity mono FIR adapted
   from `ir-postcards`, streaming/reference tests, and a four-comparison WAV lab.
   See [CONVOLUTION_LAB.md](CONVOLUTION_LAB.md).
4. **Granular synthesis — implemented:** symmetric Hann window, single-grain
   renderer, deterministic periodic cloud, voice-capacity checks, and a WAV lab.
   See [GRANULAR_LAB.md](GRANULAR_LAB.md).
5. **Frequency shifting — implemented:** original windowed FIR Hilbert pair,
   signed quadrature modulation, frequency/sideband checks, and a WAV lab. See
   [FREQUENCY_SHIFT_LAB.md](FREQUENCY_SHIFT_LAB.md).
6. **Spectral processing — implemented:** radix-2 FFT, windowed framing,
   weighted overlap-add, and mono spectral hold informed by `fog-bank`. Native
   reconstruction/freeze checks and a WAV comparison are in place. See
   [SPECTRAL_LAB.md](SPECTRAL_LAB.md).

The six-step expansion sequence is complete. Remaining candidates above include
true lookahead dynamics, record/overdub loops, feedback composition, and actual
board audio labs with measured callback and memory budgets.

## Hardware measurement lab

A Teensy 4.0/4.1 SGTL5000 audio bench now exercises the portable blocks and
reports steady/transition callback peaks, graph CPU, Audio pool usage/failures,
and compiled state sizes. Both board environments compile; the portable patch
is native-tested. Actual board measurements are pending because no Teensy was
attached. Follow [AUDIO_LAB.md](AUDIO_LAB.md) and retain the raw CSV results.
