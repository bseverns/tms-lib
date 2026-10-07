#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "../examples/audio_lab/LabPatch.h"
void require(bool ok,const char* message) { if(!ok) { std::fprintf(stderr,"%s\n",message); std::exit(1); } }
int main() {
  static LabPatch patch;
  patch.prepare(44100);
  for(int mode=0;mode<LabPatch::Count;++mode) {
    patch.select(static_cast<LabPatch::Mode>(mode));
    double energy=0;
    for(int i=0;i<44100;++i) {
      float input=0.24f*std::sin(2*tms::kPi*440*i/44100.0f);
      float output=patch.tick(input);
      require(std::isfinite(output) && std::fabs(output)<1,"lab output finite and below clamp");
      energy+=output*output;
    }
    require(energy>1,"every bench mode produces audio");
  }
  patch.select(LabPatch::Bypass); require(std::fabs(patch.tick(0.3f)-0.3f)<1e-6f,"bench bypass identity");
  patch.select(LabPatch::Spectral);
  for(int i=0;i<1024;++i) require(std::fabs(patch.tick(i==0?1:0)-(i==512?1:0))<0.0002f,"bench spectral latency");
  patch.spectral.setHold(true);
  for(int i=0;i<128;++i) patch.tick(0);
  require(patch.spectral.holding(),"bench hold control");
  patch.select(LabPatch::Spectral); require(!patch.spectral.holding(),"reselect resets hold");
  patch.prepare(48000);
  require(patch.triggerEvery==24000,"bench uses configured sample rate");
  std::printf("Portable audio lab passed; resident patch bytes=%zu (host layout)\n",sizeof(patch));
}
