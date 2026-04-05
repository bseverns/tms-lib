# Source Map
This repo now includes several header-only exemplars adapted from other repos.

| tms-lib target | Source repo | Source file(s) | Notes |
| --- | --- | --- | --- |
| `lib/tms/control/BlockParam.h` | `tide-engine` | `core/ParamSmoother.h`, `core/ParamSmoother.cpp` | Linear block ramp smoother. |
| `lib/tms/core/XorShift32.h` | `seedbox` | `src/util/RNG.h` | Tiny deterministic pseudo-random generator. |
| `lib/tms/control/OnePoleLag.h` | `seedbox` | `src/util/Smoother.h` | Single-pole control lag block. |
| `lib/tms/control/SlewLimiter.h` | `seedbox` | `src/util/Smoother.h` | Separate rise/fall slew limiting. |
| `lib/tms/control/ScaleQuantizer.h` | `seedbox` | `include/util/ScaleQuantizer.h`, `src/util/ScaleQuantizer.cpp` | Snap semitone offsets to a scale. |
| `lib/tms/control/ScaleMap.h` | `silt` | `src/ScaleMap.h`, `src/ScaleMap.cpp` | Ratio tables for alternate tuning systems. |
| `lib/tms/control/EuclideanPattern.h` | `seedbox` | `src/engine/EuclidEngine.cpp` | Build and step Euclidean gate masks. |
| `lib/tms/control/TempoTransport.h` | `tide-engine`, `DiceLoop` | `core/TempoTransport.h`, `core/TempoTransport.cpp`, `src/tempo_sync.cpp` | Internal/tap/MIDI tempo handoff. |
| `lib/tms/control/MidiClockTransport.h` | `lofi-sampler` | `firmware/platformio/src/ClockTransport.h`, `firmware/platformio/src/ClockTransport.cpp` | MIDI realtime step transport with swing. |
| `lib/tms/control/ClockPulseTransport.h` | `rhythm-weave` | `lib/RhythmWeaveCore/src/RhythmWeaveMidiClock.h`, `lib/RhythmWeaveCore/src/RhythmWeaveMidiClock.cpp` | Internal/external clock pulse transport. |
| `lib/tms/control/OrbitPath.h` | `orbit-looper` | `src/PathGen.h`, `src/PathGen.cpp` | Circular, elliptical, and figure-8 motion paths. |
| `lib/tms/control/SyncUtils.h` | `orbit-looper` | `src/SyncUtils.h`, `src/SyncUtils.cpp` | Loop-length conversion and quantization helpers. |
| `lib/tms/dsp/XFeedMatrix.h` | `tide-engine` | `core/XFeedMatrix.h`, `core/XFeedMatrix.cpp` | Stereo crossfeed matrix. |
| `lib/tms/dsp/WowFlutter.h` | `tide-engine` | `core/WowFlutter.h`, `core/WowFlutter.cpp` | Delay-time modulation block. |
| `lib/tms/dsp/MSMatrix.h` | `horizon` | `src/MSMatrix.h` | Mid/side encode and decode helper. |
| `lib/tms/dsp/DynamicWidth.h` | `horizon` | `src/DynWidth.h`, `src/DynWidth.cpp` | Transient-aware stereo width shaping. |
| `lib/tms/control/EnvelopeFollower.h` | `dust-press` | `src/EnvelopeFollower.h`, `src/EnvelopeFollower.cpp` | Attack/release contour follower. |
| `lib/tms/control/TransientDetector.h` | `horizon` | `src/TransientDetector.h`, `src/TransientDetector.cpp` | Hit/transient activity detector. |
| `lib/tms/dsp/DriveCurves.h` | `dust-press` | `src/CurveBank.h`, `src/CurveBank.cpp` | Biasable drive curves with optional crackle. |
| `lib/tms/dsp/AirEQ.h` | `horizon` | `src/AirEQ.h`, `src/AirEQ.cpp` | One-pole air-band EQ. |
| `lib/tms/dsp/PresenceKeeper.h` | `fog-bank` | `src/PresenceKeeper.h`, `src/PresenceKeeper.cpp` | Adaptive upper-mid recovery for wet signals. |
| `lib/tms/dsp/PhaseBlur.h` | `fog-bank` | `src/PhaseBlur.h`, `src/PhaseBlur.cpp` | Smooth random offsets and crossmix for buffer clouds. |
| `lib/tms/dsp/AirLoss.h` | `orbit-looper` | `src/AirLoss.h`, `src/AirLoss.cpp` | Distance-based darkening and gain trim. |
| `lib/tms/dsp/Doppler.h` | `orbit-looper` | `src/Doppler.h`, `src/Doppler.cpp` | Fractional stereo sample reader. |
| `lib/tms/dsp/CombBank.h` | `silt` | `src/CombBank.h`, `src/CombBank.cpp` | Tuned feedback comb resonator bank. |
| `lib/tms/dsp/Plate.h` | `silt` | `src/Plate.h`, `src/Plate.cpp` | Compact four-line stereo plate wash. |
| `lib/tms/dsp/BitCrusher.h` | `DiceLoop` | `src/audio_pipeline.cpp` | Quantization reduction kernel. |
| `lib/tms/dsp/WaveFolder.h` | `DiceLoop` | `src/audio_pipeline.cpp` | Sine-based fold with memory smear. |
| `lib/tms/dsp/StutterGate.h` | `DiceLoop` | `src/audio_pipeline.cpp` | Probability/tempo sample-hold glitcher. |
| `lib/tms/dsp/TremoloMotion.h` | `DiceLoop` | `src/audio_pipeline.cpp` | Lightweight motion gain stage. |

## Notes
- `silt` was inspected earlier in the project when `Plate`, `CombBank`, and
  `ScaleMap` were still stubs. They are now mature enough to be cataloged.
