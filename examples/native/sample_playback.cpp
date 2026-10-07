#include <cstdint>
#include <cstdio>
#include "tms/sample/SamplePlayer.h"

// Tiny PCM16 writer for this fixed six-second mono experiment.
void littleEndian(FILE* file, uint32_t value, int bytes) {
  for (int i = 0; i < bytes; ++i) std::fputc((value >> (i * 8)) & 255, file);
}
int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "sample_playback.wav";
  FILE* file = std::fopen(path, "wb");
  if (!file) { std::perror(path); return 1; }
  constexpr int frames = 6 * tms::kSampleRate;
  std::fwrite("RIFF", 1, 4, file); littleEndian(file, 36 + frames * 2, 4);
  std::fwrite("WAVEfmt ", 1, 8, file); littleEndian(file, 16, 4);
  littleEndian(file, 1, 2); littleEndian(file, 1, 2);
  littleEndian(file, tms::kSampleRate, 4); littleEndian(file, tms::kSampleRate * 2, 4);
  littleEndian(file, 2, 2); littleEndian(file, 16, 2);
  std::fwrite("data", 1, 4, file); littleEndian(file, frames * 2, 4);

  // Fixed source: four distinct tone slices, intentionally discontinuous ends.
  constexpr int sourceFrames = tms::kSampleRate / 5;
  static float samples[sourceFrames];
  for (int i = 0; i < sourceFrames; ++i) {
    int slice = i * 4 / sourceFrames;
    samples[i] = 0.6f * sinf(2 * tms::kPi * (180 + slice * 110) * i / tms::kSampleRate);
  }
  tms::SampleView view{samples, sourceFrames};
  tms::SamplePlayer player;
  for (int i = 0; i < frames; ++i) {
    int section = i / tms::kSampleRate;
    int local = i % tms::kSampleRate;
    if (section == 0 && local % (tms::kSampleRate / 4) == 0) {
      player.setSource(view.equalSlice(local / (tms::kSampleRate / 4), 4));
      player.trigger(1, false, 64);
    } else if (section > 0 && local == 0) {
      player.setSource(view);
      const double rates[] = {1, 0.5, 2, -1, 1, 1};
      player.trigger(rates[section], true, section == 5 ? 256 : 0);
    }
    float sample = player.tick();
    // Fade section boundaries only, leaving internal loop seams audible.
    float edge = static_cast<float>(fmin(1.0, fmin(local / 220.0,
        (tms::kSampleRate - 1 - local) / 220.0)));
    const int16_t pcm = static_cast<int16_t>(tms::clampf(sample * edge, -1, 1) * 32767);
    littleEndian(file, static_cast<uint16_t>(pcm), 2);
  }
  bool failed = std::ferror(file);
  if (std::fclose(file) != 0) failed = true;
  if (failed) { std::fprintf(stderr, "Could not write WAV\n"); return 1; }
  std::printf("Rendered %s: 6 seconds, mono PCM16, %d Hz\n", path, tms::kSampleRate);
}
