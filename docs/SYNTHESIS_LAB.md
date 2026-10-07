# First expansion: generate a drum voice

Build and listen on your laptop:

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_drum build/drum_voice.wav
```

Open the generated WAV in an audio player. It contains eight identical hits,
spaced half a second apart. No audio device or external packages are needed.

Read `core/PhaseAccumulator.h`, `dsp/SineOscillator.h`, then `control/ADSR.h`.
The composition is in `examples/native/drum_voice.cpp`:

1. A phase accumulator advances by frequency / sample rate each sample.
2. A sine converts phase into the body of the sound.
3. A short envelope moves pitch from 195 Hz toward 55 Hz.
4. A second envelope shapes amplitude; a little seeded noise adds attack.

Try changing the pitch decay from 45 ms to 150 ms, removing the noise, or
lengthening the amplitude decay. Change one thing, rebuild, and listen again.
The voice is a teaching patch inspired by the oscillator/noise/envelope layering
in `rhythm-weave`, not a reproduction of its full percussion renderer.

## Contracts

| Block | Units and cadence | Reset / transitions |
| --- | --- | --- |
| PhaseAccumulator | Hz, sample rate in Hz, phase in cycles; one tick per sample | Reset wraps phase into [0,1); tick returns phase before advancing; signed rates allowed |
| SineOscillator | Output [-1,1]; one tick or quadrature tick per sample | Reset forwards phase; quadrature returns sine and cosine at the same phase |
| ADSR | Attack/decay/release in ms, sustain/output [0,1]; one tick per sample | Reset silences; noteOn retriggers from current level; noteOff releases from current level |

ADSR uses linear segments with explicit endpoints. Positive times round up to
whole samples; zero-time stages skip on the next tick. A timed segment consumes
one sample before its successor runs. Configuration changes take effect when a
new segment starts; a sample-rate change does not rescale an ongoing segment.
Times clamp to 0–60,000 ms. Invalid times become zero; invalid sustain becomes
zero. Sample rates must be finite and positive (ADSR also caps at 768 kHz).

All three headers keep scalar state and allocate no buffers or heap memory.
Oscillator rate setters and ADSR triggers/configuration should run in the same
processing context as tick, or be externally synchronized. The pitch envelope
in this example deliberately updates oscillator frequency each sample.

The sine implementation uses trig functions for readability. It is not a
bandlimited waveform bank and does not guarantee alias-free output above
Nyquist. The example writes PCM16; this writer is an example utility rather
than a general audio-file API. WAV output is deterministic from the seeded
noise on a given platform, but floating-point implementations may differ.

## Provenance

These headers are original implementations of standard synthesis mechanisms.
`FieldKitFX/engine/oscillator.h`, `oscillator.c`, and `adsr.h` were inspected as
conceptual references. That project carries GPL-3.0; its implementation and
hardware code were not imported. The example references the patch structure
in MIT-licensed `rhythm-weave/lib/RhythmWeaveCore/src/RhythmWeaveVoiceModel.cpp`;
its renderer and voice parameter tables were not copied.
