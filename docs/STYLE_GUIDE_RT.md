# Real-time Style Guide
- No allocation or locks on the audio thread.
- Keep hot-path branches minimal.
- Measure, don't guess (see `core/Profiler.h`).
- Avoid denormals (see `zapDenorm`).
