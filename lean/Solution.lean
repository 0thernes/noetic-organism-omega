module

public import NoeticProofs

/-!
# NOETIC ORGANISM OMEGA v2.1.0 Formal Solution Entrypoint (`Solution.lean`)

Exports all 54 machine-checked theorems (`N01..N54`, including 6 non-vacuity and
sharp negative boundary witnesses) from `NoeticProofs` with `autoImplicit = false`
for isolated judging against `Challenge.lean` via:
    lake comparator --config comparator.json --paranoid
-/
