#pragma once
// Fixed ring buffer (power-of-two capacity). Externally synchronize concurrent
// access: volatile counters alone do not make a portable lock-free SPSC queue.
#include <stdint.h>
namespace tms {
template<typename T, uint32_t N> struct RingBuffer{
  static_assert(N > 0 && (N&(N-1))==0,"N pow2"); T data[N]; volatile uint32_t head=0,tail=0;
  inline uint32_t mask()const{return N-1;} inline uint32_t size()const{return head-tail;}
  inline uint32_t freeSpace()const{return N-size();}
  inline bool push(const T& v){ if(!freeSpace()) return false; data[head&mask()]=v; head++; return true; }
  inline bool pop(T& out){ if(!size()) return false; out=data[tail&mask()]; tail++; return true; }
};}
