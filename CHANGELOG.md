# Changelog

All notable changes to `noetic-organism-omega` are documented in this file.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and [Semantic Versioning](https://semver.org/).

## [2.1.0] - 2026-11-25

### Added
- **6-Kernel Palomar Lean 4.35.0 Formalization (`lean/`):** Added `lean-toolchain` (`v4.35.0`), `lakefile.toml` (`autoImplicit = false`, `relaxedAutoImplicit = false`, `linter.unusedVariables = true`, `linter.deprecated = true`), `lake-manifest.json`, Palomar `module` headers, isolated pure definitions `NoeticProofs/Spec.lean`, statement-only `Challenge.lean`, `Solution.lean`, `comparator.json`, `VerifyAxioms.lean`, `formalization.yaml`, and 6 explicit Non-Vacuity & Sharp Boundary Refutation theorems (`N49..N54`, bringing the suite to **54/54** pure theorems).
- **Multi-Kernel Verification Receipts:** Locally replayed and logged `leanchecker --fresh`, `leanchecker-paranoid --fresh`, `leanexport`, `con-leche --verified`, `con-ron --verified`, and `nanoda_bin` (`unpermitted_axiom_hard_error = true`) into `results/lean4-multi-kernel.txt`, plus 6/6 Linux `bubblewrap` `lake check --paranoid` + `lake comparator --config comparator.json --paranoid` in `.github/workflows/ci.yml`.
- **AMS-LaTeX White Paper (`paper/` & `dist/`):** Authored Playbook v1.3 white paper (`paper/main.tex`, `paper/refs.bib` with 22 live-verified primary citations, TikZ architecture diagram, PGFPlots scaling curve), frozen `dist/Donahue_2026_noetic_organism_omega.pdf`, deterministic clean-room verified `dist/arxiv_submission.tar.gz`, `scripts/build_paper.py`, `scripts/check_citations.py`, and `scripts/check_readme_axioms.py`.
- **Build Standard v1.2.0 Scaffolding (`100.0%` target):** Added dual open licenses (`LICENSE` Apache-2.0, `LICENSE-DOCS` CC-BY-4.0), `NOTICE`, `THIRD_PARTY_NOTICES.md`, `VERSION`, `CITATION.cff`, `CITATION.bib`, `.zenodo.json`, `CLAIMS.md`, `LIMITATIONS.md`, `REPLICATION.md`, `DEVIATIONS.md`, `ERRATA.md`, `EVIDENCE.md`, `HARDENING_LOG.md`, `ROADMAP.md`, `SECURITY.md`, `SUPPORT.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `justfile`, `repo.yaml`, `CHECKSUMS.sha256`, `data/MANIFEST.sha256`, `tests/noetic_prop.test.mjs`, `tests/fuzz_noetic_input.c`, `examples/run_quick_demo.mjs`, `.github/ISSUE_TEMPLATE/*`, `.github/labels.tsv`, and full `docs/` (including `statement-fidelity.md`, `MEGA_ATLAS_CROSSCHECK.md`, `AI_DISCLOSURE.md`, `benchmarks.md`, `preregistration.md`, `adr/`, `media/social-preview.svg`, and `reviews/2026-11-25-10-pass-red-team-audit.md`).
- **Observatory Hardening:** Added `/healthz` health check endpoint in `web/server.mjs` and WAI-ARIA landmarks/labels in `web/index.html`.

### Changed
- **Identity & Legal Hardening (Playbook Finding #1):** Removed all unformed LLC entity affiliation strings; standardized author affiliation to `Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA` (ORCID `0009-0006-8168-9648`).
- **Live C11 -> Lean Export:** Updated `noetic_export_lean_receipt()` in `src/noetic_eval.c` to emit Palomar `module` + `@[expose] public def` declarations in `lean/NoeticProofs/LiveC11Receipt.lean`.
- **Version:** Bumped `NOETIC_VERSION_STR` and build artifacts from `2.0.0` to `2.1.0`.

## [2.0.0] - 2026-11-22

### Added
- Full native C11 execution of all 9 Clean-Slate v3.0 suites (**825/825** checks, 0 failures, 29 `[INDICATOR-ONLY]`, 5 `PASS_CLASSICAL_PARITY_DISCLOSED`), 100/100 bottom-up milestones (`M001..M100`), 13/13 release gates, and 48 Pure Lean 4 theorems (`N01..N48`).
- AVX2/AVX-VNNI/BMI2 + ThinLTO Win64 Clang compilation in `build.ps1`.

## [1.1.0] - 2026-11-20

### Added
- Hard-wired Clean-Slate v3.0 Native Release Gates (`GATE-02, 05, 06, 07, 09, 10, 11`), Ring-0 ACL + WSE-12, XPBD soft-body hydrostat, packed `b1.58` ganglia, Blackiston metamorphosis, and 32 Lean 4 theorems.

## [1.0.0] - 2026-11-18

### Added
- Initial portable C11 whole-organism cognitive engine with 11-pool exact integer ledger, 18 structural ablations, 5 Anti-LARP mutants, Web Observatory (`:8795`), and 24 Lean 4 theorems.
