#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Live Negative Elaboration & Sorry-Tamper Rejection Harness for Lean 4.35.0.

Verifies two adversarial soundness properties:
1. Negative elaboration: Lean 4.35.0 MUST reject intentionally false theorems
   about `NoeticProofs.Spec` (`unguardedAtpToHeat`, `ring0ApplyEdit` at region 4,
   imbalanced `Ledger11` debit/credit, invalid zero seed).
2. Sorry-tamper detection:
   - Injecting `sorry` emits `sorryAx` in `#print axioms` (caught by axiom gate).
   - With `--comparator` (Linux CI with bubblewrap), tampering a temporary copy of
     `Solution` with `sorry` MUST cause `lake comparator --paranoid` to exit non-zero.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEAN_DIR = ROOT / "lean"


def find_lake() -> str:
    for cand in (
        shutil.which("lake"),
        str(Path.home() / ".elan" / "bin" / ("lake.exe" if sys.platform == "win32" else "lake")),
    ):
        if cand and Path(cand).exists():
            return cand
    raise FileNotFoundError("lake executable not found on PATH or ~/.elan/bin")


NEGATIVE_CASES: list[tuple[str, str]] = [
    (
        "false_unguarded_heat_monotone",
        """module
public import NoeticProofs.Spec
open NoeticProofs
theorem bad_heat (L : Ledger11) (amt : Int) : L.p8 ≤ (unguardedAtpToHeat L amt).p8 := by
  unfold unguardedAtpToHeat; dsimp only; omega
""",
    ),
    (
        "false_region4_immutable",
        """module
public import NoeticProofs.Spec
open NoeticProofs
theorem bad_r4 (st delta : Int) : ring0ApplyEdit st 4 delta = st := by
  unfold ring0ApplyEdit; decide
""",
    ),
    (
        "false_imbalanced_ledger11",
        """module
public import NoeticProofs.Spec
open NoeticProofs
theorem bad_l11 (L : Ledger11) :
    residual11 { L with p3 := L.p3 - 10, p8 := L.p8 + 11 } = residual11 L := by
  unfold residual11 poolSum11; dsimp only; omega
""",
    ),
    (
        "false_zero_seed_valid",
        """module
public import NoeticProofs.Spec
open NoeticProofs
theorem bad_seed0 : isValidSeed 0 := by
  unfold isValidSeed; decide
""",
    ),
]


def test_negative_elaboration(lake: str) -> None:
    tmp_dir = Path(tempfile.mkdtemp(prefix="noetic_lean_neg_"))
    try:
        for name, code in NEGATIVE_CASES:
            f = tmp_dir / f"{name}.lean"
            f.write_text(code, encoding="utf-8", newline="\n")
            res = subprocess.run(
                [lake, "env", "lean", str(f)],
                cwd=LEAN_DIR,
                capture_output=True,
                text=True,
                check=False,
            )
            if res.returncode == 0:
                print(f"FAIL: Negative Lean case {name!r} unexpectedly elaborated with exit 0!", file=sys.stderr)
                sys.exit(1)
            print(f"  [PASS] Negative elaboration rejected {name} (exit={res.returncode})")
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_sorry_axiom_detection(lake: str) -> None:
    tmp_dir = Path(tempfile.mkdtemp(prefix="noetic_lean_sorry_"))
    try:
        f = tmp_dir / "tamper_sorry.lean"
        f.write_text(
            "module\npublic import NoeticProofs.Spec\nopen NoeticProofs\n"
            "theorem tampered_zero_seed : isValidSeed 0 := by sorry\n"
            "#print axioms tampered_zero_seed\n",
            encoding="utf-8",
            newline="\n",
        )
        res = subprocess.run(
            [lake, "env", "lean", str(f)],
            cwd=LEAN_DIR,
            capture_output=True,
            text=True,
            check=False,
        )
        combined = res.stdout + "\n" + res.stderr
        if "sorryAx" not in combined:
            print(f"FAIL: Expected sorryAx in tampered Lean output, got:\n{combined}", file=sys.stderr)
            sys.exit(1)
        print("  [PASS] Axiom gate exposes sorryAx on sorry-tampered theorem")
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def test_comparator_sorry_rejection(lake: str) -> None:
    """Copy `lean/` (without `.lake` cache) to a clean temp sandbox, inject `sorry` into
    `AdversarialTests.lean`, build `Challenge` & `Solution`, and verify
    `lake comparator --config comparator.json --paranoid` REJECTS the tampered Solution."""
    tmp_dir = Path(tempfile.mkdtemp(prefix="noetic_comp_tamper_"))
    try:
        sandbox = tmp_dir / "lean"
        shutil.copytree(LEAN_DIR, sandbox, ignore=shutil.ignore_patterns(".lake"))
        adv = sandbox / "AdversarialTests.lean"
        orig = adv.read_text(encoding="utf-8")
        tampered = orig.replace(
            "  unfold isValidSeed\n  decide",
            "  sorry",
            1,
        )
        if tampered == orig:
            print("FAIL: Could not inject sorry tamper into AdversarialTests.lean", file=sys.stderr)
            sys.exit(1)
        adv.write_text(tampered, encoding="utf-8", newline="\n")

        for target in ("Challenge", "Solution"):
            b_res = subprocess.run(
                [lake, "build", target],
                cwd=sandbox,
                capture_output=True,
                text=True,
                check=False,
            )
            if b_res.returncode != 0:
                print(f"FAIL: Tampered {target} failed to build before comparator:\n{b_res.stdout}\n{b_res.stderr}", file=sys.stderr)
                sys.exit(1)

        c_res = subprocess.run(
            [lake, "comparator", "--config", "comparator.json", "--paranoid"],
            cwd=sandbox,
            capture_output=True,
            text=True,
            check=False,
        )
        if c_res.returncode == 0:
            print("FAIL: lake comparator unexpectedly ACCEPTED a sorry-tampered Solution!", file=sys.stderr)
            sys.exit(1)
        print(f"  [PASS] Live lake comparator --paranoid REJECTED sorry-tampered Solution (exit={c_res.returncode})")
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def main() -> None:
    parser = argparse.ArgumentParser(description="Run Lean 4.35.0 negative elaboration & sorry-tamper tests.")
    parser.add_argument(
        "--comparator",
        action="store_true",
        help="Also run live lake comparator sorry-tamper rejection (requires Linux bubblewrap)",
    )
    args = parser.parse_args()
    lake = find_lake()
    print("=== LEAN 4.35.0 NEGATIVE ELABORATION & SORRY-TAMPER VERIFICATION ===")
    test_negative_elaboration(lake)
    test_sorry_axiom_detection(lake)
    if args.comparator:
        test_comparator_sorry_rejection(lake)
    print("PASS: All Lean negative elaboration & tamper-rejection gates passed!")


if __name__ == "__main__":
    main()
