# tms-lib

`tms-lib` is a small, header-only DSP and control toolkit for teaching
embedded audio systems.

It is not a single instrument firmware. It is a curated library of the
building blocks that show up across a family of custom machines: delay boxes,
looper/orbit systems, stereo finishers, granular/generative sequencers,
rhythm engines, and performance control surfaces.

Some of those source machines are public, some are not. This repo deliberately
does not depend on linking back out to them. Instead, it gathers the most
teachable parts into one place, trims them down, and makes them readable in a
classroom setting.

## What This Repo Is For

This repo exists to answer a practical question:

How do you teach DSP, musical control systems, and embedded instrument design
without dropping students into a 20,000-line firmware codebase on day one?

The answer here is:

- use very small headers
- keep the blocks composable
- prefer serial-printable examples first
- move from one concept to one block to one machine

## Who It Is For

- students learning embedded DSP on `Teensy 4.x`
- artists teaching instrument design or physical computing
- developers who want compact reference implementations of common musical DSP
- anyone building custom boxes who wants “the useful middle” between toy code
  and full production firmware

## The Machines Behind It

The catalog in this repo comes from a broader ecosystem of machines and
experiments. The point is not to recreate any one of them exactly. The point is
to extract the recurring ideas.

Those machines generally fall into a few families:

- tape / delay / wow-flutter machines
- dirt / saturation / transient-reactive processors
- tuned comb / plate / resonator structures
- stereo imaging / width / finishing tools
- frozen-buffer / spectral / motion-based spatial tools
- sequencers, clocks, and generative pattern systems
- sampler / looper / pitch-quantized rhythm engines

In other words:

```text
            performance machines
                    |
    -----------------------------------------
    |           |            |              |
  timing      control       DSP         spatial motion
    |           |            |              |
  clocks     smoothing    filters       paths / blur
  gates      envelopes    dirt          doppler / air
  sync       quantizers   delay         stereo image
```

## Why Header-Only

Header-only is a deliberate teaching choice.

- Students can open one file and usually understand the whole block.
- There is very little build-system overhead.
- It is easy to paste a block into a sketch and modify it live.
- It keeps the focus on algorithm and state, not project plumbing.

This repo is not trying to model perfect industrial library design. It is
trying to maximize teachability.

## What You Need

The expected hardware/software setup is simple:

- `Teensy 4.0` or `Teensy 4.1` (these systems were designed around the Cortex M7 MCU's speed, but technically-speaking you could use an Uno if speed wasn't a factor)
- solderless breadboard
- jumper wires
- `2-6` potentiometers, buttons, and a few LEDs
- USB serial monitor
- optional audio shield or codec board for audio labs

The deeper classroom notes live in [TEACHING_GUIDE.md](/Users/bseverns/Documents/GitHub/tms-lib/docs/TEACHING_GUIDE.md).

## Repo Map

```text
tms-lib/
├── lib/tms/core       low-level utilities, buffers, timing constants, RNG
├── lib/tms/control    smoothing, transport, quantizers, pattern logic
├── lib/tms/dsp        filters, motion, dirt, stereo, air/presence tools
├── lib/tms/io         tap tempo and MIDI-related helpers
├── lib/tms/interop    small transport / trigger message types
├── lib/tms/ui         simple performance-facing display helpers
├── lib/tms/safety     guardrails for real-time systems
├── examples/          tiny classroom sketches
└── docs/              teaching notes and provenance
```

## The Main Teaching Idea

A useful musical machine is usually just a few layers stacked together:

```text
performer gesture
    ↓
control interpretation
    ↓
parameter shaping
    ↓
DSP block
    ↓
mix / output / interaction
```

Concrete examples in this repo:

- a potentiometer goes through `Param`, `OnePoleLag`, or `SlewLimiter`
- a hit detector goes through `EnvelopeFollower` or `TransientDetector`
- a rhythm idea goes through `EuclideanPattern` and `TempoTransport`
- a spatial idea goes through `OrbitPath`, `PhaseBlur`, `AirLoss`, or `Doppler`
- a tone idea goes through `Biquad`, `TiltEQ`, `AirEQ`, `DriveCurves`, or
  `PresenceKeeper`

## Catalog Highlights

If you are new to the repo, these are strong starting points:

- `core/Types.h`: basic constants and utility math
- `control/Param.h`: exponential smoothing
- `control/EnvelopeFollower.h`: audio-to-control bridge
- `control/EuclideanPattern.h`: rhythm from integer math
- `control/ScaleQuantizer.h`: harmony constraints from simple pitch math
- `control/ScaleMap.h`: alternate tuning tables for resonant systems
- `dsp/Biquad.h`: classic filter structure
- `dsp/DriveCurves.h`: nonlinear shaping in a compact form
- `dsp/DynamicWidth.h`: stereo width with transient awareness
- `dsp/CombBank.h` and `dsp/Plate.h`: compact resonator and wash structures
- `dsp/WowFlutter.h`: motion as delay-time modulation
- `dsp/PhaseBlur.h` and `dsp/Doppler.h`: two different spatial movement ideas

The full provenance map lives in [SOURCE_MAP.md](/Users/bseverns/Documents/GitHub/tms-lib/docs/SOURCE_MAP.md).

## Example Progression

The examples are designed to build confidence in small steps:

- [param_demo.ino](/Users/bseverns/Documents/GitHub/tms-lib/examples/param_demo.ino)
  smoothing and target chasing
- [euclid_quantizer_demo.ino](/Users/bseverns/Documents/GitHub/tms-lib/examples/euclid_quantizer_demo.ino)
  rhythm plus pitch quantization
- [stereo_image_demo.ino](/Users/bseverns/Documents/GitHub/tms-lib/examples/stereo_image_demo.ino)
  mid/side, width, and crossfeed
- [orbit_motion_demo.ino](/Users/bseverns/Documents/GitHub/tms-lib/examples/orbit_motion_demo.ino)
  geometry as modulation

Typical classroom flow:

```text
serial numbers
   ↓
one control block
   ↓
one DSP block
   ↓
two-block patch
   ↓
small instrument sketch
   ↓
full custom machine
```

## Design Principles

- keep hot-path code obvious
- avoid allocation on the audio thread
- prefer small stateful structs over heavy abstractions
- make the useful math visible
- use comments to explain intent, not restate syntax
- preserve enough realism that the code still feels like instrument firmware

## What This Repo Is Not

- not a complete audio framework
- not a monolithic synth or effects firmware
- not a vendor SDK replacement
- not a polished product library with every edge case covered

It is a teaching toolkit and a catalog of reusable musical mechanisms.

## Where To Start

If you want the shortest path:

1. Read [TEACHING_GUIDE.md](/Users/bseverns/Documents/GitHub/tms-lib/docs/TEACHING_GUIDE.md).
2. Open [Types.h](/Users/bseverns/Documents/GitHub/tms-lib/lib/tms/core/Types.h).
3. Run [param_demo.ino](/Users/bseverns/Documents/GitHub/tms-lib/examples/param_demo.ino).
4. Add one potentiometer and compare `Param`, `OnePoleLag`, and `SlewLimiter`.
5. Build outward from there.
