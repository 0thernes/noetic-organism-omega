# Honest Benchmarks & Performance Budgets (`docs/benchmarks.md`)

All timings below are **real, live measurements** captured from `bin/noetic_engine.exe e3` and `bin/noetic_engine.exe hw24` on Windows 11 x64 using Clang 23 (`-std=c11 -O3 -Wall -Wextra -Werror -march=native -mavx2 -mavxvnni -mbmi -mbmi2 -mpopcnt -ffp-contract=off -flto=thin`, `v2.1.0`).

## 1. Single-Thread Scaling & Cache Footprint (`M8 E3`)

Source artifact: [`results/m8_e3_scaling_bench.json`](../results/m8_e3_scaling_bench.json) (`sizeof(NoeticWorld) = 6,776 B`).

| Organisms ($N$) | Ticks ($T$) | Total Footprint | Cache Tier | Wall Time (ms) | Throughput (org-ticks/s) | Latency ($\mu\text{s}$/org-tick) | Max Energy Drift |
|---:|---:|---:|---|---:|---:|---:|---:|
| 1 | 2,000 | 6,776 B | `L1D_FIT` | 10.192 | 196,240.04 | 5.0958 | `0 uq` |
| 16 | 500 | 108,416 B | `L2_FIT` | 42.672 | 187,478.76 | 5.3339 | `0 uq` |
| 64 | 200 | 433,664 B | `L2_FIT` | 66.887 | 191,366.39 | 5.2256 | `0 uq` |
| 256 | 80 | 1,734,656 B | `L2_FIT` | 125.516 | 163,166.71 | 6.1287 | `0 uq` |
| 1,024 | 30 | 6,938,624 B | `L3_FIT` | 282.766 | 108,640.88 | 9.2046 | `0 uq` |
| 4,096 | 10 | 27,754,496 B | `L3_FIT` | 466.043 | 87,888.92 | 11.3780 | `0 uq` |

### Honest Caveats & Measurement Variance
- Earlier `v2.0.0` runs on an unloaded host measured `268.7k -> 189.4k` org-ticks/s; the `v2.1.0` run above (`196.2k -> 87.9k` org-ticks/s) ran concurrently alongside sibling repository compilation builds on the same workstation. We report the exact measured `v2.1.0` JSON receipt rather than cherry-picking peak unloaded figures.
- All runs are single-threaded (`N` organisms stepped sequentially in a flat contiguous array) so results reflect per-core throughput and cache-hierarchy behavior without multi-core synchronization noise.

## 2. Explicit Performance Budgets (Clean-Slate v3.0 `L9_HW24` & `GATE-02..11`)

| Budget Dimension | Target Cap | Measured (`v2.1.0`) | Status |
|---|---:|---:|---|
| Per-world static memory (`HW01` / `GATE-02`) | $\le 8{,}192~\text{B}$ | `6,776 B` | PASS |
| Hot-loop heap allocations over 10k ticks (`GATE-02` / `HW02`) | `0` | `0` | PASS |
| Single-organism Tier-L step budget (`HW23`) | $\le 16{,}700~\mu\text{s}$ (60 Hz) | `5.04 us` (`HW23`) / `5.10 us` (`E3`) | PASS ($>3{,}200\times$ margin) |
| Sparse CSR compression vs. dense (`HW03`) | $\le 6.0~\text{B/syn}$ | `5.316 B/syn` (`101.0x` save) | PASS |
| Packed `b1.58` ganglia synapse size & offload (`GATE-07`) | $\le 0.5~\text{B/syn}$, $\ge 50\%$ | `0.25 B/syn`, `84.13%` offload | PASS |
| 5-gear multirate scheduler speedup & flux error (`HW10`) | $\ge 5.0\times$, $< 1\%$ err | `7.52x`, `0.0032%` err | PASS |
| XPBD hydrostatic volume drift (`GATE-06`) | $< 1.0\%$ | `0.000304%` | PASS |
| WSE-12 serialized diff size & spectral norm (`GATE-09`) | $< 16{,}384~\text{B}$, $\|W\|_2 \le 0.99$ | `2,464 B`, `0.844337` | PASS |
| Telemetry frame sketch (`HW19`) | $\le 32{,}768~\text{B}$ | `776 B` | PASS |
| Portable deterministic math (`HW15`) | $< 10^{-10}$ rel err | `2.175e-16` max rel err | PASS |

## 3. Historical Tracking

CI uploads `results/m8_e3_scaling_bench.json` on every push to `main` as a report-only performance artifact (`B27`) so latency regressions are tracked without blocking commits (Build Standard Rule R3).
