# ADR 0001: Exact Integer Conservation, Zero-Fanout Structural Ablations, and 6-Kernel Palomar Lean 4.35.0 Boundaries

- **Date:** 2026-11-25
- **Status:** Accepted (`v2.1.0`)
- **Author:** Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA)

## Context

Whole-organism artificial-life simulations that combine floating-point energy updates, ad hoc cognitive scores, and monolithic Lean verification files suffer from four recurring failure modes: (1) unbilled energy creation from FP64 rounding drift, (2) "evaluation theater" where arm labels directly manipulate readouts, (3) consciousness overclaiming on computational proxies, and (4) monolithic Lean files lacking statement isolation or multi-kernel verification.

## Decisions

1. **Exact 11-Pool `int64_t` Thermodynamic Ledger:** All internal transfers and solar inflows use exact integer micro-quanta (`uq`) with irreversible heat/dissipation sinks (`pools[8..9]`), guaranteeing `0 uq` First-Law residual and `0` Second-Law violations.
2. **Zero-Fanout Structural Circuit Ablations & Anti-LARP Mutants:** Every cognitive or homeostatic readout reads strictly from organism behavioral/neural accumulators with zero `w->arm` or `ablation_mask` branches in `readout_metric()`, audited statically (`scripts/audit_zero_theater.mjs`) and dynamically via 5 compiled negative mutants (`MUT-1..5`).
3. **Strict Epistemic Tagging (ADR 0018 Rule-4/6 & Policy P7):** All 29 consciousness-related checks remain tagged `EXECUTED_PASS_INDICATOR_ONLY`, all 5 equal-parameter classical ties remain tagged `PASS_CLASSICAL_PARITY_DISCLOSED`, and `docs/MEGA_ATLAS_CROSSCHECK.md` explicitly records `0 / 1,900` Mega-Atlas problems retired.
4. **Palomar 6-Kernel Lean 4.35.0 Separation:** Pure definitions live in `NoeticProofs/Spec.lean` (plus auto-exported `NoeticProofs/LiveC11Receipt.lean`), `Challenge.lean` imports ONLY `Spec.lean`, `Solution.lean` exports 54 sorry-free proofs (`N01..N48` core + `N49..N54` non-vacuity/refutation witnesses) with `module` headers and `autoImplicit = false`, and `VerifyAxioms.lean` output is locked verbatim into `README.md`.
5. **Dual Open Licensing & Accurate Affiliation:** Apache-2.0 for code/proofs, CC-BY-4.0 for papers/docs/receipts, and `Independent Researcher, Spokane, WA, USA` (removing unformed LLC entity claims).

## Consequences

- Clean zero-dependency builds on Win64 Clang and Linux GCC (`6,776 B/world`, `0` hot-loop heap allocations).
- Full 6/6 Lean kernel verification runs cleanly on Linux CI (`bubblewrap`), while Windows runs 5/6 local independent checkers cleanly (`lean4`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`, `nanoda_bin`).
- C11 source semantics remain outside Lean's kernel and are explicitly documented as an empirical/sanitizer/fuzzing boundary rather than a formal CompCert proof.
