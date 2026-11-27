# Noetic Organism Omega v2.2.0 — Task Runner (`justfile`)
# SPDX-License-Identifier: Apache-2.0

set shell := ["bash", "-euo", "pipefail", "-c"]
set windows-shell := ["pwsh", "-NoProfile", "-Command"]

# Full clean-clone reproduction of C11 engine, 825 checks, 100 milestones, 68 Lean 4.35.0 proofs, blueprint, timestamps, and white paper
reproduce: build test lean lean-negative axioms citations blueprint timestamps paper audit

# Build portable C11 native engine (-std=c11 -O3 -ffp-contract=off -Wall -Wextra -Werror)
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
  gcc -std=c11 -O1 -g -ffp-contract=off -Wall -Wextra -Werror -Wno-unused-parameter -fsanitize=address,undefined -Iinclude \
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

# Build and verify 68 Pure Lean 4.35.0 Palomar theorems across independent kernels
lean:
  cd lean && lake build && lake build Challenge
  cd lean && lake env lean VerifyAxioms.lean > ../results/lean4-noetic-audit.txt
  cd lean && lake env leanchecker --fresh Solution
  cd lean && lake env leanchecker-paranoid --fresh Solution
  cd lean && lake check --paranoid && lake comparator --config comparator.json --paranoid

# Run live Lean negative elaboration and sorry-tamper comparator rejection
lean-negative:
  python3 scripts/test_lean_negative.py --comparator

# Verify README verbatim Lean #print axioms block matches results/lean4-noetic-audit.txt
axioms:
  python3 scripts/check_readme_axioms.py

# Verify all 22 citations (reachability + Crossref/arXiv title & author alignment) live
citations:
  python3 scripts/check_citations.py

# Verify WCAG 2.2 AA strict-CSP zero-CDN Interactive Proof Blueprint & Pages site
blueprint:
  python3 scripts/check_blueprint.py

# Verify DigiCert RFC 3161 (.tsr) and 3-calendar Bitcoin OpenTimestamps (.ots) proofs
timestamps:
  python3 scripts/stamp_timestamps.py --verify

# Compile AMS-LaTeX white paper & clean-room arXiv tarball via Tectonic
paper:
  python3 scripts/build_paper.py

# Run Build Standard v1.2.0 self-audit
audit:
  bash scripts/audit-repo.sh .

# Launch loopback-only Web Observatory on http://127.0.0.1:8795/
serve:
  node web/server.mjs
