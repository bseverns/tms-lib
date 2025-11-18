#pragma once
namespace tms { struct PresetStore{ virtual ~PresetStore(){} virtual bool save(const char* name)=0; virtual bool load(const char* name)=0; }; }
