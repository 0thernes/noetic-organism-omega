module

public import NoeticProofs
public import AdversarialTests

/-!
# NOETIC ORGANISM OMEGA v2.2.0 Formal Solution Entrypoint (`Solution.lean`)

Exports all 68 machine-checked theorems (`N01..N60` from `NoeticProofs` + `A01..A08`
from `AdversarialTests`) with `autoImplicit = false` for isolated judging against
`Challenge.lean` via:
    lake comparator --config comparator.json --paranoid
-/
