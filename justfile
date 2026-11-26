# Noetic Organism Omega v2.1.0 — Task Runner (`justfile`)
# SPDX-License-Identifier: Apache-2.0

set shell := ["bash", "-euo", "pipefail", "-c"]
set windows-shell := ["pwsh", "-NoProfile", "-Command"]

# Full clean-clone reproduction of C11 engine, 825 checks, 100 milestones, Lean 4.35.0, and white paper
reproduce: build test lean axioms citations paper audit

# Build portable C11 native engine (-std=c11 -O3 -Wall -Wextra -Werror)
build:
  ./build_linux.sh

# Run C11 unit/mutant tests, Seed Firewall, Zero-Theater audit, and property-based tests
test:
  ./bin/noetic_engine_linux test
  node scripts/seed_firewall.mjs
  node scripts/audit_zero_theater.mjs
  node tests/noetic_prop.test.mjs
  node examples/run_quick_demo.mjs

# Compile and run C11 engine under AddressSanitizer + UndefinedBehaviorSanitizer
sanitizers:
  gcc -std=c11 -O1 -g -Wall -Wextra -Werror -Wno-unused-parameter -fsanitize=address,undefined -Iinclude \
    src/noetic_rng.c src/noetic_stats.c src/noetic_organism.c src/noetic_gates_core.c \
    src/noetic_hw24.c src/noetic_soft40.c src/noetic_freak100.c src/noetic_novel100.c \
    src/noetic_life500.c src/noetic_milestones100.c src/noetic_eval.c src/main_cli.c \
    -o bin/noetic_engine_asan -lm
  ./bin/noetic_engine_asan test
  ./bin/noetic_engine_asan gates

# Run report-only static analysis (clang-tidy / cppcheck when installed)
lint:
  command -v cppcheck >/dev/null && cppcheck --enable=warning,performance,portability --std=c11 -Iinclude src/ || echo "cppcheck not installed; skipped"
  command -v clang-tidy >/dev/null && clang-tidy src/noetic_rng.c -- -std=c11 -Iinclude || echo "clang-tidy not installed; skipped"

# Build and verify 54 Pure Lean 4.35.0 Palomar theorems across independent kernels
lean:
  cd lean && lake build && lake build Challenge
  cd lean && lake env lean VerifyAxioms.lean > ../results/lean4-noetic-audit.txt
  cd lean && lake env leanchecker --fresh Solution
  cd lean && lake env leanchecker-paranoid --fresh Solution
  cd lean && lake check --paranoid && lake comparator --config comparator.json --paranoid

# Verify README verbatim Lean #print axioms block matches results/lean4-noetic-audit.txt
axioms:
  python3 scripts/check_readme_axioms.py

# Verify all 22 citations & DOIs/URLs live
citations:
  python3 scripts/check_citations.py

# Compile AMS-LaTeX white paper & clean-room arXiv tarball via Tectonic
paper:
  python3 scripts/build_paper.py

# Run Build Standard v1.2.0 self-audit
audit:
  bash scripts/audit-repo.sh .

# Launch loopback-only Web Observatory on http://127.0.0.1:8795/
serve:
  node web/server.mjs
