#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "tms/sample/GrainCloud.h"
void require(bool ok, const char* label) {
  if (!ok) { std::fprintf(stderr,"%s\n",label); std::exit(1); }
}
bool near(float a,float b) { return std::fabs(a-b)<0.00001f; }
int main() {
  require(near(tms::hannWindow(0,5),0) && near(tms::hannWindow(4,5),0) &&
          near(tms::hannWindow(2,5),1), "window endpoints/center");
  require(near(tms::hannWindow(1,5),tms::hannWindow(3,5)) &&
          near(tms::hannWindow(9,5),0) && near(tms::hannWindow(1,2),0), "window symmetry/bounds");
  float data[]={0,1,2,3}; tms::SampleView view{data,4}; tms::GrainVoice voice;
  require(voice.trigger(view,0,1,5),"trigger");
  float expected[]={0,0.5f,2,1.5f,0};
  for(float sample:expected) require(near(voice.tick(),sample),"windowed forward grain");
  require(!voice.active() && near(voice.tick(),0),"exact duration");
  voice.trigger(view,3,-1,5);
  require(near(voice.tick(),0) && near(voice.tick(),1) && near(voice.tick(),1),"reverse grain");
  voice.trigger(view,1,0.5,5); voice.tick();
  require(near(voice.tick(),0.75f),"fractional grain");
  voice.trigger(view,1,0,5); voice.tick(); voice.tick();
  require(near(voice.tick(),1),"frozen source");
  voice.trigger(view,3,1,5,1,false); voice.tick();
  require(near(voice.tick(),0) && near(voice.tick(),0),"nonwrapping zero pad");
  voice.trigger(view,-9,1001,5); for(int i=0;i<5;++i) require(std::isfinite(voice.tick()),"large wrapped rates");
  require(!voice.trigger(view,0,1,2) && !voice.active(),"short grain rejected");
  require(!voice.trigger({},0,1,5) && !voice.trigger(view,0,std::numeric_limits<double>::infinity(),5),"invalid grain rejected");
  tms::GrainCloud<2> a,b;
  require(a.start(view,3,5,1,0.5,2,0.2f,42) && b.start(view,3,5,1,0.5,2,0.2f,42),"cloud setup");
  for(int i=0;i<30;++i) require(near(a.tick(),b.tick()) && a.activeVoices()<=2,"seed repeatability/voice bound");
  require(a.launched==10 && a.dropped==0,"sample-accurate launch count");
  tms::GrainCloud<1> full; full.start(view,1,5,1,0);
  for(int i=0;i<10;++i) full.tick();
  require(full.launched==2 && full.dropped==8,"capacity drops new grains");
  full.stop(); require(near(full.tick(),0) && full.activeVoices()==0,"stop clears cloud");
  require(!full.start(view,0,5,0) && !full.running,"invalid interval");
  std::puts("Granular checks passed");
}
