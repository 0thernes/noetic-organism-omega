# Third-Party Notices and Attribution

`noetic-organism-omega v2.1.0` ships **zero third-party runtime libraries** inside `src/`, `include/`, `web/`, or `lean/` (pure portable C11 + Node.js built-in standard modules + zero-Mathlib Lean 4.35.0).

## Build, Proof, and Verification Toolchains (External / Not Vendored)

| Tool / Component | License | Role in Repository |
|---|---|---|
| **Lean 4 (`v4.35.0`) & Lake** (`leanprover/lean4`) | Apache-2.0 | Reference dependent type checker, `leanchecker`, `leanchecker-paranoid`, `leanexport`, `lake check`, `lake comparator` |
| **External Lean Kernels** (`lean4lean`, `nanoda_bin`, `con-leche`, `con-ron`) | Apache-2.0 / MIT | Independent multi-kernel verification bundled with Lean 4.35.0 |
| **LLVM / Clang / LLD** & **GCC** | Apache-2.0 with LLVM Exception / GPL-3.0 (runtime exception) | Portable C11 native compilation (`-std=c11 -O3 -Wall -Wextra -Werror`) and ASan/UBSan sanitizer runs |
| **Tectonic** (`tectonic-typesetting/tectonic`) | MIT | Hermetic AMS-LaTeX PDF compilation (`paper/main.tex`) |
| **Node.js** (built-in `node:http`, `node:crypto`, `node:fs`, `node:child_process`) | MIT | Seed firewall, trace matrix builder, zero-theater audit, local Observatory server |
| **Build Standard Audit Harness** (`scripts/Audit-Repo.ps1`, `scripts/audit-repo.sh`, `scripts/audit-checks.tsv`, `repo-standard.schema.json`) | Apache-2.0 / MIT | Build Standard for LLMs v1.2.0 self-audit |

## Algorithmic & Literature Attribution

- **Philox4x32-10** counter-based PRNG design: Salmon, Moraes, Dror & Shaw (SC 2011), DOI `10.1145/2063384.2063405`.
- **SHA-256** cryptographic hash specification: NIST FIPS PUB 180-4 (2015).
- **Extended Position Based Dynamics (XPBD)** formulation: Macklin, Müller & Chentanez (MIG 2016), DOI `10.1145/2994258.2994272`.
- **BitNet b1.58** ternary synaptic packing inspiration: Ma et al. (2024), arXiv:2402.17764.
- **Computational consciousness indicator taxonomy (`[INDICATOR-ONLY]`)**: Butlin et al. (2023), arXiv:2308.08708.

All primary references are cited with verified DOIs/URLs in `paper/refs.bib`.
