# Frequently Asked Questions (`docs/faq.md`)

### Does passing `L1_BUTLIN14` (14/14) or `L2_FRAMEWORKS10` (10/10) mean the organism is conscious?
**No.** Every one of those 24 checks (plus 5 mirrored CQM items) is explicitly tagged `EXECUTED_PASS_INDICATOR_ONLY`. They verify that concrete, ablatable C11 state variables (recurrent `sLTC` memory, global workspace broadcast slots, Brier-calibrated confidence readout, attention schema spotlight, Forked-Snapshot `PCI*`) causally affect behavior compared to matched sham, shuffled, and passive controls. They provide zero evidence of subjective qualia, sentience, or moral patienthood.

### Why are 5 checks labeled `PASS_CLASSICAL_PARITY_DISCLOSED`?
Under Policy P7 (anti-inflation honesty), five quantum-inspired checks (`F-092`, `N01`, `N02`, `N50`, `LIFE-L040-A5`) compare bounded $Q \le 8$ registers against equal-parameter classical controls (such as an $SO(2)$ oscillator ring or Philox4x32-10 rollout). Because the classical control ties the quantum-inspired register ($d_z = 0.00$, $p_{\text{Holm}} = 1.00$), we disclose classical parity transparently instead of handicapping the control.

### Why do only 5 Lean kernels run locally on Windows while CI runs 6/6?
In Lean 4.35.0, `lean4lean.exe` cannot parse Windows `.olean` path headers and `lake check` / `lake comparator` require Linux `bubblewrap` (`bwrap`). Locally on Windows we run and archive `lean4`, `leanchecker --fresh`, `leanchecker-paranoid --fresh`, `con-leche --verified`, `con-ron --verified`, and `nanoda_bin` (`results/lean4-multi-kernel.txt`); on Linux (`ubuntu-latest`) CI runs all **6/6** kernels plus `lake check --paranoid` and `lake comparator --config comparator.json --paranoid`.

### Does this repository solve any problem in the Unsolved Problems Mega-Atlas?
**No (`0 / 1,900` retired).** See `docs/MEGA_ATLAS_CROSSCHECK.md`.

### Is this paper peer-reviewed?
**No.** `dist/Donahue_2026_noetic_organism_omega.pdf` is an independent, AI-assisted technical research preprint.
