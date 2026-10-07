# Contributing teaching blocks

Keep each adaptation small enough to inspect in one lesson. Match the neighbor
projects' separation of DSP, control, hardware wiring, examples, and native checks.
Use C++17, the `tms` namespace, and includes rooted at `tms/`.

For each new block:

1. Inspect the source implementation and its tests. Record the repository and
   exact paths in `docs/SOURCE_MAP.md`; explain what was simplified.
2. Keep hardware and vendor APIs outside the portable header. Use fixed storage,
   and document its memory cost and sample-rate assumptions.
3. Explain input/output units, state/reset behavior, and how often processing
   must run. Distinguish sample ticks, block updates, and control polling.
4. Supply a small experiment with observable behavior and a native behavioral
   check where appropriate (an impulse, invariant, timing boundary, or round trip).
5. Compile the header independently, run native checks, and build affected
   Teensy examples. Mark exercises and incomplete algorithms explicitly.

Do not copy whole firmware engines simply to increase coverage. Extract the
portable mechanism first. Preserve source licensing and attribution; check the
source license before importing additional code.

Run `cmake -S . -B build`, `cmake --build build --parallel`, and
`ctest --test-dir build --output-on-failure`. To embed the library in a CMake
project, disable `TMS_BUILD_TESTS` and `TMS_BUILD_EXAMPLES`, call `add_subdirectory`,
and link `tms::tms`. Existing header paths remain the public interface.
