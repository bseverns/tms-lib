#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/spectral/SpectralHold.h"
void require(bool ok,const char* message) { if(!ok) { std::fprintf(stderr,"%s\n",message); std::exit(1); } }
bool near(float a,float b,float tolerance=0.0001f) { return std::fabs(a-b)<tolerance; }
template<size_t N,size_t Hop> void reconstruct() {
  tms::SpectralFrame<N,Hop> frame;
  float input[4096];
  for(int i=0;i<4096;++i) input[i]=0.3f*std::sin(i*0.123f)+0.2f*std::cos(i*0.749f);
  for(size_t i=0;i<4096+N;++i) {
    float output=frame.tick(i<4096?input[i]:0);
    require(near(output,i<N?0:input[i-N],0.0002f),"STFT reconstructs delayed input including startup/tail");
  }
  frame.reset();
  for(size_t i=0;i<4*N;++i) require(near(frame.tick(i==0?1:0),i==N?1:0),"STFT impulse latency");
}
int main() {
  tms::FFT<16>::Buffer real={},imag={}; real[0]=1;
  tms::FFT<16>::transform(real,imag);
  for(int i=0;i<16;++i) require(near(real[i],1) && near(imag[i],0),"FFT impulse spectrum");
  for(int i=0;i<16;++i) { real[i]=std::cos(i*0.7f); imag[i]=std::sin(i*0.2f); }
  auto originalReal=real,originalImag=imag;
  tms::FFT<16>::transform(real,imag); tms::FFT<16>::transform(real,imag,true);
  for(int i=0;i<16;++i) require(near(real[i],originalReal[i]) && near(imag[i],originalImag[i]),"complex FFT roundtrip");
  real.fill(0); imag.fill(0);
  for(int i=0;i<16;++i) real[i]=std::cos(2*3.14159265358979323846*3*i/16);
  tms::FFT<16>::transform(real,imag);
  require(near(real[3],8) && near(real[13],8),"FFT known bin and mirror");
  reconstruct<64,16>(); reconstruct<128,64>(); reconstruct<512,128>();
  tms::SpectralHold<256,64> held,changed;
  held.setSampleRate(8192); changed.setSampleRate(8192);
  for(int i=0;i<4096;++i) {
    float tone=0.4f*std::cos(2*3.14159265358979323846*512*i/8192);
    held.tick(tone); changed.tick(tone);
  }
  held.setHold(true); changed.setHold(true);
  // Capture the same next complete frame.
  for(int i=4096;i<4160;++i) {
    float tone=0.4f*std::cos(2*3.14159265358979323846*512*i/8192);
    held.tick(tone); changed.tick(tone);
  }
  require(held.holding(),"hold captured at next frame");
  double energy=0,realTone=0,imagTone=0;
  for(int i=0;i<8192;++i) {
    float a=held.tick(0),b=changed.tick(0.7f*std::sin(i*0.73f));
    require(near(a,b),"held spectrum ignores changing live input");
    if(i>=512) {
      energy+=a*a;
      realTone+=a*std::cos(2*3.14159265358979323846*512*i/8192);
      imagTone+=a*std::sin(2*3.14159265358979323846*512*i/8192);
    }
  }
  require(energy>300 && energy<800,"held tone sustains bounded energy");
  require(2*std::hypot(realTone,imagTone)/7680>0.35,"held tone retains frequency/amplitude");
  tms::SpectralHold<256,64> offBin;
  offBin.setSampleRate(8192);
  for(int i=0;i<4096;++i) offBin.tick(0.4f*std::cos(2*3.14159265358979323846*440*i/8192));
  offBin.setHold(true);
  for(int i=4096;i<4160;++i) offBin.tick(0.4f*std::cos(2*3.14159265358979323846*440*i/8192));
  double offReal=0,offImag=0;
  for(int i=0;i<8192;++i) {
    float x=offBin.tick(0);
    if(i>=512) { offReal+=x*std::cos(2*3.14159265358979323846*440*i/8192);
                 offImag+=x*std::sin(2*3.14159265358979323846*440*i/8192); }
  }
  require(2*std::hypot(offReal,offImag)/7680>0.3,"off-bin held tone preserves frequency");
  held.setDecaySeconds(0.25f);
  double early=0,late=0;
  for(int i=0;i<8192;++i) {
    float x=held.tick(0);
    if(i>=512 && i<1536) early+=x*x;
    if(i>=7168) late+=x*x;
  }
  require(late<early*0.01,"held magnitude decays");
  changed.setHold(false);
  for(int i=0;i<1024;++i) changed.tick(0);
  for(int i=0;i<256;++i) require(near(changed.tick(0),0),"release resumes live silence");
  held.reset(); require(!held.holding(),"reset clears hold");
  for(int i=0;i<1024;++i) require(near(held.tick(0),0),"reset silence");
  require(!held.setDecaySeconds(-1) && !held.setSampleRate(0) &&
          !held.setDecaySeconds(std::numeric_limits<float>::infinity()),"invalid settings rejected");
  std::puts("Spectral checks passed");
}
