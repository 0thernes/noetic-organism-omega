# Replication Protocol (`REPLICATION.md`)

This document specifies exact clean-clone steps to reproduce every cross-OS bit-identical C11 receipt, 68-theorem multi-kernel Lean 4.35.0 proof audit, negative elaboration & `sorry`-tamper rejection test, 22/22 live Crossref/arXiv citation audit, AMS-LaTeX white paper PDF, interactive proof blueprint, dual timestamps, and Build Standard v1.2.0 score for `noetic-organism-omega v2.2.0`.

## Prerequisites

- **C11 Compiler:** Clang 18+ or GCC 12+ (`-std=c11 -O3 -ffp-contract=off -Wall -Wextra -Werror`)
- **Node.js:** `>= 20` (zero npm packages required; built-in modules only)
- **Lean 4 Toolchain:** `elan` with pinned `leanprover/lean4:v4.35.0` (auto-selected by `lean/lean-toolchain`)
- **Python 3.11+ & Tectonic (`0.15+`):** For `scripts/check_readme_axioms.py`, `scripts/check_citations.py`, `scripts/test_lean_negative.py`, `scripts/check_blueprint.py`, `scripts/stamp_timestamps.py`, and `scripts/build_paper.py`
- **Linux Only (for 6/6 sandboxed kernels):** `bubblewrap` (`sudo apt-get install -y bubblewrap`)

## Clean-Clone Reproduction Steps

### 1. Clone and Build the Cross-OS Bit-Identical Portable C11 Engine

**Windows (PowerShell):**
```powershell
git clone https://github.com/0thernes/noetic-organism-omega.git
cd noetic-organism-omega
.\build.ps1
```

**Linux (Bash):**
```bash
git clone https://github.com/0thernes/noetic-organism-omega.git
cd noetic-organism-omega
chmod +x build_linux.sh
./build_linux.sh
./bin/noetic_engine_linux export-lean 5001 /tmp/linux_LiveC11Receipt.lean
diff -u lean/NoeticProofs/LiveC11Receipt.lean /tmp/linux_LiveC11Receipt.lean
```

**Expected Verification Outputs (0-Bit Identical Across Win64 Clang & Linux GCC):**
- `noetic_engine test`: `10/10` PASS (`5/5` Anti-LARP mutants caught, `T01` SHA-256 `6c0ca90e4090aac8611287bf8cd0a49bcef6538dd2b92fbdc82840fb4ed56d2c`)
- `gates`: `7/7` native gates PASS (`GATE-11` 10,000-tick SHA-256 `c7a53fd557c4a08bbeeff960a77efe04069849455e479d1fad97111bb18d3f75`)
- `diff -u` on `LiveC11Receipt.lean`: **0 bytes**
- `hw24`: `24/24` PASS (`0 uq` drift, `HW15` `max_rel_err = 2.274e-16` across 8 portable IEEE-754 functions); `soft40`: `40/40` PASS; `freak100`: `99 PASS + 1 PARITY_DISCLOSED (0 FAIL)`; `novel100`: `97 PASS + 3 PARITY_DISCLOSED (0 FAIL)`; `life500`: `499 PASS + 1 PARITY_DISCLOSED (0 FAIL)`; `milestones100`: `100/100 VERIFIED (First-15 LOCKED_PASS)`

### 2. Run Seed Firewall, Zero-Theater Audit & Property-Based Tests

```bash
node scripts/seed_firewall.mjs
node scripts/audit_zero_theater.mjs
node tests/noetic_prop.test.mjs
node examples/run_quick_demo.mjs
```

### 3. Replay 68-Theorem Multi-Kernel Lean 4.35.0 Proofs & Negative Gates

```bash
cd lean
lake clean && lake build && lake build Challenge
lake env lean VerifyAxioms.lean > ../results/lean4-noetic-audit.txt
lake env leanchecker --fresh Solution
lake env leanchecker-paranoid --fresh Solution
# On Linux with bubblewrap (all 6/6 kernels + Challenge/Solution comparator + sorry-tamper gate):
lake check --paranoid
lake comparator --config comparator.json --paranoid
cd ..
python3 scripts/test_lean_negative.py --comparator
```

### 4. Verify Axiom Parity, Live Citations, Blueprint, Dual Timestamps, White Paper PDF & Repo Standard

```bash
python3 scripts/check_readme_axioms.py
python3 scripts/check_citations.py
python3 scripts/check_blueprint.py
python3 scripts/stamp_timestamps.py --verify
python3 scripts/build_paper.py
bash scripts/audit-repo.sh .
```

---

## Replication Log

| Date (UTC) | Replicator | Platform & Toolchain | Scope Verified | Outcome & Hash Anchor |
|---|---|---|---|---|
| 2026-11-26 | Alexander Stephen Donahue (Author, self-run) | Windows 11 x64 Clang 23 (`AVX2/VNNI ThinLTO -ffp-contract=off`) + Ubuntu 24.04 Linux GCC 13.3 (`-ffp-contract=off`) + Lean 4.35.0 + Tectonic 0.17.0 | Full `v2.2.0` cross-OS 0-byte `diff -u` parity, 825/825 C11, 100/100 milestones, 13/13 gates, 5/5 mutants, 32-trial `prop_check`, 68/68 Lean proofs across 5 local + 6/6 CI kernels, live negative & `sorry`-tamper gates, 22/22 Crossref/arXiv citations, 11-page PDF + clean-room arXiv, WCAG 2.2 AA blueprint, DigiCert `.tsr` + 3-cal Bitcoin `.ots`, `100.0%` (`78/78`) Build Standard | **PASS** (`T01=6c0ca90e...6d2c`, `GATE-11=c7a53fd5...3f75`, `0 B diff`, `68/68` Lean) |
| 2026-11-25 | Alexander Stephen Donahue (Author, self-run) | Windows 11 x64, Clang 23, Lean 4.35.0, Node v25.8.2, Tectonic 0.17.0 | `v2.1.0` full `build.ps1`, 825/825 C11, 100/100 milestones, 13/13 gates, 5/5 mutants, 54/54 Lean theorems | **PASS** (superseded by `v2.2.0` cross-OS fix) |
| — | *(Awaiting external third-party replicator)* | Linux / Windows / macOS | Clean-clone reproduction & independent issue report (`replication` label) | **OPEN** — External replication invited via GitHub Issues |
