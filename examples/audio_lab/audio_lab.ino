#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include "LabPatch.h"

constexpr unsigned kAudioBlocks=32;
struct Measurements {
  uint32_t blocks=0, maxCycles=0, transitionMaxCycles=0, late=0;
  uint32_t allocationFailures=0, missingInput=0, clipped=0;
  uint32_t lastCycles=0;
  float peak=0;
  uint8_t mode=0;
  bool external=false, holding=false;
};

class AudioTeachingLab : public AudioStream {
  audio_block_t* queue_[1];
  LabPatch patch_;
  tms::SineOscillator fundamental_, harmonic_;
  Measurements measurements_;
  volatile uint8_t requestedMode_=0;
  volatile bool requestedExternal_=false, requestedHold_=false;
public:
  AudioTeachingLab() : AudioStream(1,queue_) {}
  void prepare() {
    patch_.prepare(AUDIO_SAMPLE_RATE_EXACT);
    fundamental_.setSampleRate(AUDIO_SAMPLE_RATE_EXACT); fundamental_.setFrequency(440);
    harmonic_.setSampleRate(AUDIO_SAMPLE_RATE_EXACT); harmonic_.setFrequency(880);
  }
  void mode(uint8_t value) { requestedMode_=value; }
  void external(bool enabled) { requestedExternal_=enabled; }
  void hold(bool enabled) { requestedHold_=enabled; }
  Measurements snapshot() const { return measurements_; }
  void clearMeasurements() { measurements_={}; measurements_.mode=patch_.mode; }
  static constexpr size_t patchBytes() { return sizeof(LabPatch); }
  void update() override {
    uint32_t begin=ARM_DWT_CYCCNT;
    bool transition=requestedMode_!=patch_.mode;
    if(transition) patch_.select(static_cast<LabPatch::Mode>(requestedMode_));
    patch_.spectral.setHold(requestedHold_);
    measurements_.mode=patch_.mode; measurements_.external=requestedExternal_;
    audio_block_t* input=receiveReadOnly(0);
    audio_block_t* output=allocate();
    if(!output) ++measurements_.allocationFailures;
    if(requestedExternal_ && !input) ++measurements_.missingInput;
    if(output) {
      for(unsigned i=0;i<AUDIO_BLOCK_SAMPLES;++i) {
        float signal=requestedExternal_ ? (input ? input->data[i]/32768.0f : 0) :
            0.24f*fundamental_.tick()+0.12f*harmonic_.tick();
        float wet=patch_.tick(signal);
        if(!isfinite(wet)) { wet=0; ++measurements_.clipped; }
        measurements_.peak=fmaxf(measurements_.peak,fabsf(wet));
        if(fabsf(wet)>1) ++measurements_.clipped;
        // Conservative output trim; clamp is a bench guard, not a lookahead limiter.
        output->data[i]=static_cast<int16_t>(tms::clampf(wet,-1,1)*0.25f*32767);
      }
      transmit(output,0); release(output);
    }
    if(input) release(input);
    measurements_.holding=patch_.spectral.holding();
    uint32_t cycles=ARM_DWT_CYCCNT-begin;
    measurements_.lastCycles=cycles;
    if(transition) measurements_.transitionMaxCycles=max(measurements_.transitionMaxCycles,cycles);
    else measurements_.maxCycles=max(measurements_.maxCycles,cycles);
    const double budget=static_cast<double>(F_CPU_ACTUAL)*AUDIO_BLOCK_SAMPLES/AUDIO_SAMPLE_RATE_EXACT;
    if(cycles>=budget) ++measurements_.late;
    ++measurements_.blocks;
  }
};

AudioInputI2S input;
AudioTeachingLab lab;
AudioOutputI2S output;
AudioConnection inputToLab(input,0,lab,0);
AudioConnection labToLeft(lab,0,output,0);
AudioConnection labToRight(lab,0,output,1);
AudioControlSGTL5000 codec;
elapsedMillis reportTimer;
bool externalSource=false, holdEnabled=false, codecReady=false;
void printHelp() {
  Serial.printf("tms audio lab codec=%s sample_rate=%.3f block=%u cpu_hz=%lu\n",
      codecReady?"ready":"FAILED",AUDIO_SAMPLE_RATE_EXACT,AUDIO_BLOCK_SAMPLES,(unsigned long)F_CPU_ACTUAL);
  Serial.printf("resident_patch_bytes=%u node_bytes=%u audio_pool_blocks=%u block_bytes=%u\n",
      (unsigned)lab.patchBytes(),(unsigned)sizeof(lab),kAudioBlocks,(unsigned)sizeof(audio_block_t));
  Serial.printf("source_bytes=%u drum_state_bytes=%u sample_state_bytes=%u fir_bytes=%u grain_bytes=%u shift_bytes=%u spectral_bytes=%u\n",
      (unsigned)(LabPatch::sourceFrames*sizeof(float)),
      (unsigned)(sizeof(tms::SineOscillator)+2*sizeof(tms::ADSR)),
      (unsigned)sizeof(tms::SamplePlayer),(unsigned)sizeof(tms::FIR<256>),
      (unsigned)sizeof(tms::GrainCloud<8>),(unsigned)sizeof(tms::FrequencyShifter<129>),
      (unsigned)sizeof(tms::SpectralHold<512,128>));
  Serial.println("Modes 0=bypass 1=drum 2=sample 3=FIR 4=grains 5=shift 6=spectral; i=source toggle h=hold toggle r=clear peaks ?=help");
  Serial.println("mode,source,hold,blocks,last_us,max_us,transition_max_us,budget_us,late,graph_cpu_pct,graph_max_pct,pool_used,pool_max,alloc_fail,missing_input,clipped,peak");
}

void setup() {
  Serial.begin(115200);
  AudioMemory(kAudioBlocks);
  AudioNoInterrupts(); lab.prepare(); AudioInterrupts();
  codecReady=codec.enable();
  codec.inputSelect(AUDIO_INPUT_LINEIN); codec.lineInLevel(5); codec.volume(0.3f);
  // DWT is also used by the Teensy Audio library's profiler.
  ARM_DEMCR|=ARM_DEMCR_TRCENA; ARM_DWT_CTRL|=ARM_DWT_CTRL_CYCCNTENA;
  printHelp();
}

void loop() {
  while(Serial.available()) {
    char command=Serial.read();
    if(command=='?') { printHelp(); continue; }
    AudioNoInterrupts();
    if(command>='0' && command<='6') { lab.mode(command-'0'); holdEnabled=false; lab.hold(false); }
    else if(command=='i') { externalSource=!externalSource; lab.external(externalSource); }
    else if(command=='h') { holdEnabled=!holdEnabled; lab.hold(holdEnabled); }
    else if(command=='r') { lab.clearMeasurements(); AudioProcessorUsageMaxReset(); AudioMemoryUsageMaxReset(); }
    AudioInterrupts();
  }
  if(reportTimer>=1000) {
    reportTimer=0;
    AudioNoInterrupts();
    Measurements m=lab.snapshot();
    float cpu=AudioProcessorUsage(),maxCpu=AudioProcessorUsageMax();
    unsigned used=AudioMemoryUsage(),maxUsed=AudioMemoryUsageMax();
    AudioInterrupts();
    double cyclesPerUs=F_CPU_ACTUAL/1000000.0;
    Serial.printf("%u,%s,%u,%lu,%.2f,%.2f,%.2f,%.2f,%lu,%.2f,%.2f,%u,%u,%lu,%lu,%lu,%.4f\n",
        m.mode,m.external?"line":"test",m.holding,(unsigned long)m.blocks,
        m.lastCycles/cyclesPerUs,m.maxCycles/cyclesPerUs,m.transitionMaxCycles/cyclesPerUs,
        1000000.0*AUDIO_BLOCK_SAMPLES/AUDIO_SAMPLE_RATE_EXACT,(unsigned long)m.late,
        cpu,maxCpu,used,maxUsed,(unsigned long)m.allocationFailures,
        (unsigned long)m.missingInput,(unsigned long)m.clipped,m.peak);
  }
}
