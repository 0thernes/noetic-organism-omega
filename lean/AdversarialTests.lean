module

public import NoeticProofs.Spec

/-!
# NOETIC ORGANISM OMEGA v2.2.0 — Adversarial Refutations & Negative Boundary Proofs (`AdversarialTests.lean`)

Eight concrete false-variant refutations (`A01..A08`) proving that relaxing the guards
or hypotheses of our `Spec.lean` definitions immediately breaks the claimed invariants.
Zero `sorry`, zero custom axioms, wired into `Challenge.lean`, `Solution.lean`,
`VerifyAxioms.lean`, and `comparator.json`.

Author: Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA; ORCID 0009-0006-8168-9648)
License: Apache-2.0
-/

namespace NoeticProofs

-- A01: Adversarial refutation: unguarded ATP -> Heat transfer violates heat-field monotonicity (amt = -1)
public theorem A01_refute_unguarded_transfer_monotone_heat :
    ¬ (∀ (L : Ledger11) (amt : Int), L.p8 ≤ (unguardedAtpToHeat L amt).p8) := by
  intro h
  have hbad := h ⟨0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10⟩ (-1)
  exact (by unfold unguardedAtpToHeat; decide :
    ¬ ((0 : Int) ≤ (unguardedAtpToHeat ⟨0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10⟩ (-1)).p8)) hbad

-- A02: Adversarial refutation: unguarded transfer violates non-negativity on overdraw (amt = 1 > p3 = 0)
public theorem A02_refute_unguarded_transfer_nonneg :
    ¬ (∀ (L : Ledger11) (amt : Int), nonNeg11 L → nonNeg11 (unguardedAtpToHeat L amt)) := by
  intro h
  have hnn : nonNeg11 ⟨0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0⟩ := by unfold nonNeg11; decide
  have hbad := h ⟨0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0⟩ 1 hnn
  exact (by unfold unguardedAtpToHeat nonNeg11; decide :
    ¬ nonNeg11 (unguardedAtpToHeat ⟨0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0⟩ 1)) hbad

-- A03: Adversarial refutation: user-space region 4 IS mutable under ring0ApplyEdit
public theorem A03_refute_region4_immutable :
    ¬ (∀ (st delta : Int), ring0ApplyEdit st 4 delta = st) := by
  intro h
  have hbad := h 100 1
  exact (by unfold ring0ApplyEdit; decide : ring0ApplyEdit 100 4 1 ≠ 100) hbad

-- A04: Adversarial refutation: unmasked/out-of-domain b1.58 input (w = 2) fails roundtrip
public theorem A04_refute_unrestricted_b158_roundtrip :
    ¬ (∀ w : Int, unpackB158 (packB158 w) = w) := by
  intro h
  have hbad := h 2
  exact (by unfold unpackB158 packB158; decide : unpackB158 (packB158 2) ≠ 2) hbad

-- A05: Adversarial refutation: imbalanced ledger debit/credit (-10 / +11) breaks 11-slot conservation
public theorem A05_refute_bad_ledger11_transfer_conserves :
    ¬ (∀ L : Ledger11,
        residual11 { L with p3 := L.p3 - 10, p8 := L.p8 + 11 } = residual11 L) := by
  intro h
  let L0 : Ledger11 := ⟨0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10⟩
  have hbad := h L0
  exact (by unfold residual11 poolSum11; decide :
    residual11 { L0 with p3 := L0.p3 - 10, p8 := L0.p8 + 11 } ≠ residual11 L0) hbad

-- A06: Adversarial refutation: seed = 0 is strictly rejected by the Seed Firewall partition predicate
public theorem A06_refute_zero_seed_valid :
    ¬ isValidSeed 0 := by
  unfold isValidSeed
  decide

-- A07: Adversarial refutation: injecting solar energy into p6 without incrementing p10 breaks residual11
public theorem A07_refute_solar_without_p10_preserves_residual :
    ¬ (∀ L : Ledger11, residual11 { L with p6 := L.p6 + 100 } = residual11 L) := by
  intro h
  let L0 : Ledger11 := ⟨0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0⟩
  have hbad := h L0
  exact (by unfold residual11 poolSum11; decide :
    residual11 { L0 with p6 := L0.p6 + 100 } ≠ residual11 L0) hbad

-- A08: Adversarial refutation: upper-bound <= 950 alone on signed Int does NOT guarantee non-negativity
public theorem A08_refute_negative_score_nonneg :
    ¬ (∀ score_milli : Int, score_milli ≤ 950 → 0 ≤ score_milli) := by
  intro h
  have hbad := h (-1) (by decide)
  exact (by decide : ¬ ((0 : Int) ≤ -1)) hbad

end NoeticProofs
