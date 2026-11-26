# Limitations, Threats to Validity & Real-vs-Metaphor Boundary (`LIMITATIONS.md`)

`noetic-organism-omega v2.1.0` enforces strict epistemic hygiene. Every limitation and boundary below is an explicit constraint on how results may be interpreted.

## 1. Real-vs-Metaphor Boundary (CRITICAL)

- **Zero Consciousness or Sentience Claims:** All 14 Butlin indicator checks (`L1_BUTLIN14`), all 10 consciousness-framework checks (`L2_FRAMEWORKS10`), and 5 mirrored CQM items (`CQM13, CQM19..22`) are strictly **`EXECUTED_PASS_INDICATOR_ONLY`** computational/architectural proxies. Passing these structural lesion tests demonstrates that specific C11 state equations causally drive specific behavioral readouts inside a 6,776-byte simulation struct—**nothing more**. They provide **zero** evidence of subjective qualia, phenomenal experience, sentience, or moral patienthood.
- **Biological Metaphors Are Compact Toy Equations:** Terms such as *sLTC cortex*, *apical dCaAP XOR binding*, *global workspace ignition*, *REM reality monitoring*, *XPBD hydrostatic coelom*, *Gánti Chemoton*, *Blackiston metamorphosis*, and *autopoietic tissue repair* denote deterministic low-dimensional numerical models (16-unit cortex, 16 patches, 4 peers), not wet-lab neurobiology, real cellular biochemistry, or insect neuroanatomy.
- **Mega-Atlas Non-Retirement:** **`0 / 1,900`** overarching problems in `0thernes/unsolved-problems-mega-atlas` are solved or retired (`docs/MEGA_ATLAS_CROSSCHECK.md`).

## 2. Formal-to-C11 Verification Boundary

- **Not CompCert / VST Verified:** The 54 Lean 4.35.0 theorems (`lean/NoeticProofs.lean`) formally verify discrete mathematical specifications (`Ledger10`, `ring0ApplyEdit`, `packB158`, `sum1To`, bitmask/memory bounds) and concrete integer/boolean constants exported by a live C11 run (`NoeticProofs/LiveC11Receipt.lean`, audited by `N24`, `N32`, `N48`). Lean does **not** verify the C11 source files (`src/*.c`) or Clang/GCC machine-code emission.
- **Unbounded `Int` vs. C11 `int64_t`:** Lean's `Ledger10` uses arbitrary-precision `Int`, whereas C11 uses signed 64-bit integers (`int64_t`). Overflow avoidance in C11 relies on bounded initial pool magnitudes ($\sim 10^7~\text{uq} \ll 9.2 \times 10^{18}$) and runtime validation rather than a Lean `Int64` overflow proof.
- **IEEE-754 Transcendentals:** `noetic_exp_det`, `noetic_log_det`, and `noetic_tanh_det` are verified empirically against IEEE-754 reference values (`max_rel_err = 2.175e-16` in `HW15`), not formally proved in Lean/Flocq.

## 3. Statistical & Experimental Limitations

- **Deterministic Low-Variance $d_z$ Inflation:** In deterministic mechanistic suites (`L8_SOFT40`, `L4_FREAK100`, `L5_LIFE500`), paired intact-vs-knockout differences across locked seeds frequently exhibit near-zero variance, producing very large Cohen's $d_z$ values ($> 50$ or capped at `50.00`). These values indicate clean deterministic separation under controlled software perturbations and must not be compared naively to noisy wet-lab biological effect sizes.
- **Honest Classical Parity Ties:** Five quantum-inspired checks (`F-092`, `N01`, `N02`, `N50`, `LIFE-L040-A5`) show exact `0.00` lift over matched-parameter classical controls (`p_Holm = 1.00`). No quantum computational advantage is claimed anywhere in this repository.
- **Single-Host Windows Multi-Kernel Constraint:** Locally on Windows, `lean4lean.exe` cannot parse Windows `.olean` path headers and `lake check` / `lake comparator` require Linux `bubblewrap` (`bwrap`). Full 6/6 kernel execution (adding `lean4lean` + `lake comparator --paranoid`) runs on `ubuntu-latest` in `.github/workflows/ci.yml`.
- **Single-Author / AI-Assisted Provenance & Peer Review:** This is an independent, AI-assisted research artifact and has **not** undergone external peer review or third-party independent replication yet (`REPLICATION.md`).
