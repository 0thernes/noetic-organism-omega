# Changelog

All notable changes to `noetic-organism-omega` are documented in this file.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and [Semantic Versioning](https://semver.org/).

## [2.2.0] - 2026-11-26

### Fixed
- **Cross-OS 0-Bit Receipt Determinism (`ERRATA.md` E-10 & CI Hole):** Diagnosed and eliminated Win64 Clang vs Linux GCC world-state SHA-256 drift by adding 5 portable IEEE-754 functions (`noetic_log2_det`, `noetic_sin_det`, `noetic_cos_det`, `noetic_atan2_det`, `noetic_hypot_det`; `max_rel_err = 2.274e-16` across all 8 functions in `HW15`), removing all 89 raw `libm` calls from `src/noetic_organism.c` and `noetic_philox_normal()`, adding `-ffp-contract=off` across Linux GCC & Windows MinGW builds, switching `export-lean` to binary LF output (`"wb"`), and adding a hard `diff -u` cross-OS receipt parity check in `.github/workflows/ci.yml`. Win64 Clang 23 and Linux GCC 13.3 now produce 0-bit identical SHA-256 world states (`6c0ca90e4090aac8...ed56d2c` at 180 ticks; `c7a53fd5...3f75` at 10,000 ticks) and 0-byte `diff` on `LiveC11Receipt.lean`.
- **Win64 Clang Sysroot Isolation:** Added `--sysroot=$root` and `-B$root\lib` in `build.ps1` so Lean's bundled Clang links its own `crt2.o` rather than picking up a third-party MinGW installation on `PATH`.
- **Formal Code Comments & Paper Errata (`ERRATA.md`):** Corrected all inaccurate Lean comments (`N02`, `N06`, `N25`, `N30`, bookkeeping counts) and all 6 frozen paper items in `paper/main.tex` (added `\usepackage[obeyspaces]{url}`, split C1–C5 Lean-model vs C11-runtime rows, honest `SYS-009` & `AI-001` rows, accurate 68-theorem breakdown, cross-OS hash disclosure, SWHID, SLSA/Rekor, dual timestamps).

### Added
- **11-Slot `Ledger11` Formalization & Adversarial Refutations (`68` Lean 4.35.0 Theorems):** Added `Ledger11`, `poolSum11`, `residual11`, `nonNeg11`, `solarIn11`, `openTick11`, `unguardedAtpToHeat`, and `isValidSeed` in `lean/NoeticProofs/Spec.lean`, 6 new theorems (`N55..N60`, closing the 10-vs-11 ledger gap and proving strict `1.6x` fasting ratio equivalence), 8 zero-axiom false-variant refutations in `lean/AdversarialTests.lean` (`A01..A08`), and live negative elaboration + `sorry`-tamper `lake comparator` rejection in `scripts/test_lean_negative.py`.
- **Live Crossref/arXiv Citation Alignment Audit:** Upgraded `scripts/check_citations.py` to check both live reachability and Crossref/arXiv title + author alignment across all 22 references (`results/citation-audit.json`).
- **Interactive Proof Blueprint, GitHub Pages, FAIR Metadata, REUSE & Dual Timestamps:** Added WCAG 2.2 AA strict-CSP zero-CDN `docs/blueprint.html` + `docs/index.html`, `scripts/check_blueprint.py`, `.github/workflows/pages.yml`, `.github/workflows/release.yml`, `croissant.json`, `ro-crate-metadata.json`, `codemeta.json`, `docs/DATASHEET.md`, `SUBMISSION_READINESS.md`, `REUSE.toml`, `LICENSES/{Apache-2.0,CC-BY-4.0}.txt`, and `scripts/stamp_timestamps.py` minting real DigiCert RFC 3161 (`CHECKSUMS.sha256.tsr`) + 3-calendar Bitcoin OpenTimestamps (`CHECKSUMS.sha256.ots`) proofs.

### Changed
- **Claims corrected to match the 2026-11-26 statement audit** (preserved from commit `95c0ec6` and extended to `v2.2.0`): Lean theorems report exact breakdown (`46` parametric/adversarial + `18` closed numeral bookkeeping identities + `4` live C11 receipt constant checks = `68`), split Lean-model (`kernel-checked`) from C11 runtime (`program-audited`/`empirical`), and retain `0 / 1,900` Mega-Atlas non-retirement and `[INDICATOR-ONLY]` tagging.

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
