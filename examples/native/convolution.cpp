#include <cstdint>
#include <cstdio>
#include "tms/dsp/FIR.h"
#include "tms/core/XorShift32.h"

// Tiny PCM16 writer for this fixed eight-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "convolution.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 8 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  // Four two-second comparisons: identity, averaging, differentiator,
  // and sparse reflections. These synthetic kernels are not measured rooms.
  static tms::FIR<2048> filter;
  static float reflections[2048] = {};
  reflections[0]=0.6f; reflections[441]=0.2f;
  reflections[1103]=0.12f; reflections[1985]=-0.08f;
  const float identity[]={1};
  const float average[]={0.125f,0.125f,0.125f,0.125f,0.125f,0.125f,0.125f,0.125f};
  const float difference[]={0.5f,-0.5f};
  tms::XorShift32 noise;
  for (int i = 0; i < frames; ++i) {
    int section = i / (2*tms::kSampleRate);
    int local = i % (2*tms::kSampleRate);
    if (local==0) {
      noise.seed(42); // Same input in every comparison.
      bool loaded = section==0 ? filter.load(identity,1) :
                    section==1 ? filter.load(average,8) :
                    section==2 ? filter.load(difference,2) : filter.load(reflections,2048);
      if (!loaded) { std::fclose(file); return 1; }
    }
    // An impulse followed by three short, repeatable noise bursts.
    int burstPosition = local % (tms::kSampleRate/2);
    float input = local==0 ? 0.7f : 0.0f;
    if (local >= tms::kSampleRate/2 && burstPosition<2205) {
      float envelope=sinf(tms::kPi*burstPosition/2205.0f);
      input=0.6f*envelope*noise.uniformSigned();
    }
    float sample=filter.tick(input);
    const int16_t pcm = static_cast<int16_t>(tms::clampf(sample,-1,1)*32767);
    littleEndian(file,static_cast<uint16_t>(pcm),2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 8 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
