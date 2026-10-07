#pragma once
#include "tms/dsp/SineOscillator.h"
#include "tms/control/ADSR.h"
#include "tms/sample/SamplePlayer.h"
#include "tms/sample/GrainCloud.h"
#include "tms/dsp/FIR.h"
#include "tms/dsp/FrequencyShifter.h"
#include "tms/spectral/SpectralHold.h"

// Portable patch exercised by native tests and the Teensy callback.
// All modes stay allocated so reported resident RAM describes the entire lab.
struct LabPatch {
  enum Mode { Bypass, Drum, Sample, Convolution, Grains, Shift, Spectral, Count };
  static constexpr size_t sourceFrames = 2048;
  float source[sourceFrames] = {};
  tms::SineOscillator body;
  tms::ADSR amplitude, pitch;
  tms::SamplePlayer player;
  tms::GrainCloud<8> cloud;
  tms::FIR<256> fir;
  tms::FrequencyShifter<129> shifter;
  tms::SpectralHold<512,128> spectral;
  float sampleRate = 44100;
  uint32_t triggerEvery = 22050, age = 0;
  Mode mode = Bypass;

  void prepare(float sr) {
    sampleRate = sr; triggerEvery = static_cast<uint32_t>(sr * 0.5f);
    body.setSampleRate(sr); amplitude.setSampleRate(sr); pitch.setSampleRate(sr);
    amplitude.set(1,180,0,30); pitch.set(0,45,0,10);
    shifter.setSampleRate(sr); shifter.setShiftHz(100);
    spectral.setSampleRate(sr); spectral.setDecaySeconds(0);
    for(size_t i=0;i<sourceFrames;++i)
      source[i]=0.4f*sinf(2*tms::kPi*16*i/sourceFrames)+0.15f*sinf(2*tms::kPi*37*i/sourceFrames);
    // Full 256-tap workload: bounded L1 gain, with explicit sparse reflections.
    float taps[256]={}; taps[0]=0.6f; taps[71]=0.2f; taps[191]=-0.1f;
    fir.load(taps,256);
    select(Bypass);
  }
  void select(Mode next) {
    if(next<Bypass || next>=Count) return;
    mode=next; age=0;
    switch(mode) {
    case Drum: body.reset(); amplitude.reset(); pitch.reset(); break;
    case Sample: player.setSource({source,sourceFrames}); player.trigger(0.75,true,64); break;
    case Convolution: fir.reset(); break;
    case Grains: cloud.start({source,sourceFrames},static_cast<size_t>(sampleRate*0.025f),
        static_cast<size_t>(sampleRate*0.1f),700,0.8f,600,0.18f,42); break;
    case Shift: shifter.reset(); break;
    case Spectral: spectral.reset(); break;
    default: break;
    }
  }
  float tick(float input) {
    float out=0;
    switch(mode) {
    case Bypass: out=input; break;
    case Drum:
      if(age==0) { body.reset(); amplitude.noteOn(); pitch.noteOn(); }
      if(age==triggerEvery/2) { amplitude.noteOff(); pitch.noteOff(); }
      body.setFrequency(55+140*pitch.tick()); out=0.6f*body.tick()*amplitude.tick(); break;
    case Sample: out=player.tick(); break;
    case Convolution: out=fir.tick(input); break;
    case Grains: out=cloud.tick(); break;
    case Shift: out=shifter.tick(input); break;
    case Spectral: out=spectral.tick(input); break;
    default: break;
    }
    if(++age==triggerEvery) age=0;
    return out;
  }
};
