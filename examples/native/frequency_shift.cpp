#include <cstdint>
#include <cstdio>
#include "tms/dsp/FrequencyShifter.h"

// Tiny PCM16 writer for this fixed eight-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "frequency_shift.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 8 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  // Repeat a 600/1200/1800 Hz harmonic sound with four translations.
  tms::FrequencyShifter<> shifter;
  for(int i=0;i<frames;++i) {
    int section=i/(2*tms::kSampleRate), local=i%(2*tms::kSampleRate);
    if(local==0) {
      shifter.reset();
      const float shifts[]={0,100,-100,300};
      if(!shifter.setShiftHz(shifts[section])) { std::fclose(file); return 1; }
    }
    double time=static_cast<double>(local)/tms::kSampleRate;
    float input=static_cast<float>(0.25*sin(2*tms::kPi*600*time)+
        0.15*sin(2*tms::kPi*1200*time)+0.1*sin(2*tms::kPi*1800*time));
    // Fade excitation at each boundary, leaving a silent tail for the FIR.
    float edge=static_cast<float>(fmax(0.0,fmin(1.0,fmin(local/441.0,
        (2*tms::kSampleRate-441-local)/441.0))));
    float sample=shifter.tick(input*edge);
    const int16_t pcm=static_cast<int16_t>(tms::clampf(sample,-1,1)*32767);
    littleEndian(file,static_cast<uint16_t>(pcm),2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 8 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
