# Noetic Organism Omega (`v2.2.0`)

Cross-OS bit-identical portable C11 whole-organism simulation engine with an 11-slot `int64_t` integer energy ledger, a 9-suite 825-check and 100-milestone Clean-Slate v3.0 test harness, and 68 Lean 4.35.0 theorems (`N01..N60` + `A01..A08`) covering unbounded-`Int` 10-field (`Ledger10`) and 11-slot (`Ledger11`) models, helper encodings, and 8 adversarial false-variant refutations. The Lean theorems are kernel-checked across 6 independent proof kernels; the C11 engine itself is tested, not formally proved.

[![Version](https://img.shields.io/badge/version-2.2.0-0284c7.svg)](VERSION)
[![Status](https://img.shields.io/badge/status-active-059669.svg)](ROADMAP.md)
[![C11 Checks](https://img.shields.io/badge/C11_checks-825%2F825_0_fail-059669.svg)](results/m8_e2_825_honest_trace_summary.json)
[![Lean 4.35.0](https://img.shields.io/badge/Lean_4.35.0-68%2F68_6--kernel_Palomar-7c3aed.svg)](lean/NoeticProofs.lean)
[![Cross-OS Parity](https://img.shields.io/badge/Win64_==_Linux-0--bit_SHA256-0284c7.svg)](lean/NoeticProofs/LiveC11Receipt.lean)
[![License: Code](https://img.shields.io/badge/code-Apache--2.0-blue.svg)](LICENSE)
[![License: Docs](https://img.shields.io/badge/docs%20%26%20paper-CC--BY--4.0-orange.svg)](LICENSE-DOCS)

status: active

![Noetic Organism Omega v2.2.0 Overview](docs/media/social-preview.svg)

- **Author:** Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA (ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648))
- **Repository:** <https://github.com/0thernes/noetic-organism-omega>
- **Interactive Proof Blueprint & Pages Site:** <https://0thernes.github.io/noetic-organism-omega/> · [Interactive Lean 4 Blueprint (`docs/blueprint.html`)](https://0thernes.github.io/noetic-organism-omega/blueprint.html)
- **White Paper Preprint (Not Peer-Reviewed):** [`dist/Donahue_2026_noetic_organism_omega.pdf`](dist/Donahue_2026_noetic_organism_omega.pdf) (11 pages, 0 overfull hboxes) · LaTeX source [`paper/main.tex`](paper/main.tex) · Clean-room arXiv tarball [`dist/arxiv_submission.tar.gz`](dist/arxiv_submission.tar.gz)
- **Archival Provenance & Dual Timestamps:** Software Heritage [`swh:1:snp:b6ab6984080956100c967e802c6fb44e0da7ff52`](https://archive.softwareheritage.org/swh:1:snp:b6ab6984080956100c967e802c6fb44e0da7ff52) · SLSA/Rekor Provenance (`actions/attest-build-provenance@v2`) · DigiCert RFC 3161 (`CHECKSUMS.sha256.tsr`) + 3-Calendar Bitcoin OpenTimestamps (`CHECKSUMS.sha256.ots`)
- **Epistemic Notice:** All 14 Butlin indicators (`L1_BUTLIN14`), 10 consciousness-framework checks (`L2_FRAMEWORKS10`), and 5 mirrored CQM items (`CQM13, CQM19..22`) are strictly **`[INDICATOR-ONLY]`** computational/architectural proxies. This project makes **zero** claims of subjective qualia, phenomenal consciousness, or sentience, and retires **0 / 1,900** overarching problems in the Unsolved Problems Mega-Atlas (`docs/MEGA_ATLAS_CROSSCHECK.md`).

---

## Verified Results (`v2.2.0` Cross-OS Bit-Identical Win64 Clang == Linux GCC & Lean 4.35.0)

| Verification Layer | Result | Honesty & Forensic Safeguards |
|---|---:|---|
| **Cross-OS Determinism (`Win64 == Linux`)** | **0-Bit Identical (`0 B diff`)** | 8 portable IEEE-754 math funcs (`max_rel_err=2.274e-16`), `-ffp-contract=off`, LF binary export; Seed 5001 180t SHA-256 `6c0ca90e4090aac8...ed56d2c`, 10k-tick `GATE-11` `c7a53fd5...3f75`; CI enforces `diff -u` on `LiveC11Receipt.lean` |
| **M7 Unit & Anti-LARP Mutants** | **10 / 10 PASS** (`5/5` mutants caught) | Leaky `+1 uq`, cosmetic label, oracle sensor-bypass, frozen plasticity, shuffled stream all caught |
| **Clean-Slate v3.0 Release Gates** | **13 / 13 PASS** (`GATE-01..13`) | Cryptographic Seed Firewall (`GATE-01`), 10k zero-alloc (`GATE-02`), Antithetic+3-phase fasting `2.43x` (`GATE-05`), XPBD `0.000304%` (`GATE-06`), b1.58 ganglia `84.13%` offload (`GATE-07`), Read-only ACL `10/10` + WSE `10/10` (`GATE-09`), Blackiston `87.50%` remap (`GATE-10`), 10k 0-bit Observer Independence (`GATE-11`) |
| **Suite `L1_BUTLIN14`** | **14 / 14 PASS** | Explicitly tagged `EXECUTED_PASS_INDICATOR_ONLY` (ADR 0018 Rule-4 & Rule-6; never claims qualia) |
| **Suite `L2_FRAMEWORKS10`** | **10 / 10 PASS** | Explicitly tagged `EXECUTED_PASS_INDICATOR_ONLY` (ADR 0018 Rule-4 & Rule-6) |
| **Suite `L3_CQM25`** | **25 / 25 PASS** (`5/5` Mirrors) | `20` `EXECUTED_PASS` + `5` `EXECUTED_PASS_INDICATOR_ONLY` (`CQM13, CQM19..22`); exact `N=48` locked seeds, 5 arms, Holm FWER |
| **Suite `L4_FREAK100`** | **100 / 100 (`0` FAIL)** | `99` `EXECUTED_PASS` + `1` `PASS_CLASSICAL_PARITY_DISCLOSED` (`F-092` matched Kuramoto clock) |
| **Suite `L5_LIFE500`** | **500 / 500 (`0` FAIL)** | `499` `EXECUTED_PASS` + `1` `PASS_CLASSICAL_PARITY_DISCLOSED` (`LIFE-L040-A5`); `0 uq` conservation drift |
| **Suite `L6_NOVEL100`** | **100 / 100 (`0` FAIL)** | `97` `EXECUTED_PASS` + `3` `PASS_CLASSICAL_PARITY_DISCLOSED` (`N01, N02, N50`) |
| **Suite `L7_WSE12`** | **12 / 12 PASS** | `NATIVE_RELEASE_GATE_PASS` (`GATE-09`: `10/10` protected edits blocked, `10/10` WSE ops, `||W||_2=0.8443 <= 0.99`, `2,464 B < 16 KB` diff, 0-bit rollback) |
| **Suite `L8_SOFT40`** | **40 / 40 PASS** | `N=48` locked seeds, Holm FWER `m=40`, `0 uq` conservation drift |
| **Suite `L9_HW24`** | **24 / 24 PASS** | Flat SoA (`6,776 B`), 10k zero-alloc, 5-trit/B b1.58, `Q<=10`, 5-gear, implicit stiff `100x Km`, 8 portable IEEE-754 funcs (`2.274e-16`), 8-tuple Philox isolation, `0 uq` drift |
| **Total Master Spec Checks** | **825 / 825 EXECUTED (`0` FAIL)** | `779` `EXECUTED_PASS` + `29` `INDICATOR_ONLY` + `5` `PASS_CLASSICAL_PARITY_DISCLOSED` + `12` `NATIVE_RELEASE_GATE_PASS` (`820` unique + `5` mirrors) |
| **Bottom-Up Acyclic Milestones** | **100 / 100 VERIFIED (`M001..M100`)** | Stages `0..8` live DAG; First-15 Playable (`M001..M015`) = `LOCKED_PASS`; `0 uq` drift |
| **Pure Lean 4.35.0 Palomar Proofs** | **68 / 68 PROVEN** (Lean models only) | **46** parametric `Ledger10` (10-field) & `Ledger11` (11-slot unbounded-`Int`), encoding, strict `1.6x` ratio, non-vacuity, and 8 adversarial false-variant refutation theorems (`AdversarialTests.lean`: `A01..A08`); **18** closed numeral identities explicitly labeled as bookkeeping arithmetic; **4** live C11 receipt constant `decide`-checks (`N24`, `N32`, `N48`, `N59`). Strictly within `{propext, Classical.choice, Quot.sound}` (`40` zero-axiom, `27` `[propext/Quot.sound]`, `1` `Classical.choice`); `0 sorry`, `0 warnings`; live negative elaboration + `sorry`-tamper `lake comparator` rejection (`scripts/test_lean_negative.py`) |
| **Live Citation Audit** | **22 / 22 PASS** | Reachability + Crossref/arXiv title & first-author alignment logged in `results/citation-audit.json` |

---

## Quickstart

### Windows (Win64 Clang + Full 825-Check Battery + 68 Pure Lean 4.35.0 Proofs)

```powershell
# Full build, 10/10 unit+mutants, 13/13 gates, 825/825 C11 checks, 100/100 milestones, 68 Lean 4.35.0 proofs
.\build.ps1
node .\scripts\seed_firewall.mjs
node .\scripts\build_m8_e2_825_trace.mjs
node .\scripts\audit_zero_theater.mjs
node .\tests\noetic_prop.test.mjs
python .\scripts\check_readme_axioms.py
python .\scripts\test_lean_negative.py
python .\scripts\check_blueprint.py
python .\scripts\stamp_timestamps.py --verify
```

### Linux (GCC C11 `-ffp-contract=off` + 6-Kernel Palomar Lean 4.35.0)

```bash
chmod +x build_linux.sh
./build_linux.sh
node tests/noetic_prop.test.mjs
cd lean && lake build && lake build Challenge && lake env lean VerifyAxioms.lean
lake comparator --config comparator.json --paranoid
cd .. && python3 scripts/test_lean_negative.py --comparator
```

---

## Usage Examples

```bash
# Run the quick intact-vs-lesion structural ablation demo
node examples/run_quick_demo.mjs

# Run individual C11 engine subcommands
./bin/noetic_engine_linux test
./bin/noetic_engine_linux gates results/m9_native_release_gates.json
./bin/noetic_engine_linux export-lean 5001 lean/NoeticProofs/LiveC11Receipt.lean

# Launch the loopback-only interactive Web Observatory (http://127.0.0.1:8795/)
node web/server.mjs

# Verify all 22 live citations (reachability + Crossref/arXiv title & author alignment) & compile PDF
python scripts/check_citations.py
python scripts/build_paper.py
```

---

## Verbatim Lean 4.35.0 `#print axioms` 5-Second Proof Audit (68 / 68 Theorems)

Generated directly by `cd lean && lake env lean VerifyAxioms.lean` (`results/lean4-noetic-audit.txt`), verified drift-free by `scripts/check_readme_axioms.py`, and re-checked across independent kernels (`leanchecker --fresh`, `leanchecker-paranoid --fresh`, `con-leche --verified`, `con-ron --verified`, `nanoda_bin` locally in `results/lean4-multi-kernel.txt`; plus `lean4lean`, `lake check --paranoid`, `lake comparator --config comparator.json --paranoid`, and `scripts/test_lean_negative.py --comparator` on Linux CI):

<!-- LEAN_AXIOMS_START -->
```text
'NoeticProofs.N01_atp_to_heat_conserves_sum' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N02_atp_to_heat_monotone_sink' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N03_atp_to_heat_preserves_nonneg' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N04_patch_to_gut_conserves_sum' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N05_workspace_billing_conserves_sum' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N06_workspace_billing_monotone_heat' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N07_autopoietic_repair_conserves_sum' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N08_solar_inflow_exact_residual' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N09_closed_tick_exact_conservation' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N10_leaky_mutant_nonzero_residual' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N11_forked_pci_observer_noninterference' does not depend on any axioms
'NoeticProofs.N12_master_825_dedup_exact' does not depend on any axioms
'NoeticProofs.N13_cognitive_49_dedup_exact' does not depend on any axioms
'NoeticProofs.N14_mirror_exact_parity' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N15_wilcoxon_rank_sum_identity' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N16_wilcoxon_n48_exact' does not depend on any axioms
'NoeticProofs.N17_holm_bonferroni_inflation_bound' depends on axioms: [propext]
'NoeticProofs.N18_non_ceiling_strictly_below_max' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N19_sham_noninferiority_symmetric' does not depend on any axioms
'NoeticProofs.N20_seed_firewall_disjoint' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N21_anti_oracle_separation' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N22_ablation_mask_18_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
'NoeticProofs.N23_ecosystem_186_pure_lean_theorems' does not depend on any axioms
'NoeticProofs.N24_live_c11_receipt_verified' does not depend on any axioms
'NoeticProofs.N25_ring0_acl_blocks_protected_regions' does not depend on any axioms
'NoeticProofs.N26_wse_rollback_involution' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N27_wse_diff_budget_bounded' does not depend on any axioms
'NoeticProofs.N28_xpbd_area_constraint_bounded' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N29_packed_b158_2bit_roundtrip' does not depend on any axioms
'NoeticProofs.N30_fasting_protein_sparing_ratio' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N31_ecosystem_194_pure_lean_theorems' does not depend on any axioms
'NoeticProofs.N32_release_gates_live_c11_verified' does not depend on any axioms
'NoeticProofs.N33_quantum_cap_10_bytes_le_16384' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N34_five_gear_60s_exact_divisors' does not depend on any axioms
'NoeticProofs.N35_philox_key_domain_distinct' does not depend on any axioms
'NoeticProofs.N36_carcass_lysis_exact_conservation' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N37_yolk_cleavage_exact_conservation' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N38_b158_5trit_radix_fits_u8' does not depend on any axioms
'NoeticProofs.N39_xls_shard_below_excel_limit' does not depend on any axioms
'NoeticProofs.N40_suite_825_exact_partition' does not depend on any axioms
'NoeticProofs.N41_suite_825_minus_5_mirrors' does not depend on any axioms
'NoeticProofs.N42_milestones_100_stage_partition' does not depend on any axioms
'NoeticProofs.N43_first15_plus_85_eq_100' does not depend on any axioms
'NoeticProofs.N44_hybrid_cube_1000_exact' does not depend on any axioms
'NoeticProofs.N45_substrates_75_exact' does not depend on any axioms
'NoeticProofs.N46_compiled_mutants_35_exact' does not depend on any axioms
'NoeticProofs.N47_ecosystem_224_pure_lean_theorems' does not depend on any axioms
'NoeticProofs.N48_live_c11_full_825_and_100m_verified' does not depend on any axioms
'NoeticProofs.N49_nonvacuous_closed_tick_witness' does not depend on any axioms
'NoeticProofs.N50_nonvacuous_ring0_vs_user_boundary' does not depend on any axioms
'NoeticProofs.N51_refute_ring0_blocks_all_regions' does not depend on any axioms
'NoeticProofs.N52_refute_quantum_cap_11_fits_16384' does not depend on any axioms
'NoeticProofs.N53_nonvacuous_seed_firewall_and_holm_witness' does not depend on any axioms
'NoeticProofs.N54_refute_leaky_mutant_conserves_sum' does not depend on any axioms
'NoeticProofs.N55_ledger11_solar_inflow_preserves_residual' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N56_ledger11_open_tick_preserves_residual' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N57_ledger11_open_tick_monotone_heat' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N58_ledger11_open_tick_preserves_nonneg' depends on axioms: [propext, Quot.sound]
'NoeticProofs.N59_ledger11_live_receipt_zero_residual' does not depend on any axioms
'NoeticProofs.N60_fasting_ratio_equivalence_and_strict' depends on axioms: [propext, Quot.sound]
'NoeticProofs.A01_refute_unguarded_transfer_monotone_heat' does not depend on any axioms
'NoeticProofs.A02_refute_unguarded_transfer_nonneg' does not depend on any axioms
'NoeticProofs.A03_refute_region4_immutable' does not depend on any axioms
'NoeticProofs.A04_refute_unrestricted_b158_roundtrip' does not depend on any axioms
'NoeticProofs.A05_refute_bad_ledger11_transfer_conserves' does not depend on any axioms
'NoeticProofs.A06_refute_zero_seed_valid' does not depend on any axioms
'NoeticProofs.A07_refute_solar_without_p10_preserves_residual' does not depend on any axioms
'NoeticProofs.A08_refute_negative_score_nonneg' does not depend on any axioms
```
<!-- LEAN_AXIOMS_END -->

---

## Honest Verification Boundaries & Mega-Atlas Non-Retirement

- **What is kernel-checked (`lean/`):** Statements about unbounded-`Int` Lean models in `lean/NoeticProofs/Spec.lean`, not about `src/*.c`. Both the 10-field `Ledger10` (`N01..N10`, `N49`, `N54`) and the full 11-slot `Ledger11` (`p0..p10` + `initialTotal`, `N55..N59`) conserve their respective sum/residual under guarded transfers and exogenous solar inflow, never lower the heat field `p8`, and preserve non-negativity across all slots. `N25`/`N50`/`N51`/`A03` establish the abstract `region < 4` guard identity and its tight boundary at `region = 4` (not a hardware ACL proof); `N26` proves `(w + d) - d = w`; `N29` and `A04` prove exact 2-bit `b1.58` roundtrip on `{-1, 0, 1}` and its failure at `w = 2`; `N60` proves exact `1.6x` fasting ratio equivalence (`8a <= 5b <-> 16a <= 10b`) and strict `a < b` for `a > 0`; `A01..A08` in `lean/AdversarialTests.lean` refute 8 unguarded/broken model variants. **18** theorems (`N12`, `N13`, `N16`, `N23`, `N27`, `N31`, `N34`, `N38..N47`, `N52`) are closed numeral identities explicitly labeled as bookkeeping arithmetic that certify nothing about the engine, and **4** (`N24`, `N32`, `N48`, `N59`) `decide`-check constants exported by one C11 run (see [`ERRATA.md`](ERRATA.md) & [`docs/statement-fidelity.md`](docs/statement-fidelity.md)).
- **What is empirically & program-audited (`src/`, `tests/`, `scripts/`):** C11 runtime execution (825/825 checks, 100/100 milestones, 13/13 gates, 5/5 Anti-LARP mutants, 32-trial `prop_check`, ASan/UBSan & `libFuzzer` harness, and cross-OS Win64 Clang == Linux GCC 0-bit SHA-256 receipt parity enforced via `diff -u` in CI). C11 source code and `int64_t` overflow bounds are **not** formally verified in CompCert/VST; the 11-slot `int64_t` runtime behavior, `||W||_2 <= 0.99` spectral-norm bound, and 825-check results rest on these runtime tests.
- **Mega-Atlas Cross-Check (`docs/MEGA_ATLAS_CROSSCHECK.md`):** **`0 / 1,900`** overarching open problems in `0thernes/unsolved-problems-mega-atlas` are solved or retired. Eight probed IDs (`NEURO-001`, `NEURO-004`, `NEURO-024`, `SUB-007`, `SYS-009`, `SYS-050`, `AI-001`, `AI-018`) remain strictly **OPEN**.

---

## AI-Contribution Disclosure, CRediT Authorship & Licensing

- **AI-Assistance Disclosure:** This project was developed through AI-assisted research, C11/Python/JS engineering, Lean 4.35.0 formalization, and technical writing using frontier LLM coding assistants (Claude and Gemini agentic workflows) under the sole human direction and mechanical verification of Alexander Stephen Donahue. Full tool-by-tool disclosure is recorded in [`docs/AI_DISCLOSURE.md`](docs/AI_DISCLOSURE.md). AI tools are never listed as authors.
- **CRediT Author Statement:** **Alexander Stephen Donahue** (Independent Researcher, Spokane, WA, USA; ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648)) — *Conceptualization, Methodology, Supervision, Validation, Writing — review & editing*.
- **Peer-Review Status:** Independent technical research preprint; **not peer-reviewed**.
- **Licensing & FAIR Metadata:** Dual open license — **Apache-2.0** (`LICENSE`, `LICENSES/Apache-2.0.txt`) for code, Lean proofs, tests, and scripts; **CC-BY-4.0** (`LICENSE-DOCS`, `LICENSES/CC-BY-4.0.txt`) for white paper, PDFs, figures, documentation, and result datasets, annotated via [`REUSE.toml`](REUSE.toml). Machine-readable FAIR descriptors: [`croissant.json`](croissant.json), [`ro-crate-metadata.json`](ro-crate-metadata.json), [`codemeta.json`](codemeta.json), [`docs/DATASHEET.md`](docs/DATASHEET.md), and [`SUBMISSION_READINESS.md`](SUBMISSION_READINESS.md).
