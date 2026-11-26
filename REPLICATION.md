# Replication Protocol (`REPLICATION.md`)

This document specifies exact clean-clone steps to reproduce every C11 receipt, multi-kernel Lean 4.35.0 proof audit, AMS-LaTeX white paper PDF, and repository compliance score for `noetic-organism-omega v2.1.0`.

## Prerequisites

- **C11 Compiler:** Clang 18+ or GCC 12+ (`-std=c11 -O3 -Wall -Wextra -Werror`)
- **Node.js:** `>= 20` (zero npm packages required; built-in modules only)
- **Lean 4 Toolchain:** `elan` with pinned `leanprover/lean4:v4.35.0` (auto-selected by `lean/lean-toolchain`)
- **Python 3.11+ & Tectonic (`0.15+`):** For `scripts/check_readme_axioms.py`, `scripts/check_citations.py`, and `scripts/build_paper.py`
- **Linux Only (for 6/6 sandboxed kernels):** `bubblewrap` (`sudo apt-get install -y bubblewrap`)

## Clean-Clone Reproduction Steps

### 1. Clone and Build the Portable C11 Engine

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
```

**Expected Verification Outputs:**
- `bin/noetic_engine` `test`: `10/10` PASS (`5/5` Anti-LARP mutants caught, `T01` SHA-256 `1fde9fe591dadd326d8a90ba011ead82447150ba227adc16995fe7b1e3a1d1ba`)
- `gates`: `7/7` native gates PASS (`GATE-11` 10,000-tick SHA-256 `eceedf4debbe1b0676cb98c99063076f989ac561bb1afa5fdf02048a7241bce1`)
- `hw24`: `24/24` PASS (`0 uq` drift); `soft40`: `40/40` PASS; `freak100`: `99 PASS + 1 PARITY_DISCLOSED (0 FAIL)`; `novel100`: `97 PASS + 3 PARITY_DISCLOSED (0 FAIL)`; `life500`: `499 PASS + 1 PARITY_DISCLOSED (0 FAIL)`; `milestones100`: `100/100 VERIFIED (First-15 LOCKED_PASS)`

### 2. Run Seed Firewall, Zero-Theater Audit & Property-Based Tests

```bash
node scripts/seed_firewall.mjs
node scripts/audit_zero_theater.mjs
node tests/noetic_prop.test.mjs
node examples/run_quick_demo.mjs
```
*(Note: `scripts/build_m8_e2_825_trace.mjs` additionally cross-checks row IDs against `ALife-Framework-v3.0-Master-Spec.json` when the sibling `alife-singularity-omega-package` repo is present; the pre-generated CSV/JSON trace matrix is committed in `results/`.)*

### 3. Replay Multi-Kernel Lean 4.35.0 Proofs

```bash
cd lean
lake build
lake build Challenge
lake env lean VerifyAxioms.lean > ../results/lean4-noetic-audit.txt
lake env leanchecker --fresh Solution
lake env leanchecker-paranoid --fresh Solution
# On Linux with bubblewrap (all 6/6 kernels + Challenge/Solution comparator):
lake check --paranoid
lake comparator --config comparator.json --paranoid
```

### 4. Verify Axiom Parity, Live Citations, White Paper PDF & Repo Standard

```bash
python scripts/check_readme_axioms.py
python scripts/check_citations.py
python scripts/build_paper.py
pwsh -NoProfile -File scripts/Audit-Repo.ps1
```

---

## Replication Log

| Date (UTC) | Replicator | Platform & Toolchain | Scope Verified | Outcome & Hash Anchor |
|---|---|---|---|---|
| 2026-11-25 | Alexander Stephen Donahue (Author, self-run) | Windows 11 x64, Clang 23 (`AVX2/VNNI ThinLTO`), Lean 4.35.0, Node v25.8.2, Tectonic 0.17.0 | Full `build.ps1`, 825/825 C11, 100/100 milestones, 13/13 gates, 5/5 mutants, 32-trial `prop_check`, 5 local Lean kernels (`lean4`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`, `nanoda_bin`), 22/22 live citations, Tectonic PDF + clean-room arXiv tarball | **PASS** (`T01=1fde9fe5...d1ba`, `GATE-11=eceedf4d...bce1`, `54/54` Lean theorems) |
| — | *(Awaiting external third-party replicator)* | Linux / Windows / macOS | Clean-clone reproduction & independent issue report (`replication` label) | **OPEN** — External replication invited via GitHub Issues |
