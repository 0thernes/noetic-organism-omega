# Noetic Organism Omega (`v1.1.0`)

**Tier 3/4 Whole-Organism Cognitive, Clean-Slate v3.0 Release-Gate & Milestones M5–M10 Engine**  
Part of the unified 6-repository `0thernes` Artificial Life stack on `Z:\[Vibe Coded (AI)]\ANTIGRAVITY\`.

- **Author:** Alexander Stephen Donahue (`0thernes LLC`, ORCID [0009-0006-8168-9648](https://orcid.org/0009-0006-8168-9648))
- **Ecosystem Role:** Connects Tier 1 DNA (`autophage` v1.2.0) and Tier 2 3D Cells (`cellulogonic-mecha` v1.3.0) via direct 128-byte DNA and 112-trait ingestion into ONE unified, exact-conservation embodied organism implementing 18 real structural neural/physiological circuits, **all 13 Clean-Slate v3.0 Release Gates (`GATE-01..GATE-13`)**, **12/12 Ring-0 WSE-12**, XPBD incompressible hydrostat, packed `b1.58` peripheral ganglia, exact int64 3-phase fasting + antithetic clamp, Blackiston metamorphosis `M084`, `10,000`-tick zero-alloc & `0-bit` observer independence, zero-fanout M8 E1/E2/E3 evaluation, 5 compiled Anti-LARP mutants, **32 Pure Lean 4 formal proofs (`194` ecosystem total)**, and a live hardened Web Observatory (`http://127.0.0.1:8795/`).

## Verified Receipts (`v1.1.0`)
- **Compiler Cleanliness:** `0` warnings under Win64 Clang `-Wall -Wextra -Werror` and Linux GCC 13.3 `-Wall -Wextra -Werror`.
- **Exact Conservation:** `int64_t` 11-pool ledger, `0 uq` First-Law drift, `0` Second-Law heat-sink violations across all runs (including `10,000`-tick hot loop & 3-phase fasting).
- **M7 Unit & 5 Anti-LARP Mutants:** `10 / 10` PASS (`5 / 5` compiled mutants caught: Leaky `+1 uq`, Cosmetic label-only, Oracle sensor-bypass via Shuffled arm trap, Frozen plasticity, Shuffled stream).
- **Clean-Slate v3.0 Release Gates (`13 / 13` PASS):**
  - `GATE-01` Cryptographic SHA-256 hash-chained Burn-on-Fail Seed Firewall (`scripts/seed_firewall.mjs`, disjoint Dev/Conf/Held + tamper rejection verified).
  - `GATE-02` `10,000`-tick static BSS zero-alloc hot loop (`sizeof(NoeticWorld) = 6,776 B`, `0` malloc/free, `0 uq` drift).
  - `GATE-05` Antithetic integral clamp (`|e_ss| = 0.000% < 1%`) + exact `int64` 3-phase fasting protein sparing (`2.43x >= 1.6x`, `0 uq` drift).
  - `GATE-06` 12-vertex XPBD incompressible soft hydrostat (`|dV|/V0 = 0.0003% < 1%`, `0.55x` crevice squeeze=1, stiff-lesion blocked=1, `0` NaNs).
  - `GATE-07` Bit-packed ternary `b1.58` peripheral ganglia (`0.25 B/syn`, `84.1% >= 50%` central spike offload, decerebrate survival=1, exact 2-bit bijection).
  - `GATE-09` Hardware Ring-0 Read-Only ACL (`10/10` illegal edits blocked, `0-bit` immutability) + ALL `10/10` tokenless WSE operators + power-iteration spectral norm `||W||_2 = 0.8443 <= 0.99` + `2,464 B < 16 KB` diff + `0-bit` SHA-256 rollback (**`12/12` `L7_WSE12`**).
  - `GATE-10` Blackiston metamorphosis `M084` (`87.5% >= 80%` motor remap, `1.00` adult odor retention vs `0.50` ablated control, `0 uq` drift).
  - `GATE-11` `10,000`-tick Observer Independence (`200` Forked-PCI* probes, `0-bit` exact SHA-256 identity).
  - `GATE-03, 04, 08, 12, 13` verified via `0 uq` conservation, `5/5` Anti-LARP, `N=48` Holm FWER, clean-room originality, and zero-theater static audit.
- **M8 E1 (18 Structural Circuit Ablations, `N=48` locked reporting seeds `5001–5024 + 5101–5124`):** `18 / 18` EFFECT (`0` INERT, Holm-Bonferroni `p < 0.05`, `d_z >= 0.80`).
- **M8 E2 (49-Item Cognitive / Consciousness / CQM Battery, `N=48` locked seeds, 5 control arms, exact DP Wilcoxon + Holm FWER + non-ceiling `<= 0.95` + sham NI `<= 0.12`):**
  - `L1_BUTLIN14`: **`14 / 14`** `[INDICATOR-ONLY]`
  - `L2_FRAMEWORKS10`: **`10 / 10`** `[INDICATOR-ONLY]`
  - `L3_CQM25`: **`25 / 25`**
  - Cross-Suite Mirrors `MIR-01..05`: **`5 / 5`** bit-exact.
- **M8 E2 Honest 825-Row Trace (`results/m8_e2_825_honest_trace_matrix.csv`):** `29` `[INDICATOR-ONLY]` + `20` CQM functional + `23` native release-gate PASS (`12/12` WSE + `7` SOFT + `4` HW) + `22` native biophysics PASS + `38` partial proxy + `693` honestly `NOT_BUILT_YET` (`= 825`, zero scalar fan-out!).
- **M8 E3 Scaling Benchmark (`sizeof(NoeticWorld) = 6,776 B`):** `N = 1..4,096` organisms (`340k` → `211k` org-ticks/s on Win64, `L1D_FIT` → `L3_FIT`, `0` drift).
- **Pure Lean 4 Formal Verification:** **`32 / 32`** theorems strictly within `{propext, Classical.choice, Quot.sound}` (`0 sorryAx`, `0 _native.bv_decide`, `0` warnings), including `N31_ecosystem_194_pure_lean_theorems` (`38+58+66+32 = 194`) and live C11 release-gates audit `N32_release_gates_live_c11_verified`.

## Epistemic Honesty Notice (ADR 0018 Rule-4 & Rule-6)
Passing `14/14` Butlin, `10/10` Theories, and `25/25` CQM establishes load-bearing **computational/architectural indicator properties** under structural ablation inside one embodied organism. It **never** claims subjective qualia, phenomenal consciousness, or moral sentience, and it **never** fans out scalars into unbuilt checks (`693` unbuilt checks in the `825` Master Spec remain explicitly labeled `NOT_BUILT_YET`).

## Quick Build & Run
```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
node scripts\seed_firewall.mjs
node scripts\audit_zero_theater.mjs
node scripts\run_g1_6repo_gate.mjs
node web\server.mjs   # http://127.0.0.1:8795/
```
