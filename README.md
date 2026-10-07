# tms-lib

A shelf of jarred specimens from machines I've made.

A delay line from one box. A rhythm rule from another. A little mechanism for
making a sound wander, break apart, or hang in the air. `tms-lib` collects those
pieces in small C++ headers so they can be opened up, taught, changed, and put
into something else.

The machines were where these ideas got worked out. This is where I keep the
parts I want to look at again—and share without asking someone to learn an
entire instrument's firmware first.

Some specimens are simplified extractions. Others are fresh explanations of a
mechanism that appeared in an older project. The labels matter: the
[source map](docs/SOURCE_MAP.md) records where an idea came from and what changed
on its way here.

## Why keep the jars?

For teaching, mostly. Also for remembering what I was thinking when I built a
particular machine, and finding connections between projects that started in
different places.

A small header gives us something we can read together. We can find the state,
follow the math, change one number, and listen to what happens. An envelope can
become a meter, a pitch gesture, or the edge of a grain. A moving read head can
become a sampler, a looper, or a source passing by the listener.

The useful lesson is often in that change of context. Take a specimen off the
shelf, understand one behavior, then see what else it can do.

## What's on the shelf

The collection draws from delay boxes, resonator gardens, samplers, loopers,
stereo processors, spatial experiments, and rhythm machines. You'll find:

- clocks, gates, Euclidean rhythms, and pitch constraints
- smoothing, envelopes, transient detection, and modulation
- filters, saturation, combs, and small reverb structures
- stereo width, crossfeed, paths, and delay-time motion
- oscillators, sample slices, looping read heads, and grains
- short impulse responses, frequency shifting, and spectral hold

There are familiar textbook mechanisms here alongside choices that came out of
building particular instruments. Both are useful to keep around.

Browse the [header catalog](docs/CATALOG.md), follow an idea back through the
[source map](docs/SOURCE_MAP.md), or start with an experiment below.

## Open a jar on your laptop

You can begin with a compiler and a pair of headphones. The native examples
print values or write WAV files; no board is needed.

You'll need a C++17 compiler and CMake 3.16 or newer.

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_patch
```

That last command prints an impulse, its filtered response, and an envelope as
CSV. Change the cutoff or follower times, rebuild, and compare the numbers.

For something to listen to:

```sh
./build/tms_drum build/drum_voice.wav
```

Open the WAV in an audio player. Then change the pitch envelope, remove the
noise, or stretch the decay. The [synthesis lab](docs/SYNTHESIS_LAB.md) walks
through the pieces.

There are several other jars to open. Each lab explains the mechanism, suggests
changes, and names the assumptions behind the example.

| Experiment | Run after building | Notes |
| --- | --- | --- |
| A small drum voice | `./build/tms_drum build/drum_voice.wav` | [Oscillators, envelopes, pitch, and noise](docs/SYNTHESIS_LAB.md) |
| Sample playback | `./build/tms_sample build/sample_playback.wav` | [Slices, speed, reverse, and loop seams](docs/SAMPLE_LAB.md) |
| Short convolution | `./build/tms_convolution build/convolution.wav` | [An impulse response as a filter or a little space](docs/CONVOLUTION_LAB.md) |
| Grains into a cloud | `./build/tms_granular build/granular.wav` | [Windows, overlap, and seeded position spray](docs/GRANULAR_LAB.md) |
| Frequency shifting | `./build/tms_frequency_shift build/frequency_shift.wav` | [Moving partials by a fixed number of hertz](docs/FREQUENCY_SHIFT_LAB.md) |
| Spectral hold | `./build/tms_spectral build/spectral_hold.wav` | [Frames, overlap-add, capture, and release](docs/SPECTRAL_LAB.md) |

## Bring a specimen to the bench

The serial sketches are a good first stop on Teensy 4.0 or 4.1. A board and USB
cable are enough to watch a parameter chase its target, step a rhythm, or follow
an orbit. Add a pot, button, or LED when you want to connect those behaviors to
your hands.

With PlatformIO installed:

```sh
pio run -e teensy41
pio run -e teensy41 -t upload
pio device monitor -b 115200
```

`teensy41` and `teensy40` run the parameter demo. `euclid`, `stereo`, and `orbit`
select the other serial demos on Teensy 4.1.

| Sketch | Idea |
| --- | --- |
| [param_demo.ino](examples/param_demo.ino) | Smoothing and target chasing |
| [euclid_quantizer_demo.ino](examples/euclid_quantizer_demo.ino) | Rhythm and pitch constraints |
| [stereo_image_demo.ino](examples/stereo_image_demo.ino) | Mid/side, width, and crossfeed |
| [orbit_motion_demo.ino](examples/orbit_motion_demo.ino) | Geometry becoming modulation |

For real audio, the [Teensy audio lab](docs/AUDIO_LAB.md) provides a shared bench:
Teensy 4.x, an SGTL5000 Audio Shield, and seven selectable patches. Build with
`pio run -e audio_lab41` or `audio_lab40`. You can listen, feed in line audio,
and measure callback time and memory use as you change patches. Board
measurements still need to be collected; the lab includes a procedure and a
[results sheet](docs/AUDIO_LAB_RESULTS.csv).

The [teaching guide](docs/TEACHING_GUIDE.md) has classroom routes, equipment
suggestions, and ideas for combining the blocks.

## How the specimens are kept

The library is header-only so most mechanisms can be read in one place. Include
just the pieces you need:

```cpp
#include "tms/dsp/Biquad.h"
```

The compiler's include path should contain `lib`. CMake projects can link
`tms::tms`; the [contribution notes](CONTRIBUTING.md) explain integration. For
Arduino IDE, put one sketch in a folder matching its filename and supply `lib`
as an include path. The PlatformIO setup already handles that path.

```text
lib/tms/
├── core/       math, buffers, phase, and randomness
├── control/    parameters, envelopes, clocks, and musical rules
├── dsp/        filters, tone, motion, stereo, and windows
├── sample/     sample views, playback, and grains
├── spectral/   FFT, framing, overlap-add, and hold
├── io/         tap tempo and MIDI helpers
├── interop/    transport and trigger messages
├── presets/    a persistence interface
├── ui/         small display helpers
└── safety/     output guardrails

examples/       things to run, hear, and change
docs/           labels, origins, lab notes, and directions to explore
```

The code aims to keep the math and state visible, use fixed storage, and avoid
allocation in the audio path. Native checks help keep the specimens usable as
we change them. Each lab explains its units, timing, and simplifications;
[the current limitations](docs/ROADMAP.md#current-limitations) include unfinished
exercises and memory assumptions worth reading before building a larger patch.

## Room for more jars

This collection will expand as the ideas shift. An older machine might give up
another useful piece. A lesson might need a clearer version of something already
here. Two specimens might suggest a new experiment neither source project had.

The [roadmap](docs/ROADMAP.md) keeps those possibilities visible. It can change
with the work. Coverage grows through ideas worth teaching and revisiting.

If you're adding something, bring its label along: where it came from, what was
simplified, and a small experiment that makes its behavior observable. Keep the
header readable enough that someone can follow it and start changing it.
[CONTRIBUTING.md](CONTRIBUTING.md) has the practical details.

MIT licensed; see [LICENSE](LICENSE).
