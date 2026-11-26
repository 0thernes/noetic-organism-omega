# Verification Evidence Index (`EVIDENCE.md`)

Machine-readable receipts and formal verification transcripts backing every claim in `CLAIMS.md` and `paper/main.tex` (`v2.1.0`).

| Evidence Artifact | Producer | Summary Metric |
|---|---|---|
| `results/lean4-noetic-audit.txt` | `cd lean && lake env lean VerifyAxioms.lean` | `54/54` theorems strictly within `{propext, Classical.choice, Quot.sound}` (`31` zero-axiom, `0` warnings) |
| `results/lean4-multi-kernel.txt` | 5 local Windows Lean 4.35.0 checkers | `leanchecker`, `leanchecker-paranoid`, `leanexport` (`5,501,394 B`), `con-leche --verified` (`1,617` decls), `con-ron --verified` (`1,617` decls), `nanoda_bin` — ALL PASS |
| `results/m9_native_release_gates.json` | `noetic_engine gates` | `7/7` native gates PASS (`GATE-02,05,06,07,09,10,11`, `0 uq` drift, `GATE-11 SHA=eceedf4d...bce1`) |
| `results/seed_firewall_ledger.json` | `node scripts/seed_firewall.mjs` | `GATE-01` PASS: `9/9` live receipts SHA-256 bound, disjoint partitions, tamper rejection verified |
| `results/m8_e1_ablation_sweep.json` | `noetic_engine e1 48` | `18/18` structural circuit ablations `EFFECT` (`N=48`, Holm FWER, `0 uq` drift) |
| `results/m8_e2_cognitive_49.json` | `noetic_engine e2 48` | `49/49` PASS (`14` Butlin `[INDICATOR-ONLY]`, `10` Frameworks `[INDICATOR-ONLY]`, `25` CQM, `5/5` mirrors, `0 uq` drift) |
| `results/m8_e3_scaling_bench.json` | `noetic_engine e3` | `N=1..4096` (`6,776 B/org`), `196.2k -> 87.9k` org-ticks/s, `0 uq` drift across all 6 tiers |
| `results/m10_hw24_receipt.json` | `noetic_engine hw24` | `24/24` PASS (`0 uq` drift, `exp_det` `2.175e-16` max rel err, `x86_64-AVX2-native`) |
| `results/m10_soft40_receipt.json` | `noetic_engine soft40 48` | `40/40` PASS (`N=48`, Holm FWER `m=40`, `0 uq` drift) |
| `results/m10_freak100_receipt.json` | `noetic_engine freak100 48` | `99` PASS + `1` `PASS_CLASSICAL_PARITY_DISCLOSED` (`F-092`), `0` FAIL |
| `results/m10_novel100_receipt.json` | `noetic_engine novel100 48` | `97` PASS + `3` `PASS_CLASSICAL_PARITY_DISCLOSED` (`N01, N02, N50`), `0` FAIL |
| `results/m10_life500_receipt.json` | `noetic_engine life500 48` | `499` PASS + `1` `PASS_CLASSICAL_PARITY_DISCLOSED` (`LIFE-L040-A5`), `0` FAIL, `0 uq` drift |
| `results/m10_milestones_100_receipt.json` | `noetic_engine milestones100` | `100/100` VERIFIED (`M001..M100`, `First-15 = LOCKED_PASS`, `0 uq` drift) |
| `results/m8_e2_825_honest_trace_summary.json` | `node scripts/build_m8_e2_825_trace.mjs` | `825/825` natively executed (`779` pass, `29` indicator, `12` gate, `5` parity, `0` fail) |
| `dist/Donahue_2026_noetic_organism_omega.pdf` | `python scripts/build_paper.py` | Zero-overflow AMS-LaTeX PDF + clean-room verified `dist/arxiv_submission.tar.gz` |
| `CHECKSUMS.sha256` & `data/MANIFEST.sha256` | SHA-256 manifest generator | Cryptographic integrity digests of binaries, frozen PDFs, tarball, Lean proofs, and receipts |
