#include <cstdint>
#include <cstdio>
#include "tms/spectral/SpectralHold.h"

// Tiny PCM16 writer for this fixed eight-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "spectral_hold.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 8 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  // Four two-second comparisons: live, freeze, decay, release.
  tms::SpectralHold<> spectral;
  for(int i=0;i<frames;++i) {
    int section=i/(2*tms::kSampleRate), local=i%(2*tms::kSampleRate);
    if(local==0) {
      spectral.reset();
      spectral.setDecaySeconds(section==2 ? 0.35f : 0);
    }
    if(section>0 && local==tms::kSampleRate/2) spectral.setHold(true);
    if(section==3 && local==tms::kSampleRate*5/4) spectral.setHold(false);
    double time=static_cast<double>(local)/tms::kSampleRate;
    float frequency=local<tms::kSampleRate*3/4 ? 440.0f : 660.0f;
    float input=static_cast<float>(0.24*sin(2*tms::kPi*frequency*time)+
                                   0.12*sin(2*tms::kPi*frequency*2*time));
    // Smooth the source change, but keep the held branch independent of it.
    float change=static_cast<float>(fmin(1.0,fabs(local-tms::kSampleRate*3/4)/220.0));
    float edge=static_cast<float>(fmax(0.0,fmin(1.0,fmin(local/441.0,
        (2*tms::kSampleRate-441-local)/441.0))));
    float sample=spectral.tick(input*edge*change);
    // Fade rendered section boundaries so resetting the held tail stays quiet.
    sample*=edge;
    const int16_t pcm=static_cast<int16_t>(tms::clampf(sample,-1,1)*32767);
    littleEndian(file,static_cast<uint16_t>(pcm),2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 8 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
