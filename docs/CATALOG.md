# Header catalog

All current headers compile independently in the native build. Behavioral
coverage is selective; see `tests/behavior.cpp` and `tests/synthesis.cpp` and `tests/sample.cpp` and `tests/fir.cpp` and `tests/granular.cpp` and `tests/frequency_shifter.cpp` and `tests/spectral.cpp`. Origins refer to the
[source map](SOURCE_MAP.md), not guaranteed firmware equivalence.

| Header | Origin / status |
| --- | --- |
| [control/BlockParam.h](../lib/tms/control/BlockParam.h) | Adaptation; see source map |
| [control/ChaosEngine.h](../lib/tms/control/ChaosEngine.h) | Local utility / teaching block |
| [control/ClockEngine.h](../lib/tms/control/ClockEngine.h) | Local utility / teaching block |
| [control/ClockPulseTransport.h](../lib/tms/control/ClockPulseTransport.h) | Adaptation; see source map |
| [control/EnvelopeFollower.h](../lib/tms/control/EnvelopeFollower.h) | Adaptation; see source map |
| [control/EuclideanPattern.h](../lib/tms/control/EuclideanPattern.h) | Adaptation; see source map |
| [control/MidiClockTransport.h](../lib/tms/control/MidiClockTransport.h) | Adaptation; see source map |
| [control/ModMatrix.h](../lib/tms/control/ModMatrix.h) | Local utility / teaching block |
| [control/OnePoleLag.h](../lib/tms/control/OnePoleLag.h) | Adaptation; see source map |
| [control/OrbitPath.h](../lib/tms/control/OrbitPath.h) | Adaptation; see source map |
| [control/Param.h](../lib/tms/control/Param.h) | Local utility / teaching block |
| [control/ScaleMap.h](../lib/tms/control/ScaleMap.h) | Adaptation; see source map |
| [control/ScaleQuantizer.h](../lib/tms/control/ScaleQuantizer.h) | Adaptation; see source map |
| [control/SlewLimiter.h](../lib/tms/control/SlewLimiter.h) | Adaptation; see source map |
| [control/SyncUtils.h](../lib/tms/control/SyncUtils.h) | Adaptation; see source map |
| [control/TempoTransport.h](../lib/tms/control/TempoTransport.h) | Adaptation; see source map |
| [control/TransientDetector.h](../lib/tms/control/TransientDetector.h) | Adaptation; see source map |
| [core/DelayLine.h](../lib/tms/core/DelayLine.h) | Local utility / teaching block |
| [core/ObjectPool.h](../lib/tms/core/ObjectPool.h) | Local utility / teaching block |
| [core/Profiler.h](../lib/tms/core/Profiler.h) | Local utility / teaching block |
| [core/RingBuffer.h](../lib/tms/core/RingBuffer.h) | Local utility / teaching block |
| [core/Types.h](../lib/tms/core/Types.h) | Local utility / teaching block |
| [core/XorShift32.h](../lib/tms/core/XorShift32.h) | Adaptation; see source map |
| [dsp/AirEQ.h](../lib/tms/dsp/AirEQ.h) | Adaptation; see source map |
| [dsp/AirLoss.h](../lib/tms/dsp/AirLoss.h) | Adaptation; see source map |
| [dsp/Allpass.h](../lib/tms/dsp/Allpass.h) | Local utility / teaching block |
| [dsp/Biquad.h](../lib/tms/dsp/Biquad.h) | Local utility / teaching block |
| [dsp/BitCrusher.h](../lib/tms/dsp/BitCrusher.h) | Adaptation; see source map |
| [dsp/CombBank.h](../lib/tms/dsp/CombBank.h) | Adaptation; see source map |
| [dsp/Doppler.h](../lib/tms/dsp/Doppler.h) | Adaptation; see source map |
| [dsp/DriveCurves.h](../lib/tms/dsp/DriveCurves.h) | Adaptation; see source map |
| [dsp/DynamicWidth.h](../lib/tms/dsp/DynamicWidth.h) | Adaptation; see source map |
| [dsp/LimiterLookahead.h](../lib/tms/dsp/LimiterLookahead.h) | Exercise: hard clipper; lookahead unimplemented |
| [dsp/MSMatrix.h](../lib/tms/dsp/MSMatrix.h) | Adaptation; see source map |
| [dsp/PhaseBlur.h](../lib/tms/dsp/PhaseBlur.h) | Adaptation; see source map |
| [dsp/Plate.h](../lib/tms/dsp/Plate.h) | Adaptation; see source map |
| [dsp/PresenceKeeper.h](../lib/tms/dsp/PresenceKeeper.h) | Adaptation; see source map |
| [dsp/SoftSaturation.h](../lib/tms/dsp/SoftSaturation.h) | Local utility / teaching block |
| [dsp/StutterGate.h](../lib/tms/dsp/StutterGate.h) | Adaptation; see source map |
| [dsp/TiltEQ.h](../lib/tms/dsp/TiltEQ.h) | Local utility / teaching block |
| [dsp/TremoloMotion.h](../lib/tms/dsp/TremoloMotion.h) | Adaptation; see source map |
| [dsp/WaveFolder.h](../lib/tms/dsp/WaveFolder.h) | Adaptation; see source map |
| [dsp/WowFlutter.h](../lib/tms/dsp/WowFlutter.h) | Adaptation; see source map |
| [dsp/XFeedMatrix.h](../lib/tms/dsp/XFeedMatrix.h) | Adaptation; see source map |
| [interop/Transport.h](../lib/tms/interop/Transport.h) | Local utility / teaching block |
| [interop/TriggerBus.h](../lib/tms/interop/TriggerBus.h) | Local utility / teaching block |
| [io/MIDIMap.h](../lib/tms/io/MIDIMap.h) | Local utility / teaching block |
| [io/TapTempo.h](../lib/tms/io/TapTempo.h) | Local utility / teaching block |
| [presets/PresetStore.h](../lib/tms/presets/PresetStore.h) | Local utility / teaching block |
| [safety/StabilityGuard.h](../lib/tms/safety/StabilityGuard.h) | Local utility / teaching block |
| [ui/LEDBar.h](../lib/tms/ui/LEDBar.h) | Local utility / teaching block |
| [core/PhaseAccumulator.h](../lib/tms/core/PhaseAccumulator.h) | Original synthesis block; see synthesis lab |
| [dsp/SineOscillator.h](../lib/tms/dsp/SineOscillator.h) | Original synthesis block; see synthesis lab |
| [control/ADSR.h](../lib/tms/control/ADSR.h) | Original synthesis block; see synthesis lab |
| [sample/SampleView.h](../lib/tms/sample/SampleView.h) | Original sample reader/slicing block; see sample lab |
| [sample/SamplePlayer.h](../lib/tms/sample/SamplePlayer.h) | Original playback/fade block; see sample lab |
| [dsp/FIR.h](../lib/tms/dsp/FIR.h) | Short mono FIR adapted from ir-postcards; see convolution lab |
| [dsp/Window.h](../lib/tms/dsp/Window.h) | Original symmetric Hann window; see granular lab |
| [sample/GrainVoice.h](../lib/tms/sample/GrainVoice.h) | Original grain renderer; seedBox conceptual reference |
| [sample/GrainCloud.h](../lib/tms/sample/GrainCloud.h) | Original fixed-pool periodic scheduler; see granular lab |
| [dsp/HilbertPair.h](../lib/tms/dsp/HilbertPair.h) | Original windowed FIR quadrature pair; see frequency-shifting lab |
| [dsp/FrequencyShifter.h](../lib/tms/dsp/FrequencyShifter.h) | Original signed single-sideband modulation; see frequency-shifting lab |
| [spectral/FFT.h](../lib/tms/spectral/FFT.h) | Radix-2 FFT adapted from fog-bank |
| [spectral/SpectralFrame.h](../lib/tms/spectral/SpectralFrame.h) | Mono frame/weighted overlap-add adaptation; see spectral lab |
| [spectral/SpectralHold.h](../lib/tms/spectral/SpectralHold.h) | Magnitude/phase-advance hold informed by fog-bank |
