# Second expansion: sample playback

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/tms_sample build/sample_playback.wav
```

Listen to the six one-second sections:

| Time | Experiment |
| --- | --- |
| 0–1 s | Four separate slices, with short onset/end fades |
| 1–2 s | Whole buffer at half speed |
| 2–3 s | Whole buffer at double speed |
| 3–4 s | Whole buffer backwards |
| 4–5 s | Original-speed loop with the raw seam |
| 5–6 s | Original-speed loop with a 256-source-sample overlap |

The source is synthesized into a fixed array, so the example needs no external
sample files. Try a larger overlap, a different rate, or a different slice count.

## Read the blocks

`sample/SampleView.h` describes an immutable, non-owning mono float buffer.
A slice uses [begin,end) bounds; equal slices put the division remainder into
the final slice, matching the slicing convention in `lofi-sampler`. Empty and
invalid slices are silent. If there are more slices than samples, earlier
zero-length slices are empty and the final slice receives the remainder.
Fractional reads use linear interpolation. One-shot reads clamp endpoints;
loop reads interpolate across the seam. Negative positions wrap in loop mode.

`sample/SamplePlayer.h` adds a moving read head. `tick()` runs once per output
sample. Rate is source samples per output sample: for different sample rates,
use `(sourceRate / outputRate) * pitchRatio`. Negative rates play backwards.
Zero and non-finite rates do not start playback. A one-shot stops when its read
head passes the slice; further ticks return zero. `stop()` silences immediately.
`trigger()` restarts at the forward or reverse endpoint. `setSource()` stops
playback. Interpolation stays within the selected slice.

For one-shots, fade length controls linear onset/end gain in source samples,
with zero at both endpoints. For loops, it controls an overlap between tail
and head. Fade length caps at half the view. The first traversal spans the whole
view; subsequent traversals skip the head already heard in the overlap, giving
an effective period of `size - fade`. Reverse loops apply the same operation in
reverse read order. This avoids replaying the overlapped head, but changes loop
length: use fade zero when exact original duration matters. Fades soften seams;
they do not guarantee click-free arbitrary signals or abrupt rate/trigger changes.

Both blocks allocate nothing. Keep the caller's buffer alive during playback;
configure and tick in one processing context or synchronize externally. Position
uses double precision to preserve fractional resolution on longer buffers.
The view itself does not own or validate the actual allocation behind its pointer.

Linear interpolation is intentionally visible, not a high-quality resampler.
Faster playback can alias; pitch changes also change duration. Recording,
overdubbing, stereo storage, file decoding, and time stretching remain future
layers. This step supplies the reusable reader/envelope/seam foundation.

## Provenance and verification

These are original teaching implementations informed by MIT-licensed
`orbit-looper/src/Doppler.cpp` and the crossfade composition in
`orbit-looper/src/OrbitLooper.cpp`. Equal-slice remainder handling is informed by
`lofi-sampler/firmware/platformio/src/Slicer.cpp`; its filesystem/storage layer
was not imported. No top-level license was found in `lofi-sampler`, so it is
recorded only as a conceptual reference; its code was not copied. This reader uses float mono data rather than interleaved PCM16.

`tests/sample.cpp` checks interpolation, wrap, one-shot termination, signed
rates, slice bounds/remainders, fades, overlap skip behavior, large steps,
invalid inputs, and empty/single-sample buffers.
