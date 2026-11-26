# Roadmap (`ROADMAP.md`)

Maintenance Status: **Active** (`v2.1.0`).

## Completed Milestones
- **`v1.0.0`:** Portable C11 whole-organism engine, 11-pool exact integer ledger, 18 structural circuit ablations, 5 Anti-LARP mutants, Web Observatory (`:8795`), 24 Lean 4 proofs.
- **`v1.1.0`:** Clean-Slate v3.0 native release gates (`GATE-02,05,06,07,09,10,11`), Ring-0 ACL + WSE-12, XPBD soft-body hydrostat, packed `b1.58` ganglia, Blackiston metamorphosis, 32 Lean 4 proofs.
- **`v2.0.0`:** Full native C11 execution of all 9 Clean-Slate v3.0 suites (`825/825`, 0 fail), `100/100` bottom-up milestones, AVX2/VNNI ThinLTO build, 48 Lean 4 proofs.
- **`v2.1.0` (Current):** 6-kernel Palomar Lean 4.35.0 (`54/54` theorems including 6 non-vacuity/refutation witnesses), Playbook v1.3 AMS-LaTeX white paper (`0` overflows, `22/22` live citations), dual Apache-2.0 / CC-BY-4.0 licenses, and 100.0% Build Standard v1.2.0 compliance.

## Planned Next Steps
1. **External Independent Replication (`v2.2.0`):** Solicit and record independent third-party clean-clone verification runs on Linux/macOS/Windows in `REPLICATION.md`.
2. **Palomar Registry Submission:** Submit pinned 40-character `v2.1.0` commit SHA to Palomar Registry once Linux CI 6/6 comparator log is archived.
3. **Software Heritage & Archival Snapshot:** Trigger Software Heritage Save Code Now snapshot and record release SWHID in `CITATION.cff` and `EVIDENCE.md`.
4. **Bounded-Integer `Int64` Formalization:** Extend `NoeticProofs.Spec` with explicit `Int64` bounds proofs matching C11 `int64_t` overflow limits.
5. **Continuous Fuzz Corpus Expansion:** Run extended `libFuzzer` campaigns on `tests/fuzz_noetic_input.c` under ASan/UBSan and archive regression seeds.
