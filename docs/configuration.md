# Configuration & Environment (`docs/configuration.md`)

`noetic-organism-omega v2.1.0` requires **zero runtime configuration files or secrets** to compile and verify.

## Environment Variables (Optional)

Documented in [`.env.example`](../.env.example):

| Variable | Default | Purpose |
|---|---|---|
| `PORT` | `8795` | Loopback HTTP port for `node web/server.mjs` (always bound strictly to `127.0.0.1`) |
| `TECTONIC_BIN` | auto-detected on `PATH` or `~/.local/bin/tectonic` | Explicit path to `tectonic` executable used by `scripts/build_paper.py` |

## Compile-Time Build Configuration

- **Windows (`build.ps1`):** Uses Clang bundled with Lean 4.35.0 (`%USERPROFILE%\.elan\toolchains\leanprover--lean4---v4.35.0\bin\clang.exe`) with `-std=c11 -O3 -Wall -Wextra -Werror -march=native -mavx2 -mavxvnni -mbmi -mbmi2 -mpopcnt -ffp-contract=off -flto=thin`. Switches: `-SkipSuites`, `-SkipLean`.
- **Linux (`build_linux.sh`):** Uses `gcc -std=c11 -O3 -Wall -Wextra -Werror -Wno-unused-parameter -Iinclude ... -lm`.
- **Lean 4 (`lean/lean-toolchain` & `lean/lakefile.toml`):** Pinned to `leanprover/lean4:v4.35.0` with `autoImplicit = false`, `relaxedAutoImplicit = false`, `linter.unusedVariables = true`, `linter.deprecated = true`, and zero external dependencies (`lean/lake-manifest.json`).
