module

public import NoeticProofs.LiveC11Receipt

/-!
# NoeticProofs.Spec — Pure Shared Definitions Only (Zero Proofs, Zero `sorry`)

Canonical mathematical, thermodynamic (`Ledger10` and 11-slot `Ledger11`), abstract
region-guard, packed b1.58, seed-firewall, and statistical definitions for
NOETIC ORGANISM OMEGA v2.2.0 shared identically by BOTH:
  - `Challenge.lean` (isolated statement-only specification — imports ONLY this module)
  - `NoeticProofs.lean`, `AdversarialTests.lean`, and `Solution.lean` (sorry-free proofs
    verified across 6 independent kernels)

Author: Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA; ORCID 0009-0006-8168-9648)
License: Apache-2.0
-/

namespace NoeticProofs

/-- 10-field unbounded-`Int` thermodynamic state modeling `NoeticLedger.pools[0..9]`
    (excludes slot 10 `SOLAR_IN`; see `Ledger11` below for the full 11-slot model). -/
public structure Ledger10 where
  p0 : Int  -- GUT
  p1 : Int  -- BLOOD
  p2 : Int  -- STORES
  p3 : Int  -- ATP
  p4 : Int  -- TISSUE
  p5 : Int  -- NEURAL_WORK
  p6 : Int  -- PATCH
  p7 : Int  -- DETRITUS
  p8 : Int  -- HEAT_PHYS (irreversible sink)
  p9 : Int  -- DISS_NUM  (irreversible sink)
  deriving Repr, DecidableEq

/-- Exact total sum across all 10 conservative & dissipative fields (unbounded `Int`). -/
@[expose] public def poolSum (L : Ledger10) : Int :=
  L.p0 + L.p1 + L.p2 + L.p3 + L.p4 + L.p5 + L.p6 + L.p7 + L.p8 + L.p9

/-- Non-negativity invariant across all 10 integer energy fields. -/
@[expose] public def nonNeg (L : Ledger10) : Prop :=
  0 ≤ L.p0 ∧ 0 ≤ L.p1 ∧ 0 ≤ L.p2 ∧ 0 ≤ L.p3 ∧ 0 ≤ L.p4 ∧
  0 ≤ L.p5 ∧ 0 ≤ L.p6 ∧ 0 ≤ L.p7 ∧ 0 ≤ L.p8 ∧ 0 ≤ L.p9

/-- Atomic guarded transfer: ATP (p3) -> physical heat sink (p8). -/
@[expose] public def transferAtpToHeat (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p8 := L.p8 + amt } else L

/-- Atomic guarded transfer: environmental Patch (p6) -> Gut (p0). -/
@[expose] public def transferPatchToGut (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p6 then { L with p6 := L.p6 - amt, p0 := L.p0 + amt } else L

/-- Atomic guarded transfer: ATP (p3) -> Neural Work (p5). -/
@[expose] public def transferAtpToNeural (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p5 := L.p5 + amt } else L

/-- Atomic guarded transfer: Neural Work (p5) -> physical heat sink (p8). -/
@[expose] public def transferNeuralToHeat (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p5 then { L with p5 := L.p5 - amt, p8 := L.p8 + amt } else L

/-- Atomic guarded transfer: ATP (p3) -> Tissue repair (p4). -/
@[expose] public def transferAtpToTissue (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p4 := L.p4 + amt } else L

/-- Guarded exogenous solar inflow into environmental Patch (p6) on `Ledger10`. -/
@[expose] public def addSolarToPatch (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt then { L with p6 := L.p6 + amt } else L

/-- Composite closed-world organism tick on `Ledger10`. -/
@[expose] public def closedTick (L : Ledger10) (bite work repair heat : Int) : Ledger10 :=
  transferAtpToHeat
    (transferAtpToTissue
      (transferNeuralToHeat
        (transferAtpToNeural (transferPatchToGut L bite) work)
        work)
      repair)
    heat

/-- 11-slot unbounded-`Int` thermodynamic ledger matching all 11 slots of
    `NoeticLedger.pools[0..10]` in C11 (`p0..p9` physical pools + `p10` cumulative
    `SOLAR_IN`) paired with `initialTotal` (`E0`). Uses unbounded `Int` (C11 `int64_t`
    overflow is checked at runtime, not modeled here). -/
public structure Ledger11 where
  p0 : Int            -- GUT
  p1 : Int            -- BLOOD
  p2 : Int            -- STORES
  p3 : Int            -- ATP
  p4 : Int            -- TISSUE
  p5 : Int            -- NEURAL_WORK
  p6 : Int            -- PATCH
  p7 : Int            -- DETRITUS
  p8 : Int            -- HEAT_PHYS (irreversible sink)
  p9 : Int            -- DISS_NUM  (irreversible sink)
  p10 : Int           -- SOLAR_IN  (cumulative exogenous inflow slot)
  initialTotal : Int  -- E0
  deriving Repr, DecidableEq

/-- Sum of physical pools `p0..p9` in `Ledger11`. -/
@[expose] public def poolSum11 (L : Ledger11) : Int :=
  L.p0 + L.p1 + L.p2 + L.p3 + L.p4 + L.p5 + L.p6 + L.p7 + L.p8 + L.p9

/-- Conservation residual matching `noetic_ledger_residual()` in `src/noetic_rng.c`:
    `sum(p0..p9) - (initialTotal + p10)`. -/
@[expose] public def residual11 (L : Ledger11) : Int :=
  poolSum11 L - (L.initialTotal + L.p10)

/-- Non-negativity invariant across all 11 slots `p0..p10` of `Ledger11`. -/
@[expose] public def nonNeg11 (L : Ledger11) : Prop :=
  0 ≤ L.p0 ∧ 0 ≤ L.p1 ∧ 0 ≤ L.p2 ∧ 0 ≤ L.p3 ∧ 0 ≤ L.p4 ∧
  0 ≤ L.p5 ∧ 0 ≤ L.p6 ∧ 0 ≤ L.p7 ∧ 0 ≤ L.p8 ∧ 0 ≤ L.p9 ∧ 0 ≤ L.p10

/-- Guarded exogenous solar inflow matching `noetic_ledger_solar_in()`:
    adds `amt` simultaneously to `p6` (PATCH) and `p10` (SOLAR_IN). -/
@[expose] public def solarIn11 (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt then { L with p6 := L.p6 + amt, p10 := L.p10 + amt } else L

/-- Guarded transfer on `Ledger11`: ATP (p3) -> Heat (p8). -/
@[expose] public def transfer11AtpToHeat (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p8 := L.p8 + amt } else L

/-- Guarded transfer on `Ledger11`: Patch (p6) -> Gut (p0). -/
@[expose] public def transfer11PatchToGut (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt ∧ amt ≤ L.p6 then { L with p6 := L.p6 - amt, p0 := L.p0 + amt } else L

/-- Guarded transfer on `Ledger11`: ATP (p3) -> Neural Work (p5). -/
@[expose] public def transfer11AtpToNeural (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p5 := L.p5 + amt } else L

/-- Guarded transfer on `Ledger11`: Neural Work (p5) -> Heat (p8). -/
@[expose] public def transfer11NeuralToHeat (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt ∧ amt ≤ L.p5 then { L with p5 := L.p5 - amt, p8 := L.p8 + amt } else L

/-- Guarded transfer on `Ledger11`: ATP (p3) -> Tissue (p4). -/
@[expose] public def transfer11AtpToTissue (L : Ledger11) (amt : Int) : Ledger11 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p4 := L.p4 + amt } else L

/-- Composite open-world 11-slot tick combining exogenous solar inflow (`p6` + `p10`),
    foraging, workspace billing, tissue repair, and basal heat dissipation. -/
@[expose] public def openTick11 (L : Ledger11) (sol bite work repair heat : Int) : Ledger11 :=
  transfer11AtpToHeat
    (transfer11AtpToTissue
      (transfer11NeuralToHeat
        (transfer11AtpToNeural
          (transfer11PatchToGut (solarIn11 L sol) bite)
          work)
        work)
      repair)
    heat

/-- Intentionally unguarded ATP -> Heat transfer used in `AdversarialTests.lean`
    to refute heat monotonicity and non-negativity when the guard is omitted. -/
@[expose] public def unguardedAtpToHeat (L : Ledger11) (amt : Int) : Ledger11 :=
  { L with p3 := L.p3 - amt, p8 := L.p8 + amt }

/-- Valid Seed Firewall partitions: Dev `[1001..1099]`, Confirmatory `[5001..5024]`,
    and Held-Out `[5101..5124]` (excludes `0` and unallocated ranges). -/
@[expose] public def isValidSeed (s : Nat) : Prop :=
  (1001 ≤ s ∧ s ≤ 1099) ∨ (5001 ≤ s ∧ s ≤ 5024) ∨ (5101 ≤ s ∧ s ≤ 5124)

/-- Functional pair-projection wrapper modeling counterfactual observer evaluation:
    returns `(live, forkRun live)` without mutating `live`. -/
@[expose] public def forkedObserverEval {α β : Type} (live : α) (forkRun : α → β) : α × β :=
  (live, forkRun live)

/-- Exact triangular sum `1 + 2 + ... + n` used as the maximum Wilcoxon rank sum. -/
@[expose] public def sum1To : Nat → Nat
  | 0     => 0
  | n + 1 => sum1To n + (n + 1)

/-- Abstract region-guard function: edits targeting regions `0..3` return the input
    value unchanged; regions `≥ 4` add `delta`. -/
@[expose] public def ring0ApplyEdit (protectedState : Int) (region : Nat) (delta : Int) : Int :=
  if region < 4 then protectedState else protectedState + delta

/-- Packed b1.58 ternary 2-bit synaptic encoder on `{-1, 0, +1}`. -/
@[expose] public def packB158 (w : Int) : Nat :=
  if w < 0 then 2 else if w > 0 then 1 else 0

/-- Packed b1.58 ternary 2-bit synaptic decoder onto `{-1, 0, +1}`. -/
@[expose] public def unpackB158 (c : Nat) : Int :=
  if c = 2 then -1 else if c = 1 then 1 else 0

end NoeticProofs
