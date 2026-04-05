# Teaching Guide
- This repo is intentionally header-only and classroom-friendly: each block is
  small enough to read in one sitting and safe enough to drop into a sketch.
- Several newer exemplars were adapted from your sibling repos. See
  `docs/SOURCE_MAP.md` for provenance.

## Expected setup
- A `Teensy 4.0` or `Teensy 4.1` is the best fit for classroom use here.
  Teensy 4.x has enough CPU headroom to let students experiment without every
  patch turning into a performance-debugging lesson.
- A solderless breadboard, jumper wires, and a few potentiometers are enough
  for the control-side labs.
- Strongly recommended:
  `3x to 6x 10k potentiometers`, `momentary pushbuttons`, `LEDs with resistors`,
  a small speaker or headphones path, and a USB serial monitor.
- If you want real audio I/O instead of serial-only sketches, add a Teensy
  audio shield or another known-good codec board.
- For faster classroom setup, keep one "known working" rig prewired:
  Teensy 4.x + breadboard + one pot + one button + one LED + USB serial.

## Software assumptions
- Arduino IDE with Teensyduino, or PlatformIO if your class is already
  comfortable there.
- Serial Monitor at `115200` baud for the example sketches in this repo.
- Students should be able to edit one `.ino` sketch, upload it, and observe
  printed values before they touch any larger audio patch.

## Teaching posture
- Start with serial output before audio output. Students learn the state
  machine faster when they can see numbers change.
- Move from `one variable -> one block -> one behavior`.
- Only patch two or three blocks together per exercise. Past that point the
  lesson tends to become debugging instead of DSP.
- Keep one oscilloscope or logic-analyzer style visualization nearby if
  possible, but do not make it a requirement for the first labs.

## Suggested reading order
1. `core/Types.h`
2. `core/DelayLine.h`, `core/XorShift32.h`
3. `control/Param.h`, `control/BlockParam.h`, `control/EnvelopeFollower.h`,
   `control/OnePoleLag.h`, `control/SlewLimiter.h`,
   `control/TransientDetector.h`
4. `dsp/Biquad.h`, `dsp/TiltEQ.h`, `dsp/Allpass.h`, `dsp/AirEQ.h`,
   `dsp/AirLoss.h`
5. `dsp/BitCrusher.h`, `dsp/WaveFolder.h`, `dsp/StutterGate.h`,
   `dsp/TremoloMotion.h`, `dsp/DriveCurves.h`
6. `dsp/MSMatrix.h`, `dsp/XFeedMatrix.h`, `dsp/DynamicWidth.h`,
   `dsp/WowFlutter.h`, `dsp/PresenceKeeper.h`, `dsp/PhaseBlur.h`,
   `dsp/Doppler.h`
7. `control/ClockEngine.h`, `control/TempoTransport.h`,
   `control/MidiClockTransport.h`, `control/ClockPulseTransport.h`,
   `control/OrbitPath.h`, `control/EuclideanPattern.h`,
   `control/ScaleQuantizer.h`
8. `dsp/LimiterLookahead.h` as the "finish this in lab" example

## Example path
- `examples/param_demo.ino`: smoothing, target chasing, and zipper-noise talk.
- `examples/euclid_quantizer_demo.ino`: deterministic rhythm plus pitch
  quantization.
- `examples/stereo_image_demo.ino`: mid/side, dynamic width, and crossfeed.
- `examples/orbit_motion_demo.ino`: geometry as modulation, plus air loss and
  wow/flutter.

## Hardware progression
1. Laptop + Teensy + USB serial only.
2. Add one potentiometer and map it to a `Param`, `OnePoleLag`, or
   `SlewLimiter`.
3. Add one button and use it to reseed `XorShift32` or rotate a Euclidean
   pattern.
4. Add one LED and blink it from a gate, envelope threshold, or clock pulse.
5. Only then move to audio output or an audio shield.

## Good compare/contrast lessons
- `control/Param.h` versus `control/BlockParam.h`: exponential smoothing
  against linear block ramps.
- `control/EnvelopeFollower.h`: the missing control bridge between "audio
  signal" and "modulation source."
- `control/TransientDetector.h`: a more opinionated follower that answers
  “was that a hit?” instead of just “how loud is it?”
- `control/OnePoleLag.h` versus `control/SlewLimiter.h`: asymptotic smoothing
  against rate-limited motion.
- `core/XorShift32.h`: deterministic randomness for repeatable musical systems.
- `dsp/Biquad.h` versus `dsp/WowFlutter.h`: static coefficient design against
  time-varying modulation.
- `dsp/AirEQ.h` versus `dsp/AirLoss.h`: both shape brightness, but one is a
  creative EQ and the other is a distance model.
- `dsp/MSMatrix.h` + `dsp/DynamicWidth.h`: stereo width as M/S math rather than
  mystery.
- `dsp/XFeedMatrix.h`: the smallest useful stereo matrix.
- `control/TempoTransport.h` versus `control/MidiClockTransport.h`: estimate a
  tempo first, then step a sequencer from clock bytes.
- `control/ClockPulseTransport.h`: internal-clock scheduling versus external
  pulse following.
- `control/OrbitPath.h`: turn geometry into modulation.
- `control/EuclideanPattern.h`: derive distributed rhythm from integer math.
- `control/ScaleQuantizer.h`: connect raw control voltages or note offsets to
  harmonic constraints.
- `dsp/BitCrusher.h` + `dsp/WaveFolder.h` + `dsp/StutterGate.h`: three very
  different ways to add "dirt" with tiny code footprints.
- `dsp/DriveCurves.h`: compare four nonlinear transfer functions with the same
  test tone.
- `dsp/AirLoss.h` + `dsp/PresenceKeeper.h`: one block removes high-end as a
  source recedes, the other selectively restores intelligibility.
- `dsp/PhaseBlur.h` + `dsp/Doppler.h`: two different ways to make motion feel
  spatial, one by wandering offsets and one by moving the read head itself.

## Equipment-aware lab ideas
- Breadboard one potentiometer and print raw ADC versus smoothed value using
  `Param`, `OnePoleLag`, and `SlewLimiter`.
- Drive an LED from `EnvelopeFollower` to show the difference between attack
  and release time.
- Use a pushbutton to step `EuclideanPattern` rotation and watch the gate
  pattern change over serial.
- Put two pots on a Teensy audio rig and map them to `DynamicWidth` and
  `XFeedMatrix` so students can hear stereo image changes directly.
- Feed a drum loop or click track into `TransientDetector` and print the
  activity value to tune sensitivity by ear and by meter.

## Lab ideas
- Extend `dsp/LimiterLookahead.h` from clamp -> envelope follower ->
  lookahead limiter.
- Patch `dsp/WowFlutter.h` into `core/DelayLine.h` and listen for pitch motion.
- Build a one-knob dirt strip by chaining `BitCrusher`, `WaveFolder`,
  `StutterGate`, and `TremoloMotion`.
- Use `MSMatrix`, `DynamicWidth`, and `XFeedMatrix` to build a three-stage
  stereo image lab.
- Drive `OrbitPath` into pan, air-loss amount, or delay time for a spatial
  modulation study.
- Combine `XorShift32`, `EuclideanPattern`, and `ScaleQuantizer` into a small
  deterministic generative sequencer.
- Compare `AirEQ`, `AirLoss`, and `PresenceKeeper` on the same signal to
  discuss “brightness” versus “presence” versus “distance.”
- Use `PhaseBlur` and `Doppler` as two separate motion engines for a frozen
  sample exercise.

## Suggested classroom kits
- Minimal control kit:
  Teensy 4.x, breadboard, jumper wires, 2 pots, 1 button, 1 LED.
- Audio lab kit:
  Minimal control kit plus Teensy audio shield, headphones or powered speaker,
  and a safe line-level source.
- Group demo rig:
  One instructor-owned prewired board mirrored on a projector, plus student
  boards that only add one new component per lab.
