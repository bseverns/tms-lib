# Teensy audio bench: hear the blocks and measure their cost

This lab uses the same portable patch in native tests and a Teensy AudioStream
callback. It connects line input to a selectable mono DSP node and duplicates
the node's output to both I2S output channels. An internal 440/880 Hz test signal
is selected at startup, so no input cable is required for the first measurements.

## Hardware and build

Use Teensy 4.0 or 4.1, a compatible SGTL5000 Teensy Audio Shield, USB, and
headphones or a line-output monitoring path. Mount/wire the shield for your board;
no pots, buttons, SD card, or PSRAM are required. Start monitoring at low volume.
For external tests connect line-level audio to the shield's left line input.
This sketch selects line input, not the microphone input.

```sh
pio run -e audio_lab41
pio run -e audio_lab41 -t upload
pio device monitor -b 115200
```

Use `audio_lab40` for Teensy 4.0. The audio lab environments pin Teensy platform 5.2.0 for reproducible builds.
They use the Audio, Wire, and SPI
libraries bundled with the installed Teensy platform. Deep dependency discovery
handles the sketch wrapper; no separate DSP package is required.

If the global PlatformIO directory is not writable, use a local core:

```sh
PLATFORMIO_CORE_DIR="$PWD/.pio-core" pio run -e audio_lab41
PLATFORMIO_CORE_DIR="$PWD/.pio-core" pio run -e audio_lab41 -t upload
PLATFORMIO_CORE_DIR="$PWD/.pio-core" pio device monitor -b 115200
```

The local core is ignored by Git. PlatformIO can install its toolchain there.
For this workspace, installed Teensy tools were copied into that directory to
build without changing the global installation.

Boot reports codec status, actual sample rate, block size, and CPU clock. If
`codec=FAILED`, check shield power/I2C/wiring before interpreting listening
results. The callback can still run without successful codec initialization;
CPU numbers alone do not establish that the audio path works. Send `?` to reprint
status/help/memory/header if you opened the monitor after startup.

## Modes and commands

| Command | Patch | Source |
| --- | --- | --- |
| `0` | Bypass | Internal test signal or line input |
| `1` | Sine drum with amplitude/pitch envelopes | Generated in patch |
| `2` | Fractional sample playback with seam crossfade | Fixed 2048-sample source |
| `3` | 256-tap direct FIR, sparse reflection kernel | Internal test signal or line input |
| `4` | Eight-voice grain pool; 100 ms grains, 25 ms spacing | Fixed source with seeded position spray |
| `5` | 129-tap Hilbert frequency shift, +100 Hz | Internal test signal or line input |
| `6` | 512-sample spectral frame, 128-sample hop | Internal test signal or line input |
| `i` | Toggle test/left line input | Affects modes 0, 3, 5, 6 |
| `h` | Toggle spectral hold request | Effective in mode 6 at frame cadence |
| `r` | Clear node counters and graph/pool peaks | Does not reset DSP state |
| `?` | Print memory sizes, commands, and CSV header | Foreground only |

Send single characters; line endings are ignored. Changing modes resets the
selected patch at the next callback boundary and clears the hold request.
Selecting the currently active mode does not reset it. There is no automatic
mode rotation. Internal test oscillators are paused in external-input mode.
The fixed source uses integer-period tones; sample/grain pitch follows its rate,
not the 440 Hz internal test oscillator.

Output is clamped to [-1,1], trimmed to 25%, and codec headphone gain starts at
0.3. This is a bench guard, not a lookahead limiter. `peak` and `clipped` describe
pre-trim DSP output. Non-finite outputs are silenced and counted as clipped.

## Collect comparable measurements

1. Confirm audible bypass/internal tone. Record the printed rate/CPU, board,
   shield, firmware revision, compiler/platform versions, and source selection.
2. Select a mode and wait one second for state to settle. Send `r`, then collect
   at least 30 seconds of CSV rows. Use the worst observed values, not one row.
3. Repeat for every mode. For spectral mode measure live, then send `h`, wait
   one second, send `r`, and measure held operation separately. Release with `h`.
4. Measure mode-change cost separately: clear with `r` before switching modes,
   then retain `transition_max_us`. Do not clear that value before recording it.
5. Repeat relevant input-processing modes with line audio. Include silence,
   tones, and transients; note the source and gain. Listen for clicks/dropouts.
6. Save raw monitor output with the firmware/build details. Copy maxima into
   [AUDIO_LAB_RESULTS.csv](AUDIO_LAB_RESULTS.csv). Empty fields in that worksheet
   are deliberately unmeasured, not zeros.

Serial output occurs once per second in `loop()`, never in `update()`. Commands
and stats snapshots briefly disable Audio update interrupts. Expensive mode
resets happen inside the timed callback; no configuration/allocation of DSP
buffers is hidden in the foreground during measurements. The Teensy Audio
library's `allocate()` obtains fixed-pool audio blocks, not heap objects.

## Read the counters correctly

| CSV field | Meaning |
| --- | --- |
| `mode,source,hold` | Applied mode, input selection, and captured spectral hold state |
| `blocks` | Calls since `r`, including allocation failures |
| `last_us` | Most recent lab callback wall duration from DWT cycle counter |
| `max_us` | Maximum non-mode-transition callback duration since `r` |
| `transition_max_us` | Maximum callback containing a mode change since `r` |
| `budget_us` | `1e6 * blockSamples / actualSampleRate`; normally about 2902.49 us |
| `late` | Lab callback durations at or above one block budget |
| `graph_cpu_pct,graph_max_pct` | Teensy Audio library whole-update CPU usage/current and high-water mark |
| `pool_used,pool_max` | Current and high-water occupied blocks in the 32-block pool |
| `alloc_fail` | Lab output block allocation failures |
| `missing_input` | Missing received input blocks when external mode is selected |
| `clipped,peak` | Pre-trim output samples outside [-1,1] or non-finite, and peak magnitude |

The cycle timer includes callback bookkeeping and any higher-priority interrupt
preemption. Steady-state maxima include spectral FFT bursts. Whole-graph usage
includes input/output nodes, while node timing covers only the lab's update.
`late=0` alone is not proof of meeting all hardware deadlines: the other graph
nodes, DMA service, interrupt latency, and audible behavior matter too. The pool
peak covers Audio blocks, not the DSP's static arrays. Counters are cumulative
since `r`; do not sum the reported cumulative counts across CSV rows.

A useful first acceptance target is no allocation failures/missing inputs or
clipped samples, no audible dropouts, zero measured late callbacks, and graph
peaks comfortably below 100%. Leave substantial headroom before adding UI,
recording, storage, or more voices. Record the observed margin rather than
claiming the target from a successful compilation.

## Static memory and current verification

`?` prints actual target `sizeof` values for the portable patch, node, shared
source, and each mode's state. All modes remain resident; selecting one does
not free memory. Per-mode state sizes exclude the shared source and Audio pool.
The firmware build's RAM1/RAM2 reports additionally include framework globals,
code placement/padding, USB buffers, and the reserved Audio pool. Its "free for
local variables" is a linker estimate, not measured stack headroom. No dynamic
heap/stack watermark probe is claimed by this lab.

The locally verified Teensy builds used Teensy platform 5.2.0 and Arduino Teensy
framework 1.162.0. The audio node is 32,744 bytes; the 32-block Audio pool is
8,320 bytes (260 bytes per block). These are compiled sizes, not runtime readings.
Final FLASH/RAM totals can change with compiler/framework versions; consult your
own `pio run` report. Both Teensy 4.0 and 4.1 builds succeeded. Native tests run
all modes, check finite/unclipped non-silent output, sample-rate preparation,
bypass identity, spectral latency, and hold/reset control.

No Teensy was attached during this implementation. Callback timings, audio-path
behavior, pool usage, and the results worksheet still require the board run.

## Files

- `examples/audio_lab/LabPatch.h`: portable composition used on host and board.
- `examples/audio_lab/audio_lab.ino`: codec/I2S wiring, callback, commands, reports.
- `tests/audio_lab.cpp`: portable composition checks.

The integration follows the neighboring `silt`/`tide-engine` separation of DSP,
Teensy Audio wiring, and native tests. Existing tms headers supply the algorithms;
this step adds an adapter and measurement procedure, not another DSP abstraction.
