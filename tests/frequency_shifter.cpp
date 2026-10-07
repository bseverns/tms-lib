#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/dsp/FrequencyShifter.h"
void require(bool ok, const char* label) {
  if (!ok) { std::fprintf(stderr,"%s\n",label); std::exit(1); }
}
bool near(float a,float b) { return std::fabs(a-b)<0.00001f; }
// Correlate with a known frequency rather than depending on FFT libraries.
double amplitude(const float* samples,int count,double frequency,double sr) {
  double real=0,imag=0;
  for(int i=0;i<count;++i) {
    double angle=2*3.14159265358979323846*frequency*i/sr;
    real+=samples[i]*std::cos(angle); imag+=samples[i]*std::sin(angle);
  }
  return 2*std::sqrt(real*real+imag*imag)/count;
}
int main() {
  tms::HilbertPair<> pair;
  require(pair.latencySamples()==64,"documented latency");
  for(int i=0;i<160;++i) {
    float real,q; pair.tick(i==0?1:0,real,q);
    require(near(real,i==64?1:0),"real branch delay");
    require(std::isfinite(q),"finite impulse");
  }
  pair.reset(); float real,q; pair.tick(0,real,q);
  require(near(real,0) && near(q,0),"pair reset silence");
  constexpr int sr=44100, count=sr;
  static float output[count];
  for(float tone : {1000.0f,4000.0f}) {
    for(float shift : {200.0f,-200.0f}) {
      tms::FrequencyShifter<> shifter; require(shifter.setShiftHz(shift),"valid shift");
      for(int i=0;i<sr+count;++i) {
        float input=static_cast<float>(std::cos(2*3.14159265358979323846*tone*i/sr));
        float sample=shifter.tick(input);
        if(i>=sr) output[i-sr]=sample;
      }
      double wanted=amplitude(output,count,tone+shift,sr);
      double image=amplitude(output,count,tone-shift,sr);
      require(wanted>0.98 && wanted<1.02,"shifted tone unity amplitude");
      require(image/wanted<0.01,"unwanted sideband below -40 dB at tested tones");
      std::printf("tone %.0f shift %.0f: image %.1f dB\n",tone,shift,20*std::log10(image/wanted));
    }
  }
  tms::FrequencyShifter<> zero;
  for(int i=0;i<256;++i) require(near(zero.tick(i==0?1:0),i==64?1:0),"zero shift delayed identity");
  require(!zero.setShiftHz(std::numeric_limits<float>::quiet_NaN()) &&
          !zero.setShiftHz(22050) && !zero.setSampleRate(0),"invalid settings rejected");
  require(zero.setShiftHz(1000) && !zero.setSampleRate(1000) && zero.setSampleRate(48000),"sample-rate configuration");
  zero.reset(); for(int i=0;i<256;++i) require(near(zero.tick(0),0),"shifter reset silence");
  std::puts("Frequency-shifter checks passed");
}
