# Errata (`ERRATA.md`)

All substantive corrections to published versions, documentation, proofs, or claims are logged chronologically here.

## `v2.1.0` (2026-11-25) Corrections to `v2.0.0`

| Ref | Finding Class | Issue in `v2.0.0` | Correction in `v2.1.0` |
|---|---|---|---|
| **E-01** | `problem` | `README.md`, `FINAL_REPORT.md`, and `lean/NoeticProofs.lean` listed an unformed corporate LLC affiliation, although no LLC legal entity exists yet. | Removed all unformed LLC affiliation strings across active repository files; standardized author affiliation to `Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA` (ORCID `0009-0006-8168-9648`). |
| **E-02** | `gap` | `lean/` lacked `lean-toolchain`, `lakefile.toml`, `lake-manifest.json`, Palomar `module` headers, isolated `Spec.lean` / `Challenge.lean` / `Solution.lean` / `comparator.json` / `formalization.yaml`, and explicit non-vacuity witnesses. | Upgraded `lean/` to full 6-kernel Palomar Lean 4.35.0 standard with 54 theorems (`N01..N48` + 6 non-vacuity/refutation witnesses `N49..N54`) and verified across 5 local kernels + 6/6 Linux CI kernels. |
| **E-03** | `gap` | Repository lacked a dual open license (`LICENSE` Apache-2.0 / `LICENSE-DOCS` CC-BY-4.0), AMS-LaTeX white paper, live citation checker, and Build Standard v1.2.0 compliance scaffolding. | Added Apache-2.0 / CC-BY-4.0 dual licenses, zero-overflow Tectonic AMS-LaTeX white paper (`paper/` & `dist/`), 22 live-verified citations, and 100.0% Build Standard v1.2.0 scaffolding. |
| **E-04** | `hole` | `README.md` summarized Lean axiom status in prose rather than pasting verbatim `#print axioms` lines with an automated drift check. | Embedded verbatim 54-line `#print axioms` block in `README.md` guarded by `scripts/check_readme_axioms.py`. |
| **E-05** | `error` | Initial draft of `paper/refs.bib` assigned unregistered DOI `10.2307/4615733` to Holm (1979), and initial `paper/main.tex` miscounted zero-axiom theorems as 32 instead of 31. | Caught by `scripts/check_citations.py` and line-by-line audit of `results/lean4-noetic-audit.txt`; updated `holm1979` to live-verified HTTP 200 JSTOR URL `https://www.jstor.org/stable/4615733` and corrected theorem counts (`31` zero-axiom + `22` `[propext/Quot.sound]` + `1` `[propext, Classical.choice, Quot.sound]` = `54`). |
