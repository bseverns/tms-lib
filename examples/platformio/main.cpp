// Select one sketch so Arduino setup()/loop() are defined exactly once.
#ifndef TMS_DEMO
#define TMS_DEMO 0
#endif
#if TMS_DEMO == 0
#include "../param_demo.ino"
#elif TMS_DEMO == 1
#include "../euclid_quantizer_demo.ino"
#elif TMS_DEMO == 2
#include "../stereo_image_demo.ino"
#elif TMS_DEMO == 3
#include "../orbit_motion_demo.ino"
#elif TMS_DEMO == 4
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include "../audio_lab/audio_lab.ino"
#else
#error "Unknown TMS_DEMO"
#endif
