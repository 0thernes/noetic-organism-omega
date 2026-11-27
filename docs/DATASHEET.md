# Datasheet for Datasets & Verification Receipts (`docs/DATASHEET.md`)

Following Gebru et al. (2021), *Datasheets for Datasets*, this datasheet documents the machine-generated verification receipts, tabular evaluation sweeps, and benchmark datasets produced by `noetic-organism-omega v2.2.0`.

## 1. Motivation

- **Purpose:** Provide transparent, deterministic, audit-ready empirical receipts for all 825 Clean-Slate v3.0 checks, 100 bottom-up milestones, 13 release gates, 18 structural circuit ablations, 49 cognitive/CQM battery evaluations, 6-tier scaling benchmarks, 22-entry live citation audit, and 68-theorem Lean 4.35.0 axiom & multi-kernel transcripts.
- **Creator:** Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA (ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648)).
- **Funding:** Self-funded independent research; zero external grants or commercial sponsors.

## 2. Composition

- **Instances:**
  - `results/m8_e2_825_honest_trace_matrix.csv` (825 rows x 12 columns): every Clean-Slate v3.0 check across 9 suites (`L1_BUTLIN14`, `L2_FRAMEWORKS10`, `L3_CQM25`, `L4_FREAK100`, `L5_LIFE500`, `L6_NOVEL100`, `L7_WSE12`, `L8_SOFT40`, `L9_HW24`), explicitly recording status (`779 EXECUTED_PASS`, `29 EXECUTED_PASS_INDICATOR_ONLY`, `12 NATIVE_RELEASE_GATE_PASS`, `5 PASS_CLASSICAL_PARITY_DISCLOSED`, `0 EXECUTED_FAIL`), mirror tags (`MIR-01..05`), paired `d_z`, and Holm-adjusted `p`-values.
  - `results/m8_e1_ablation_sweep.{json,csv}` (18 rows): single-circuit structural lesion sweeps over `N=48` locked seeds (`5001..5024` + `5101..5124`).
  - `results/m8_e2_cognitive_49.{json,csv}` (49 rows): 5-arm (`INTACT`, `SHAM`, `LESION`, `SHUFFLED`, `PASSIVE`) battery receipts.
  - `results/m8_e3_scaling_bench.{json,csv}` & `results/m8_e3_scaling_linux.{json,csv}` (6 rows): single-thread cache-tier throughput benchmarks (`N = 1..4096`).
  - `results/m9_native_release_gates*.json`, `results/m10_*_receipt.json`, `results/seed_firewall_ledger.json`: native release gate, 9-suite, 100-milestone, and hash-chained Seed Firewall receipts.
  - `results/lean4-noetic-audit.txt`, `results/lean4-multi-kernel.txt`, `results/citation-audit.json`: Lean 4.35.0 68-theorem axiom/kernel transcripts and live Crossref/arXiv citation alignment log.
- **Personal / Sensitive Data:** **None.** All records are synthetic numerical outputs of a deterministic C11 simulator and formal proof checker. Zero human subjects, zero PII, zero confidential data.
- **Splits:** Strictly enforced by `GATE-01` (`scripts/seed_firewall.mjs`): Dev tuning seeds `1001..1099`, Confirmatory seeds `5001..5024`, Held-Out seeds `5101..5124` (0 overlap, burn-on-fail verified).

## 3. Collection Process

- **Generation:** Deterministically generated from compiled C11 binaries (`build.ps1` Win64 Clang 23 and `build_linux.sh` Linux GCC 13.3, both `-std=c11 -O3 -ffp-contract=off -Wall -Wextra -Werror`) and `lake env lean VerifyAxioms.lean`.
- **Cross-OS Determinism:** All 8 core transcendental functions (`exp`, `log`, `log2`, `tanh`, `sin`, `cos`, `atan2`, `hypot`) use pure IEEE-754 portable implementations in `src/noetic_rng.c` (`max_rel_err = 2.274e-16`), yielding 0-bit identical SHA-256 world states and 0-byte `diff` on `LiveC11Receipt.lean` across Windows and Linux.

## 4. Preprocessing, Cleaning & Labeling

- **Zero Filtering / Zero Cherry-Picking:** Every check and equal-parameter tie (`5` quantum-vs-classical parity ties) is retained verbatim in the trace matrix.
- **Indicator Labeling:** All 29 consciousness-related proxies (`L1_BUTLIN14`, `L2_FRAMEWORKS10`, `CQM13/19..22`) are tagged `EXECUTED_PASS_INDICATOR_ONLY`.

## 5. Uses & Prohibitions

- **Recommended Uses:** Independent verification, cross-OS floating-point determinism auditing, ablation-harness benchmarking, and Lean 4.35.0 Palomar multi-kernel replay.
- **Out-of-Scope / Prohibited Interpretations:** Do **not** cite `EXECUTED_PASS_INDICATOR_ONLY` scores as evidence of phenomenal consciousness, subjective qualia, sentience, or moral patienthood, and do **not** claim retirement of any overarching open problem in the Unsolved Problems Mega-Atlas (`0 / 1,900` retired).

## 6. Distribution & Maintenance

- **Distribution:** GitHub (`https://github.com/0thernes/noetic-organism-omega`), GitHub Pages (`https://0thernes.github.io/noetic-organism-omega/`), Software Heritage (`swh:1:snp:b6ab6984080956100c967e802c6fb44e0da7ff52`), with MLCommons Croissant (`croissant.json`), RO-Crate (`ro-crate-metadata.json`), and CodeMeta (`codemeta.json`) descriptors.
- **Integrity:** LF-normalized `CHECKSUMS.sha256` & `data/MANIFEST.sha256` bound by DigiCert RFC 3161 (`CHECKSUMS.sha256.tsr`) and 3-calendar Bitcoin OpenTimestamps (`CHECKSUMS.sha256.ots`).
- **Licensing:** Datasets, receipts, figures, documentation, and white paper are licensed under **CC-BY-4.0** (`LICENSE-DOCS`); code, scripts, and Lean 4 proofs are licensed under **Apache-2.0** (`LICENSE`).
- **Maintainer:** Alexander Stephen Donahue (`0_0@0thernes.art`). Errata are logged chronologically in `ERRATA.md`.
