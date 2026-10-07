#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/dsp/FIR.h"
void require(bool ok, const char* label) {
  if (!ok) { std::fprintf(stderr, "%s\n", label); std::exit(1); }
}
bool near(float a, float b) { return std::fabs(a-b)<0.00001f; }
int main() {
  tms::FIR<4> filter;
  require(near(filter.tick(0.7f),0.7f), "default identity");
  const float taps[] = {0.5f, -0.25f, 0.125f, 0.0625f};
  require(filter.load(taps,4), "load capacity kernel");
  for(int i=0;i<8;++i)
    require(near(filter.tick(i==0?1:0),i<4?taps[i]:0), "impulse equals kernel");
  filter.reset();
  float input[31], expected[31]={};
  for(int i=0;i<31;++i) {
    input[i]=std::sin(i*0.7f);
    for(int k=0;k<4 && k<=i;++k) expected[i]+=taps[k]*input[i-k];
  }
  float output[31];
  require(filter.processBlock(input,output,3) && filter.processBlock(input+3,output+3,28), "split blocks");
  for(int i=0;i<31;++i) require(near(output[i],expected[i]), "direct reference across wraps");
  filter.reset();
  require(filter.processBlock(input,input,31), "in place block");
  for(int i=0;i<31;++i) require(near(input[i],expected[i]), "in place values");
  const float delay[]={0,0,1}; filter.load(delay,3);
  require(near(filter.tick(1),0) && near(filter.tick(0),0) && near(filter.tick(0),1), "tap delay timing");
  const float gain[]={2}; filter.load(gain,1);
  require(near(filter.tick(0.75f),1.5f), "gain not clipped or normalized");
  filter.load(taps,4); filter.tick(1);
  require(!filter.load(nullptr,1) && !filter.load(taps,0) && !filter.load(taps,5), "invalid sizes rejected");
  const float invalid[]={std::numeric_limits<float>::quiet_NaN()};
  require(!filter.load(invalid,1) && near(filter.tick(0),-0.25f), "invalid load preserves history/kernel");
  filter.reset(); require(near(filter.tick(0),0) && filter.tapCount()==4, "reset preserves taps");
  filter.tick(1); filter.load(gain,1);
  require(near(filter.tick(0),0), "replacement resets old history");
  tms::FIR<1> single; single.load(gain,1);
  require(near(single.tick(0.25f),0.5f), "one tap capacity");
  require(filter.processBlock(nullptr,nullptr,0) && !filter.processBlock(nullptr,output,1), "block validation");
  std::puts("FIR checks passed");
}
