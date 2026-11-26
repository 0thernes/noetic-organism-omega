# Hardening Log (`HARDENING_LOG.md`)

Chronological record of red-team findings, classifications (Build Standard Rule R7: 10 finding classes), and verified fixes applied during the `v2.0.0 -> v2.1.0` 3-Cycle Hardening campaign.

## Cycle 1 — Identity, Legal, Epistemic & Palomar Lean 4.35.0 Upgrade (2026-11-25)

| ID | Class | Finding | Remediation & Verification |
|---|---|---|---|
| **H-01** | `problem` | An unformed LLC entity name appeared in `README.md`, `FINAL_REPORT.md`, and `lean/NoeticProofs.lean`. | Replaced all occurrences with `Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA` (ORCID `0009-0006-8168-9648`); verified 0 grep hits across tracked files. |
| **H-02** | `gap` | Repository had no `LICENSE` or `LICENSE-DOCS` files (ambiguous reuse rights). | Added full Apache-2.0 (`LICENSE`) for code/proofs and CC-BY-4.0 (`LICENSE-DOCS`) for papers/docs/receipts, plus `NOTICE` and `THIRD_PARTY_NOTICES.md`. |
| **H-03** | `gap` | `lean/` compiled via ad hoc `lean -R` without `lean-toolchain`, `lakefile.toml`, `lake-manifest.json`, Palomar `module` headers, isolated `Challenge.lean`/`Solution.lean`, or multi-kernel verification. | Migrated to Lake + Lean 4.35.0 `module` system with `NoeticProofs/Spec.lean`, `Challenge.lean`, `Solution.lean`, `comparator.json`, `VerifyAxioms.lean`, `formalization.yaml`, updated C11 `export-lean` emitter, added 6 non-vacuity/refutation theorems (`N49..N54`), and verified across 5 local kernels (`results/lean4-multi-kernel.txt`) + 6/6 CI kernels. |
| **H-04** | `bug` | Initial `N51_refute_ring0_blocks_all_regions` proof used `omega` after `unfold ring0ApplyEdit`, failing to reduce `if 4 < 4`. | Replaced with kernel-level `exact (by decide : ring0ApplyEdit 1000 4 50 ≠ 1000) h4` (0 axioms); `lake build` and all 5 local kernels passed cleanly. |

## Cycle 2 — White Paper Production, Live Citation Audit & Zero-Overflow Typesetting (2026-11-25)

| ID | Class | Finding | Remediation & Verification |
|---|---|---|---|
| **H-05** | `error` | Initial `paper/refs.bib` used unregistered DOI `10.2307/4615733` for Holm (1979). | Caught by `scripts/check_citations.py` (HTTP 404 from DOI Handle API); updated to live-verified HTTP 200 JSTOR URL `https://www.jstor.org/stable/4615733`. |
| **H-06** | `error` | Initial `paper/main.tex` contained unescaped `&` in Table 2 header, 4 overfull `\hbox` lines on long monospace identifiers, and miscounted zero-axiom Lean theorems as 32 instead of 31. | Fixed LaTeX escaping, added `\allowbreak` hints, corrected axiom breakdown (`31` zero-axiom + `22` `[propext/Quot.sound]` + `1` `[propext, Classical.choice, Quot.sound]`), and verified `0` overfull hboxes + clean-room `arxiv_submission.tar.gz` build. |

## Cycle 3 — Build Standard v1.2.0 Hardening, Tests, Fuzzing & Security (2026-11-25)

| ID | Class | Finding | Remediation & Verification |
|---|---|---|---|
| **H-07** | `hole` | No automated `tests/` property-based harness (`A09`/`B10`) or native fuzz target (`B15`) existed outside C11 `noetic_engine test`. | Added `tests/noetic_prop.test.mjs` (`prop_check` over 32 randomized seed/arm/ablation trials + negative CLI tests) and `tests/fuzz_noetic_input.c` (`LLVMFuzzerTestOneInput`). |
| **H-08** | `hole` | `README.md` lacked verbatim `#print axioms` output and automated drift checking. | Added verbatim 54-line `#print axioms` block wrapped in `<!-- LEAN_AXIOMS_START/END -->` and `scripts/check_readme_axioms.py`. |
| **H-09** | `issue` | `web/server.mjs` lacked `/healthz` observability endpoint (`A34`) and `web/index.html` lacked WAI-ARIA landmarks/labels (`A38`). | Added `GET /healthz` returning JSON worker liveness and WAI-ARIA roles/labels to `web/index.html`. |
| **H-10** | `concern` | `.github/workflows/ci.yml` lacked Windows cross-platform matrix, ASan/UBSan sanitizer job, 6-kernel `bubblewrap` comparator, Tectonic PDF check, `SHA256SUMS`, SLSA provenance, and `gitleaks`. | Upgraded `.github/workflows/ci.yml` with cross-platform Windows+Ubuntu matrix, ASan/UBSan, 6-kernel `bubblewrap` Lean audit, Tectonic PDF + arXiv clean-room verification, `SHA256SUMS`, `actions/attest-build-provenance@v2`, and `gitleaks`. |
