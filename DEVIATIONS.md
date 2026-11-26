# Deviations from Build Standard & Reference Templates (`DEVIATIONS.md`)

All deliberate deviations from default template conventions are documented explicitly here.

| ID | Area | Deviation | Rationale & Mitigation |
|---|---|---|---|
| **D1** | `A27` (GitHub Pages) | Declared `n/a` in `repo.yaml` | The Web Observatory (`web/server.mjs`) streams a live native C11 binary (`noetic_engine stream`) over loopback `127.0.0.1:8795`; static GitHub Pages cannot execute the native C11 binary. |
| **D2** | `A28` / `A41` / `B02` (`package: false`) | No `package.json` / `Cargo.toml` / `pyproject.toml` manifest | `noetic-organism-omega` is a zero-dependency C11 + built-in Node.js + zero-Mathlib Lean 4.35.0 research repository. Toolchains are explicitly pinned via `lean/lean-toolchain` and `lean/lake-manifest.json`. |
| **D3** | `B23` / `B24` / `B25` | No separate TLA+/Alloy, verified-language C11 replacement, or CBMC/Frama-C run | Core thermodynamic, Ring-0 ACL, `b1.58`, and statistical invariants are machine-checked directly in Lean 4.35.0 across 6 kernels; C11 runtime safety is audited via `-Wall -Wextra -Werror`, ASan/UBSan in CI, `tests/fuzz_noetic_input.c`, 5 compiled mutants, and 825 native checks. |
| **D4** | Local Windows vs. Linux CI Kernels | 5 independent Lean checkers run locally on Windows; full 6/6 (`lean4lean` + `bubblewrap` `lake comparator`) run on Linux CI | Upstream Lean 4.35.0 `lean4lean.exe` cannot parse Windows `.olean` path headers and `lake check` / `lake comparator` require Linux `bubblewrap`. Documented transparently in `results/lean4-multi-kernel.txt` and `LIMITATIONS.md`. |
| **D5** | Tracked Native Binaries (`bin/`) | Small compiled binaries (`< 450 KB`, well under the 5 MB `A24` cap) remain tracked from v2.0.0 for immediate local Observatory startup | Fully reproducible from `src/*.c` via `.\build.ps1` and `./build_linux.sh`; SHA-256 digests recorded in `CHECKSUMS.sha256`. |
