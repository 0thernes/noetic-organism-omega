# ADR 0001: Exact Integer Conservation, Zero-Fanout Structural Ablations, and 6-Kernel Palomar Lean 4.35.0 Boundaries

- **Date:** 2026-11-26 (updated for `v2.2.0`)
- **Status:** Accepted (`v2.2.0`)
- **Author:** Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA)

## Context

Whole-organism artificial-life simulations that combine floating-point energy updates, ad hoc cognitive scores, and monolithic Lean verification files suffer from four recurring failure modes: (1) unbilled energy creation from FP64 rounding drift, (2) "evaluation theater" where arm labels directly manipulate readouts, (3) consciousness overclaiming on computational proxies, and (4) monolithic Lean files lacking statement isolation or multi-kernel verification.

## Decisions

1. **Exact 11-Slot `int64_t` Thermodynamic Ledger & Cross-OS IEEE-754 Determinism:** All internal transfers and solar inflows use exact integer micro-quanta (`uq`) with irreversible heat/dissipation sinks (`pools[8..9]`), with `0 uq` First-Law residual and `0` heat-sink decreases observed in the C11 test suites (tested, not proved; in `v2.2.0`, Lean models both the 10-field unbounded-`Int` `Ledger10` and the full 11-slot unbounded-`Int` `Ledger11` matching `pools[0..10]` plus `initialTotal`, while C11 `int64_t` overflow remains runtime-tested rather than formally proved). Eight pure IEEE-754 portable math functions (`max_rel_err = 2.274e-16`) compiled with `-ffp-contract=off` yield 0-bit identical SHA-256 world states and 0-byte `diff -u` on `LiveC11Receipt.lean` across Win64 Clang and Linux GCC.
2. **Zero-Fanout Structural Circuit Ablations & Anti-LARP Mutants:** Every cognitive or homeostatic readout reads strictly from organism behavioral/neural accumulators with zero `w->arm` or `ablation_mask` branches in `readout_metric()`, audited statically (`scripts/audit_zero_theater.mjs`) and dynamically via 5 compiled negative mutants (`MUT-1..5`).
3. **Strict Epistemic Tagging (ADR 0018 Rule-4/6 & Policy P7):** All 29 consciousness-related checks remain tagged `EXECUTED_PASS_INDICATOR_ONLY`, all 5 equal-parameter classical ties remain tagged `PASS_CLASSICAL_PARITY_DISCLOSED`, and `docs/MEGA_ATLAS_CROSSCHECK.md` explicitly records `0 / 1,900` Mega-Atlas problems retired.
4. **Palomar 6-Kernel Lean 4.35.0 Separation:** Pure definitions live in `NoeticProofs/Spec.lean` (plus cross-OS identical auto-exported `NoeticProofs/LiveC11Receipt.lean`), `Challenge.lean` imports ONLY `Spec.lean`, `Solution.lean` exports 68 sorry-free proofs (`N01..N60` + `A01..A08` in `AdversarialTests.lean`, explicitly distinguishing 46 parametric/adversarial proofs, 18 bookkeeping numeral identities, and 4 receipt constant checks) with `module` headers and `autoImplicit = false`, `VerifyAxioms.lean` output is locked verbatim into `README.md`, and `scripts/test_lean_negative.py --comparator` verifies live negative elaboration and `sorry`-tamper rejection.
5. **Dual Open Licensing & Accurate Affiliation:** Apache-2.0 for code/proofs, CC-BY-4.0 for papers/docs/receipts (`REUSE.toml` SPDX-annotated), and `Independent Researcher, Spokane, WA, USA` (zero unformed LLC entity claims).

## Consequences

- Clean zero-dependency builds on Win64 Clang and Linux GCC (`6,776 B/world`, `0` hot-loop heap allocations).
- Full 6/6 Lean kernel verification runs cleanly on Linux CI (`bubblewrap`), while Windows runs 5/6 local independent checkers cleanly (`lean4`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`, `nanoda_bin`).
- C11 source semantics remain outside Lean's kernel and are explicitly documented as an empirical/sanitizer/fuzzing boundary rather than a formal CompCert proof.
