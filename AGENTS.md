# AGENTS.md — Noetic Organism Omega (`v2.1.0`)

Canonical engineering, formalization, and epistemic instructions for AI coding agents and human contributors working in `noetic-organism-omega`. (`CLAUDE.md`, `GEMINI.md`, and `.cursor/rules/build-standard.mdc` point directly to this file.)

## Repository Identity & Role

- **Project:** `noetic-organism-omega` (`v2.1.0`) — Tier 3/4 whole-organism cognitive, soft-body homeostatic, and full 9-suite 825-check Clean-Slate v3.0 engine + 54-theorem 6-kernel Palomar Lean 4.35.0 suite.
- **Author:** Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA (ORCID `0009-0006-8168-9648`, `0_0@0thernes.art`). Never attribute an unformed LLC entity.
- **Dual Open License:** `Apache-2.0` (`LICENSE`) for code, Lean proofs, tests, and scripts; `CC-BY-4.0` (`LICENSE-DOCS`) for white paper, PDFs, documentation, and result datasets.
- **Verified Baseline (`v2.1.0`):** `10/10` unit+mutants (`5/5` Anti-LARP caught), `13/13` Clean-Slate v3.0 release gates, `825/825` C11 checks (`0` fail, `779` pass, `29` `[INDICATOR-ONLY]`, `12` native gate pass, `5` `PASS_CLASSICAL_PARITY_DISCLOSED`), `100/100` milestones (`First-15 LOCKED_PASS`), `54/54` Pure Lean 4.35.0 theorems (`N01..N54`, 5 local Windows kernels / 6/6 Linux CI kernels), `0` TeX overflows, `22/22` live-verified citations.

## Hard Rules (Build Standard v1.2.0 & Playbook v1.3)

1. **Direct-to-`main` Workflow (R1–R3):** Commit and push directly to `main`. No PRs, no feature branches, no commit/tag signing, no blocking hooks. CI is informational.
2. **Banned Files (R4):** Never create `.github/dependabot.yml`, `renovate.json`, PR templates, `.github/copilot-instructions.md`, or `FUNDING.yml`.
3. **Zero Fabrication (R6):** Never invent benchmark numbers, test passes, citations, DOIs, peer review, or sentience claims. Every number must come from a live run of current code.
4. **Epistemic Boundaries:** Always keep `L1_BUTLIN14`, `L2_FRAMEWORKS10`, and `CQM13/19..22` labeled `EXECUTED_PASS_INDICATOR_ONLY`, keep `F-092`, `N01`, `N02`, `N50`, `LIFE-L040-A5` labeled `PASS_CLASSICAL_PARITY_DISCLOSED`, and affirm `0 / 1,900` Mega-Atlas problems retired.
5. **10 Finding Classes (R7):** Classify every audit finding into exactly one of: `bug`, `error`, `issue`, `problem`, `concern`, `dilemma`, `conflict`, `gap`, `hole`, `blindspot`.
6. **PowerShell Bracket Safety:** When working under `Z:\[Vibe Coded (AI)]\ANTIGRAVITY\noetic-organism-omega`, always pass `-LiteralPath` to PowerShell file cmdlets.

## Core Commands

- **Build & Verify C11 + Suites + Lean (Windows):** `.\build.ps1`
- **Build & Verify C11 (Linux):** `./build_linux.sh`
- **Firewall, 825 Trace, Zero-Theater & Property Tests:**
  ```bash
  node scripts/seed_firewall.mjs
  node scripts/build_m8_e2_825_trace.mjs
  node scripts/audit_zero_theater.mjs
  node tests/noetic_prop.test.mjs
  ```
- **Lean 4.35.0 Palomar Multi-Kernel Audit:**
  ```bash
  cd lean && lake build && lake build Challenge
  lake env lean VerifyAxioms.lean > ../results/lean4-noetic-audit.txt
  lake env leanchecker --fresh Solution
  lake env leanchecker-paranoid --fresh Solution
  # On Linux with bubblewrap:
  lake check --paranoid && lake comparator --config comparator.json --paranoid
  ```
- **Axioms, Citations, Paper & Repo Self-Audit:**
  ```bash
  python scripts/check_readme_axioms.py
  python scripts/check_citations.py
  python scripts/build_paper.py
  pwsh -NoProfile -File scripts/Audit-Repo.ps1
  ```
- **Task Runner:** See `justfile` for one-word targets (`just reproduce`, `just test`, `just lean`, `just paper`, `just audit`).
