#include <cstdint>
#include <cstdio>
#include "tms/control/ADSR.h"
#include "tms/core/XorShift32.h"
#include "tms/dsp/SineOscillator.h"

// Tiny PCM16 writer for this fixed four-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "drum_voice.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 4 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  tms::SineOscillator body;
  tms::ADSR amplitude, pitch;
  amplitude.set(1, 180, 0, 30);
  pitch.set(0, 45, 0, 10);
  tms::XorShift32 noise;
  noise.seed(42);
  for (int i = 0; i < frames; ++i) {
    const int position = i % (tms::kSampleRate / 2);
    if (position == 0) { body.reset(); amplitude.noteOn(); pitch.noteOn(); }
    if (position == tms::kSampleRate / 4) { amplitude.noteOff(); pitch.noteOff(); }
    const float bend = pitch.tick();
    body.setFrequency(55.0f + 140.0f * bend);
    const float sample = amplitude.tick() *
        (0.7f * body.tick() + 0.08f * bend * noise.uniformSigned());
    const int16_t pcm = static_cast<int16_t>(tms::clampf(sample, -1, 1) * 32767);
    littleEndian(file, static_cast<uint16_t>(pcm), 2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 4 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
