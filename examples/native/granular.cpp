#include <cstdint>
#include <cstdio>
#include "tms/sample/GrainCloud.h"

// Tiny PCM16 writer for this fixed eight-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "granular.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 8 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  // Reuse the same source and seed across all comparisons.
  static float source[tms::kSampleRate];
  for (int i=0;i<tms::kSampleRate;++i) {
    float time=static_cast<float>(i)/tms::kSampleRate;
    source[i]=0.5f*sinf(2*tms::kPi*(180*time+220*time*time)) +
              0.15f*sinf(2*tms::kPi*730*time);
  }
  tms::SampleView view{source,tms::kSampleRate};
  tms::GrainCloud<8> cloud;
  for(int i=0;i<frames;++i) {
    int section=i/(2*tms::kSampleRate), local=i%(2*tms::kSampleRate);
    if(local==0) {
      // Sparse grains, overlap, position spray, then reverse/pitch change.
      size_t intervals[]={8820,1102,1102,1102};
      double rates[]={1,1,1,-0.75};
      float sprays[]={0,0,12000,12000};
      if(!cloud.start(view,intervals[section],4410,16000,rates[section],
                      sprays[section],0.2f,42)) { std::fclose(file); return 1; }
    }
    float sample=cloud.tick();
    // Fade section boundaries, independent of each grain's Hann window.
    float edge=static_cast<float>(fmin(1.0,fmin(local/441.0,
        (2*tms::kSampleRate-1-local)/441.0)));
    const int16_t pcm=static_cast<int16_t>(tms::clampf(sample*edge,-1,1)*32767);
    littleEndian(file,static_cast<uint16_t>(pcm),2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 8 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
