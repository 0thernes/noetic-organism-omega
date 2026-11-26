# System Architecture (`docs/architecture.md`)

`noetic-organism-omega v2.1.0` is a portable, zero-dependency C11 whole-organism simulation engine coupled with a 54-theorem 6-kernel Palomar Lean 4.35.0 formal verification suite and a loopback-only Node.js Web Observatory.

## 1. Module Map (`src/` and `include/`)

| File | Responsibility |
|---|---|
| `include/bare_crt.h` | Bare Win64/POSIX portable C11 types and timing shims |
| `include/noetic_abi.h` | Canonical `NoeticWorld` (`6,776 B`), `NoeticLedger` (11 pools), 18 `NoeticAblationBit` flags, 5 control arms, 5 Anti-LARP mutants, suite APIs |
| `src/noetic_rng.c` | Counter-based `Philox4x32-10` 8-tuple domain-isolated PRNG, portable IEEE-754 `exp_det`/`log_det`/`tanh_det`, SHA-256, exact 11-pool `int64_t` ledger |
| `src/noetic_stats.c` | Exact dynamic-programming Wilcoxon signed-rank test, paired bootstrap `d_z` 95% CIs, Holm-Bonferroni step-down FWER |
| `src/noetic_organism.c` | Zero-alloc `noetic_world_step()`, 18 ablatable circuits, Forked-Snapshot `PCI*`, 128B AUTOPHAGE & 112-trait Cellulogonic ABI ingestion |
| `src/noetic_gates_core.c` | Clean-Slate v3.0 native release gates (`GATE-02, 05, 06, 07, 09, 10, 11`): XPBD soft hydrostat, `b1.58` ganglia, Ring-0 ACL + WSE-12, Blackiston metamorphosis |
| `src/noetic_hw24.c` | `L9_HW24` (24 hardware, arena, CSR, `b1.58`, `Q<=10`, 5-gear, SeqLock, FP64, watchdog checks) |
| `src/noetic_soft40.c` | `L8_SOFT40` (40 XPBD, Chemoton, wet biochemistry, evo-devo, and carcass lysis checks) |
| `src/noetic_freak100.c` | `L4_FREAK100` (100 mechanistic checks; `F-092` honest classical parity disclosure) |
| `src/noetic_novel100.c` | `L6_NOVEL100` (100 frontier checks; `N01, N02, N50` honest classical parity disclosures) |
| `src/noetic_life500.c` | `L5_LIFE500` (100 biological domains × 5 structural pathways = 500 checks; `LIFE-L040-A5` honest parity disclosure) |
| `src/noetic_milestones100.c` | `M001..M100` acyclic bottom-up stage-0..8 DAG verifier (`First-15` playable `LOCKED_PASS`) |
| `src/noetic_eval.c` | M7 unit + 5 Anti-LARP mutants, M8 E1 (18 ablations), E2 (49-item cognitive/CQM), E3 (scaling), and live C11-to-Lean Palomar `module` receipt exporter |
| `src/main_cli.c` | CLI entrypoint (`test`, `gates`, `hw24`, `soft40`, `freak100`, `novel100`, `life500`, `milestones100`, `e1`, `e2`, `e3`, `export-lean`, `stream`) |

## 2. Formal Proof Architecture (`lean/`)

- `NoeticProofs/LiveC11Receipt.lean`: Live execution receipt emitted by `noetic_engine export-lean 5001` (`module` + `@[expose] public def`).
- `NoeticProofs/Spec.lean`: Pure shared definitions (`Ledger10`, guarded transfers, `closedTick`, `forkedObserverEval`, `sum1To`, `ring0ApplyEdit`, `packB158`/`unpackB158`) with zero proofs and zero `sorry`.
- `NoeticProofs.lean`: 54 sorry-free `public theorem` proofs (`N01..N48` core + `N49..N54` non-vacuity and sharp negative refutations).
- `Challenge.lean` & `Solution.lean` & `comparator.json`: Isolated Palomar statement-vs-solution contract for 6-kernel judging.
- `VerifyAxioms.lean`: Standalone `#print axioms` audit entrypoint checked against `README.md` by `scripts/check_readme_axioms.py`.

## 3. Loopback Observatory Architecture (`web/`)

`web/server.mjs` spawns `noetic_engine stream` as an isolated child OS worker process off the Node HTTP event loop (`HW13`), exposes `GET /healthz`, `GET /api/*` (read-only receipts & live state), and `POST /api/cmd` strictly on `127.0.0.1:8795` with loopback host/origin/`Sec-Fetch-Site` enforcement, strict CSP headers, and a 16-command queue cap.
