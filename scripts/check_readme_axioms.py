#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Verify that README.md reproduces the exact verbatim Lean 4.35.0 `#print axioms`
output from `results/lean4-noetic-audit.txt` with zero drift, and that all 54 theorems
depend strictly on `{propext, Classical.choice, Quot.sound}` (or zero axioms)."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
AXIOMS_PATH = ROOT / "results" / "lean4-noetic-audit.txt"
README_PATH = ROOT / "README.md"
ALLOWED_AXIOMS = {"propext", "Classical.choice", "Quot.sound"}
FORBIDDEN = ("sorryAx", "Lean.ofReduceBool", "Lean.trustCompiler", "_native", "warning:")
EXPECTED_COUNT = 54


def main() -> None:
    if not AXIOMS_PATH.exists():
        print(f"FAIL: missing {AXIOMS_PATH}", file=sys.stderr)
        sys.exit(1)
    axioms_txt = AXIOMS_PATH.read_text(encoding="utf-8-sig").replace("\r\n", "\n").strip()
    lines = [ln.strip() for ln in axioms_txt.splitlines() if ln.strip()]
    if len(lines) != EXPECTED_COUNT:
        print(f"FAIL: expected {EXPECTED_COUNT} axiom lines in {AXIOMS_PATH}, got {len(lines)}", file=sys.stderr)
        sys.exit(1)
    for tok in FORBIDDEN:
        if tok in axioms_txt:
            print(f"FAIL: forbidden axiom/warning token {tok!r} found in {AXIOMS_PATH}", file=sys.stderr)
            sys.exit(1)

    for ln in lines:
        if re.match(r"^'NoeticProofs\.[^']+' does not depend on any axioms$", ln):
            continue
        m = re.match(r"^'NoeticProofs\.[^']+' depends on axioms: \[([^\]]+)\]$", ln)
        if not m:
            print(f"FAIL: unexpected axiom line format: {ln}", file=sys.stderr)
            sys.exit(1)
        axs = {a.strip() for a in m.group(1).split(",") if a.strip()}
        bad = axs - ALLOWED_AXIOMS
        if bad:
            print(f"FAIL: unpermitted axiom(s) {bad} in line: {ln}", file=sys.stderr)
            sys.exit(1)

    readme = README_PATH.read_text(encoding="utf-8-sig").replace("\r\n", "\n")
    marker_start = "<!-- LEAN_AXIOMS_START -->"
    marker_end = "<!-- LEAN_AXIOMS_END -->"
    if marker_start not in readme or marker_end not in readme:
        print("FAIL: README.md missing LEAN_AXIOMS_START / LEAN_AXIOMS_END markers", file=sys.stderr)
        sys.exit(1)
    block = readme.split(marker_start, 1)[1].split(marker_end, 1)[0]
    m = re.search(r"```text\n(.*?)\n```", block, flags=re.S)
    if not m:
        print("FAIL: README.md axioms block missing ```text fence", file=sys.stderr)
        sys.exit(1)
    readme_axioms = "\n".join(ln.strip() for ln in m.group(1).strip().splitlines() if ln.strip())
    expected_block = "\n".join(lines)
    if readme_axioms != expected_block:
        print("FAIL: README.md verbatim axioms block drifted from results/lean4-noetic-audit.txt!", file=sys.stderr)
        sys.exit(1)

    print(f"PASS: All {len(lines)}/{EXPECTED_COUNT} Lean 4.35.0 theorem axioms verified strictly within {{propext, Classical.choice, Quot.sound}} and README.md verbatim block matches 100%.")


if __name__ == "__main__":
    main()
