module

public import NoeticProofs.LiveC11Receipt

/-!
# NoeticProofs.Spec — Pure Shared Definitions Only (Zero Proofs, Zero `sorry`)

Canonical mathematical, thermodynamic, Ring-0 ACL, packed b1.58, and statistical
definitions for NOETIC ORGANISM OMEGA v2.1.0 shared identically by BOTH:
  - `Challenge.lean` (isolated statement-only specification — imports ONLY this module)
  - `NoeticProofs.lean` & `Solution.lean` (sorry-free proofs verified across 6 kernels)

Author: Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA; ORCID 0009-0006-8168-9648)
License: Apache-2.0
-/

namespace NoeticProofs

/-- 10-pool exact integer thermodynamic state matching `NoeticLedger.pools[0..9]` in C11. -/
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

/-- Exact total enthalpy sum across all 10 conservative & dissipative pools (micro-quanta). -/
@[expose] public def poolSum (L : Ledger10) : Int :=
  L.p0 + L.p1 + L.p2 + L.p3 + L.p4 + L.p5 + L.p6 + L.p7 + L.p8 + L.p9

/-- Non-negativity invariant across all 10 integer energy pools. -/
@[expose] public def nonNeg (L : Ledger10) : Prop :=
  0 ≤ L.p0 ∧ 0 ≤ L.p1 ∧ 0 ≤ L.p2 ∧ 0 ≤ L.p3 ∧ 0 ≤ L.p4 ∧
  0 ≤ L.p5 ∧ 0 ≤ L.p6 ∧ 0 ≤ L.p7 ∧ 0 ≤ L.p8 ∧ 0 ≤ L.p9

/-- Atomic guarded transfer: ATP (p3) -> irreversible physical heat sink (p8). -/
@[expose] public def transferAtpToHeat (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p8 := L.p8 + amt } else L

/-- Atomic guarded transfer: environmental Patch (p6) -> Gut (p0). -/
@[expose] public def transferPatchToGut (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p6 then { L with p6 := L.p6 - amt, p0 := L.p0 + amt } else L

/-- Atomic guarded transfer: ATP (p3) -> Neural Work (p5). -/
@[expose] public def transferAtpToNeural (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p5 := L.p5 + amt } else L

/-- Atomic guarded transfer: Neural Work (p5) -> irreversible physical heat sink (p8). -/
@[expose] public def transferNeuralToHeat (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p5 then { L with p5 := L.p5 - amt, p8 := L.p8 + amt } else L

/-- Atomic guarded transfer: ATP (p3) -> autopoietic Tissue repair (p4). -/
@[expose] public def transferAtpToTissue (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt ∧ amt ≤ L.p3 then { L with p3 := L.p3 - amt, p4 := L.p4 + amt } else L

/-- Guarded exogenous solar inflow into environmental Patch (p6). -/
@[expose] public def addSolarToPatch (L : Ledger10) (amt : Int) : Ledger10 :=
  if 0 ≤ amt then { L with p6 := L.p6 + amt } else L

/-- Composite closed-world organism tick combining foraging, workspace billing, repair, and basal heat. -/
@[expose] public def closedTick (L : Ledger10) (bite work repair heat : Int) : Ledger10 :=
  transferAtpToHeat
    (transferAtpToTissue
      (transferNeuralToHeat
        (transferAtpToNeural (transferPatchToGut L bite) work)
        work)
      repair)
    heat

/-- Forked-Snapshot PCI* observer wrapper: applies counterfactual probe strictly to a copy. -/
@[expose] public def forkedObserverEval {α β : Type} (live : α) (forkRun : α → β) : α × β :=
  (live, forkRun live)

/-- Exact triangular sum 1 + 2 + ... + n used for exact DP Wilcoxon rank-sum upper bound. -/
@[expose] public def sum1To : Nat → Nat
  | 0     => 0
  | n + 1 => sum1To n + (n + 1)

/-- Hardware Ring-0 Read-Only ACL guard: regions 0..3 are strictly immutable under self-edits. -/
@[expose] public def ring0ApplyEdit (protectedState : Int) (region : Nat) (delta : Int) : Int :=
  if region < 4 then protectedState else protectedState + delta

/-- Packed b1.58 ternary 2-bit synaptic encoder on {-1, 0, +1}. -/
@[expose] public def packB158 (w : Int) : Nat :=
  if w < 0 then 2 else if w > 0 then 1 else 0

/-- Packed b1.58 ternary 2-bit synaptic decoder onto {-1, 0, +1}. -/
@[expose] public def unpackB158 (c : Nat) : Int :=
  if c = 2 then -1 else if c = 1 then 1 else 0

end NoeticProofs
