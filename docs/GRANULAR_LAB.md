# Fourth expansion: grains into a cloud

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_granular build/granular.wav
```

The eight-second render uses one synthetic source and four two-second settings:

| Time | Change | Listen for |
| --- | --- | --- |
| 0–2 s | 100 ms grains every 200 ms | Separate windowed snippets |
| 2–4 s | Same grains about every 25 ms | Overlap creates a continuous texture |
| 4–6 s | Add seeded source-position spray | Different snippets vary the texture |
| 6–8 s | Reverse at rate -0.75, keeping spray | Reversed motion and lower pitch |

The source combines a swept sine and a steady tone, rather than depending on
external files. Try changing grain duration, spacing, spray, or rate one at a
time. A grain's length stays fixed in output samples even when rate changes.

## Three small mechanisms

1. [Window.h](../lib/tms/dsp/Window.h) supplies a symmetric Hann window:
   `0.5 - 0.5*cos(2*pi*index/(length-1))`. Its first and last samples are zero.
   This is a finite grain envelope, not a periodic FFT window.
2. [GrainVoice.h](../lib/tms/sample/GrainVoice.h) reads a `SampleView`, moves its
   fractional read head, and multiplies the sample by the window and gain.
3. [GrainCloud.h](../lib/tms/sample/GrainCloud.h) schedules grains at fixed
   sample intervals, draws seeded position offsets, and sums a fixed voice pool.

```cpp
// Source must remain alive and immutable while grains use it.
tms::GrainVoice grain;
grain.trigger(view, 1000, 0.5, 4410, 0.2f); // start, rate, duration, gain
float sample = grain.tick();                 // one output sample
```

A single voice is useful on its own: trigger it from a rhythm, MIDI event, or
button. The cloud adds periodic scheduling without hiding the voice mechanism.

## Units, state, and boundaries

| Setting | Units / behavior |
| --- | --- |
| Start position | Source sample index, fractional allowed |
| Rate | Source samples per output sample; signed for reverse; zero freezes a source position |
| Duration | Output samples, at least three; Hann spans exactly this many ticks |
| Interval | Output samples between scheduled cloud events, at least one |
| Spray | Maximum absolute random source-position offset, in source samples |
| Gain | Per-grain gain clamped to [0,1]; cloud sum remains unclipped |

For mismatched sample rates, use `rate = sourceRate / outputRate * pitchRatio`.
The headers do not infer or convert those rates. Zero rate produces a windowed
constant sample, which can contain DC; it is primarily an explanatory setting.

A voice wraps source reads by default, including seam interpolation. Set
`loopSource=false` to zero-pad outside the source's valid index range. Grains
continue their window for the requested duration even when source samples are
out of range. Source seams inside a grain can still click: the Hann window
softens the grain boundaries, not every discontinuity in its source.

Trigger snapshots source and settings and replaces any ongoing voice. Invalid
trigger arguments stop the voice and return false. `stop()` silences immediately.
Buffers are non-owning; no source copying or allocation occurs. Finite audio is
expected. Configure/trigger/tick in the same processing context or synchronize
externally. Public state is exposed for teaching; use the methods to preserve
these contracts.

Cloud `start()` clears old voices/counters and seeds its PRNG. The first event
occurs on the first tick, then every interval. It takes the first idle voice;
when full it drops the new event and increments `dropped`, without stealing.
A random draw occurs for every scheduled event, including drops. `stop()` clears
voices immediately rather than allowing tails to finish. Pool size is a template
argument; output is summed without normalization or a limiter. Each voice keeps
scalar state, a source view, and no audio buffer. Approximate peak overlap is
`ceil(duration / interval)`; choose pool size and gain with that in mind.

This is a periodic scheduler with position spray, not a full granular engine.
It omits timing jitter, stereo panning, live recording, storage, voice stealing,
and time-stretch reconstruction. Trig windows and linear interpolation keep the
math readable; hardware CPU cost and resampling quality need separate evaluation.
The mono WAV example has conservative gains; it is not a hardware timing test.

## Provenance

These are original implementations of standard Hann/windowed-sample mechanisms,
informed by MIT-licensed `seedBox/src/engine/Granular.h` and `Granular.cpp`,
especially `allocateVoice`, `planGrain`, seeded planning, and the separation
between a grain plan and hardware graph. The source's `renderAudio` is empty;
these headers do not claim to reproduce that renderer or its sound.

The source uses oldest-voice stealing and timing spray. This teaching scheduler
instead drops new events at capacity and applies source-position spray. It uses
`SampleView` and `XorShift32` already present in tms-lib. No SeedBox source code
was copied into these new headers. The existing PRNG retains its separate source
attribution. `seedBox/tests/test_patterns/test_granular.cpp` was inspected for
context; native tests here exercise the new sample renderer itself.

`tests/granular.cpp` checks Hann endpoints/symmetry, duration, forward/reverse
and fractional reads, frozen positions, source bounds, large rates, invalid
settings, repeatable seeds, launch timing, pool bounds, dropped events, and stop.
