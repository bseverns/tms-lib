#pragma once
#include <stdint.h>
namespace tms {
template<typename T,int N> struct ObjectPool{
  alignas(T) unsigned char storage[N][sizeof(T)]; bool used[N]={false};
  T* acquire(){ for(int i=0;i<N;++i){ if(!used[i]){ used[i]=true; return new(storage[i]) T(); } } return nullptr; }
  void release(T* p){ if(!p) return; p->~T(); for(int i=0;i<N;++i){ if(p==(T*)storage[i]){ used[i]=false; return; } } }
};}
