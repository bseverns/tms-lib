#pragma once
#include <stdint.h>
namespace tms { enum class Event:uint8_t{Grain,Pluck,Tap,SceneA,SceneB,FreezeOn,FreezeOff}; typedef void(*TriggerFn)(Event,uint8_t,uint8_t);
struct TriggerBus{ TriggerFn fn=nullptr; void set(TriggerFn f){fn=f;} void emit(Event e,uint8_t lane,uint8_t v){ if(fn) fn(e,lane,v);} }; }
