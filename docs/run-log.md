# Verification Run Log (`docs/run-log.md`)

Chronological audit trail of verified local builds and hardening runs (`B40`).

## 2026-11-25 — `v2.1.0` Gold Standard Upgrade (Cycles 1–3)

- **Host:** Windows 11 x64 (`Z:\[Vibe Coded (AI)]\ANTIGRAVITY\noetic-organism-omega`), Clang 23 (`AVX2/AVX-VNNI/BMI2 + ThinLTO`), Lean `4.35.0` (`eb6cafb7`), Node `v25.8.2`, Python `3.13`, Tectonic `0.17.0`.
- **C11 Build & Full Battery (`.\build.ps1`):**
  - `10/10` M7 unit + 5/5 Anti-LARP mutants PASS (`T01 SHA-256 = 1fde9fe591dadd326d8a90ba011ead82447150ba227adc16995fe7b1e3a1d1ba`).
  - `7/7` native release gates (`GATE-02,05,06,07,09,10,11`, `GATE-11 SHA-256 = eceedf4debbe1b0676cb98c99063076f989ac561bb1afa5fdf02048a7241bce1`) + `GATE-01` Seed Firewall (`head = b646df6f51fbeae4e93b50c8800fee35d7d7927638f7842ba90b071460006c4e`) => `13/13` Clean-Slate v3.0 release gates PASS.
  - `825/825` C11 checks executed (`779` pass, `29` `[INDICATOR-ONLY]`, `12` native gate pass, `5` `PASS_CLASSICAL_PARITY_DISCLOSED`, `0` fail); `100/100` milestones (`First-15 = LOCKED_PASS`, `0 uq` drift).
  - `M8 E1` `18/18` ablations `EFFECT`; `M8 E2` `49/49` (`5/5` mirrors); `M8 E3` `196.24k -> 87.89k` org-ticks/s (`N=1..4096`, `0 uq` drift).
  - `tests/noetic_prop.test.mjs`: `32/32` `prop_check` trials (`96` frames) PASS; `examples/run_quick_demo.mjs`: PASS (`INTACT ws_fid=0.699` vs `LESION ws_fid=0.012`).
- **Lean 4.35.0 Palomar Multi-Kernel Replay (`results/lean4-multi-kernel.txt`):**
  - `lake build` (`NoeticProofs`, `Solution`): PASS (`0` warnings).
  - `lake build Challenge`: PASS (`54` intentional statement-only `sorry` declarations).
  - `VerifyAxioms.lean`: `54/54` strictly within `{propext, Classical.choice, Quot.sound}` (`31` zero-axiom, `22` `[propext/Quot.sound]`, `1` `[propext, Classical.choice, Quot.sound]`).
  - `leanchecker --fresh Solution`: PASS; `leanchecker-paranoid --fresh Solution`: PASS; `leanexport`: `5,501,394 B`; `con-leche --verified`: PASS (`1,617` decls); `con-ron --verified`: PASS (`1,617` decls); `nanoda_bin` (`unpermitted_axiom_hard_error=true`): PASS.
- **White Paper, Citations & Axioms Parity:**
  - `scripts/check_readme_axioms.py`: PASS (`54/54` exact verbatim match).
  - `scripts/check_citations.py`: PASS (`22/22` live URLs & DOIs verified).
  - `scripts/build_paper.py`: PASS (`0` Overfull `\hbox`, `0` undefined refs/citations, clean-room `dist/arxiv_submission.tar.gz` verified).

## 2026-11-22 — `v2.0.0` Full 9-Suite Release
- Commit `0c372fd`: Enabled Win64 AVX2/VNNI/BMI2 + ThinLTO and verified 825/825 C11 checks, 100/100 milestones, and 48/48 Pure Lean 4 theorems.
