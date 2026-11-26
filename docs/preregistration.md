# Pre-Registration of Experimental Hypotheses & Statistical Protocol (`docs/preregistration.md`)

This document records the locked experimental design, seed firewall partitions, statistical tests, and non-inflation policies governing `noetic-organism-omega v2.1.0` (`B32`).

## 1. Locked Seed Partitions (`GATE-01` Seed Firewall)

All stochastic draws derive from deterministic 8-tuple `Philox4x32-10` streams partitioned strictly into three disjoint integer blocks enforced by `scripts/seed_firewall.mjs` and formalized in `lean/NoeticProofs.lean` (`N20`, `N53`):

- **Development / Tuning Seeds (Excluded from Reporting):** `1..99` (Lean unit range) and `1001..1099` (`99` seeds in `GATE-01`).
- **Locked Confirmatory Reporting Block:** `5001..5024` (`24` seeds).
- **Locked Held-Out Reporting Block:** `5101..5124` (`24` seeds).
- **Total Paired Evaluation Sample Size:** $N = 48$ locked seeds per arm, per check.
- **Burn-on-Fail Rule:** Any confirmatory or held-out seed that fails an evaluation gate is permanently burned in the SHA-256 hash-chained ledger and barred from re-entry.

## 2. Five Paired Experimental Arms & Zero-Theater Constraint

Every item in `M8 E2` (`L1_BUTLIN14`, `L2_FRAMEWORKS10`, `L3_CQM25`) is evaluated across five paired arms sharing the exact same locked seed vector:
1. `NOETIC_ARM_INTACT` — All 18 structural circuits active.
2. `NOETIC_ARM_SHAM` — Size-matched benign scratchpad perturbation; expected non-inferiority (`|intact - sham| <= 0.120`, `N19`).
3. `NOETIC_ARM_LESION` — Targeted bit-masked structural circuit ablation (`ABL_MASK(...)`).
4. `NOETIC_ARM_SHUFFLED` — Temporally permuted sensory ring buffer with brain intact (anti-oracle guard).
5. `NOETIC_ARM_PASSIVE` — Motor efferents zeroed (embodied sensorimotor closure control).

**Zero-Fanout Rule:** `readout_metric()` in `src/noetic_eval.c` MUST contain zero branches on `w->arm`, `NOETIC_ARM_*`, or `ablation_mask` (enforced by `scripts/audit_zero_theater.mjs`).

## 3. Primary Statistical Decision Rules

- **Nonparametric Test:** Exact dynamic-programming two-sided/one-sided Wilcoxon signed-rank test (`src/noetic_stats.c`, maximum rank sum $\sum_{k=1}^{48} k = 1{,}176$ proved in `N15..N16`, unit-verified at $N=10$ in `T05`).
- **Effect Sizes:** Paired standardized mean difference $d_z$ with deterministic Philox bootstrap 95% confidence intervals (`dz_ci_lo`, `dz_ci_hi`) and paired win-rate proportion `delta_pair`.
- **Multiple-Comparison Control:** Holm-Bonferroni step-down family-wise error rate adjustment (`p_Holm`, `N17`) across each suite family (`m = 14, 10, 25, 40, 100, 500`).
- **Non-Ceiling Guard:** Cognitive indicator readouts are guarded against trivial `1.000` ceiling saturation (`<= 0.950`, `N18`).
- **Policy P7 (Honest Classical Parity Disclosure):** Equal-parameter classical controls must NEVER be handicapped to manufacture quantum advantage; ties (`d_z = 0.00`, `p_Holm = 1.00`) are reported explicitly as `PASS_CLASSICAL_PARITY_DISCLOSED` (`F-092`, `N01`, `N02`, `N50`, `LIFE-L040-A5`).
- **ADR 0018 Rule-4 & Rule-6 (`[INDICATOR-ONLY]`):** All 29 consciousness-related checks (`L1_BUTLIN14`, `L2_FRAMEWORKS10`, `CQM13/19..22`) are pre-registered strictly as computational/architectural indicator proxies, never as tests of subjective experience.
