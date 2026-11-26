# Contributing to Noetic Organism Omega

Thank you for your interest in auditing, replicating, or improving `noetic-organism-omega`.

## How to Contribute

1. **Bug Reports & Falsification Attempts:** Open a GitHub Issue using the structured forms in `.github/ISSUE_TEMPLATE/`. Please classify findings using the repository's 10 finding classes (`bug`, `error`, `issue`, `problem`, `concern`, `dilemma`, `conflict`, `gap`, `hole`, `blindspot`).
2. **Independent Replication:** Follow `REPLICATION.md` from a clean clone on Windows or Linux and share your SHA-256 receipts and multi-kernel Lean output via an issue labeled `replication`.
3. **Code & Formal Proof Standards:**
   - Portable C11 must compile cleanly with `-std=c11 -O3 -Wall -Wextra -Werror` on both Clang and GCC with zero heap allocation inside `noetic_world_step()`.
   - Zero arm-label score hacking inside `readout_metric()` (verified by `node scripts/audit_zero_theater.mjs`).
   - All 5 compiled Anti-LARP mutants must remain caught (`noetic_engine test`).
   - Lean 4.35.0 modules must use `module` headers, `autoImplicit = false`, zero `sorry`, zero `admit`, zero `native_decide`, strictly within `[propext, Classical.choice, Quot.sound]`, and keep `Challenge.lean` / `Solution.lean` / `comparator.json` / `README.md` axiom block synchronized.
   - Maintain strict epistemic honesty: never claim subjective qualia, sentience, or peer review; preserve `[INDICATOR-ONLY]` and `PASS_CLASSICAL_PARITY_DISCLOSED` tags.

## Workflow Note

The maintainer commits directly to `main` with informational CI (Build Standard Rules R1–R3). External contributors may fork and open issues or share patches; all contributions are licensed under Apache-2.0 (code/proofs) and CC-BY-4.0 (documentation/papers).
