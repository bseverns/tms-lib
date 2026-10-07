#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/sample/SamplePlayer.h"
void require(bool ok, const char* label) {
  if (!ok) { std::fprintf(stderr, "%s\n", label); std::exit(1); }
}
bool near(float a, float b) { return std::fabs(a-b)<0.0001f; }
int main() {
  const float data[] = {0, 1, 2, 3, 4, 5, 6};
  tms::SampleView view{data, 7};
  require(near(view.read(1.5), 1.5), "fractional interpolation");
  require(near(view.read(-0.5, true), 3), "negative loop seam");
  require(near(view.read(700001.5, true), 1.5), "large loop position");
  require(near(view.read(-100), 0) && near(view.read(100), 6), "clamped endpoints");
  require(view.slice(2, 5).size == 3 && view.slice(4, 3).size == 0, "slice bounds");
  require(view.equalSlice(0,3).size==2 && view.equalSlice(2,3).size==3, "slice remainder");
  require(!view.equalSlice(0,0).data && !view.equalSlice(3,3).data, "invalid slice");
  require(near(view.read(std::numeric_limits<double>::infinity()),0), "invalid read");
  tms::SamplePlayer p; p.setSource(view); p.trigger();
  for (float sample : data) require(near(p.tick(),sample), "one shot order");
  require(!p.active() && near(p.tick(),0), "one shot stops");
  p.trigger(-1);
  for (int i=6;i>=0;--i) require(near(p.tick(),data[i]), "reverse order");
  p.trigger(0.5); require(near(p.tick(),0) && near(p.tick(),0.5), "half speed");
  p.trigger(2); require(near(p.tick(),0) && near(p.tick(),2), "double speed");
  p.trigger(1,true); for(int i=0;i<21;++i) require(near(p.tick(),data[i%7]), "loop period");
  p.trigger(1,true,2);
  const float blend[] = {0,1,2,3,4,5,3.5f,2,3,4,5,3.5f,2};
  for(float sample:blend) require(near(p.tick(),sample), "overlap skip continuity");
  p.trigger(-1,true,2);
  const float reverse[] = {6,5,4,3,2,1,2.5f,4};
  for(float sample:reverse) require(near(p.tick(),sample), "reverse crossfade");
  p.trigger(1,false,2);
  require(near(p.tick(),0) && near(p.tick(),0.5), "onset fade");
  for(int i=0;i<4;++i) p.tick();
  require(near(p.tick(),0) && !p.active(), "endpoint fade");
  p.trigger(1,true,100); require(p.fade==3, "fade capped");
  p.trigger(1000,true,2); for(int i=0;i<100;++i) require(std::isfinite(p.tick()) && p.position<7, "large step bounded");
  const float seam[] = {1,1,1,1,-1,-1,-1,-1};
  tms::SamplePlayer raw, smooth;
  raw.setSource({seam,8}); smooth.setSource({seam,8});
  raw.trigger(1,true); smooth.trigger(1,true,2);
  float rawBefore=0, smoothBefore=0;
  for(int i=0;i<8;++i) { rawBefore=raw.tick(); smoothBefore=smooth.tick(); }
  require(std::fabs(smooth.tick()-smoothBefore)<std::fabs(raw.tick()-rawBefore),
          "crossfade reduces deliberate seam jump");
  p.trigger(0); require(!p.active(), "zero rate rejected");
  p.trigger(std::numeric_limits<double>::quiet_NaN()); require(!p.active(), "NaN rate rejected");
  p.setSource({}); p.trigger(1,true); require(near(p.tick(),0), "empty silence");
  const float one[] = {0.75f}; p.setSource({one,1}); p.trigger(-0.5,true,5);
  for(int i=0;i<10;++i) require(near(p.tick(),0.75f), "single sample loop");
  std::puts("Sample checks passed");
}
