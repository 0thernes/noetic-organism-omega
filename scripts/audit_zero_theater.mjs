import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
let fails = 0;

function check(label, ok, detail) {
  console.log(`  [${ok ? "PASS" : "FAIL"}] ${label}: ${detail}`);
  if (!ok) fails++;
}

// 1. Zero-theater scan in C11 organism & eval readout
const evalSrc = fs.readFileSync(path.join(ROOT, "src", "noetic_eval.c"), "utf8");
const readoutSlice = evalSrc.split("static double readout_metric(")[1]?.split("static uint64_t locked_seed_at(")[0] || "";
const armInReadout = /w->arm|NOETIC_ARM_|ablation_mask/.test(readoutSlice);
check("Zero arm/ablation branches inside readout_metric()", !armInReadout, armInReadout ? "FOUND arm branch!" : "0 hits");

const orgSrc = fs.readFileSync(path.join(ROOT, "src", "noetic_organism.c"), "utf8");
const bannedPatterns = ["arm5(", "shamJitter", "lesionJitter", "evalBinding('lesion') *"];
const hits = bannedPatterns.filter((p) => orgSrc.includes(p));
check("Zero synthetic wrapper patterns in noetic_organism.c", hits.length === 0, hits.length ? hits.join(", ") : "0 hits");

// 2. Lean 4 zero sorry/admit/axiom/bv_decide scan
const leanSrc = fs.readFileSync(path.join(ROOT, "lean", "NoeticProofs.lean"), "utf8")
  .replace(/\/-[\s\S]*?-\//g, "")
  .replace(/--.*$/gm, "");
const leanBad = /\b(sorry|admit|axiom)\b|bv_decide|native_decide/.test(leanSrc);
check("Pure Lean 4 source free of sorry/admit/axiom/bv_decide", !leanBad, leanBad ? "FOUND forbidden token!" : "clean");

// 3. Verify M8 E1/E2/E3 empirical JSON receipts exist and have 0 energy drift
const e1 = JSON.parse(fs.readFileSync(path.join(ROOT, "results", "m8_e1_ablation_sweep.json"), "utf8"));
const e2 = JSON.parse(fs.readFileSync(path.join(ROOT, "results", "m8_e2_cognitive_49.json"), "utf8"));
const e3 = JSON.parse(fs.readFileSync(path.join(ROOT, "results", "m8_e3_scaling_bench.json"), "utf8"));

check("M8 E1 18-circuit ablation sweep (0 drift)", e1.effects === 18 && e1.max_energy_residual_uq === 0 && e1.second_law_violations === 0,
  `${e1.effects}/${e1.total} EFFECT, drift=${e1.max_energy_residual_uq}`);
check("M8 E2 49-item Cognitive/CQM Battery (0 drift, 5/5 mirrors)",
  e2.summary.butlin14_passed === 14 && e2.summary.theories10_passed === 10 &&
  e2.summary.cqm25_passed === 25 && e2.summary.mirrors_verified === 5 &&
  e2.max_energy_residual_uq === 0 && e2.second_law_violations === 0,
  `Butlin ${e2.summary.butlin14_passed}/14, Theories ${e2.summary.theories10_passed}/10, CQM ${e2.summary.cqm25_passed}/25, MIR ${e2.summary.mirrors_verified}/5`);
check("M8 E3 scaling benchmark 6 tiers (0 drift)", e3.rows.length === 6 && e3.rows.every((r) => r.max_energy_residual_uq === 0),
  `${e3.rows.length} scale tiers (N=1..4096), max drift=0`);

if (fails > 0) {
  console.error(`AUDIT FAILED (${fails} failure(s))`);
  process.exit(1);
}
console.log("ZERO-THEATER & M7/M8 RECEIPT AUDIT: 100% PASS!");
