#!/usr/bin/env node
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Alexander Stephen Donahue
/**
 * Automated CLI contract, determinism, negative-input, and property-based (`prop_check`)
 * test suite for `noetic-organism-omega v2.1.0`.
 */

import assert from "node:assert/strict";
import { spawnSync, spawn } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import readline from "node:readline";
import { fileURLToPath } from "node:url";

const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const BIN = process.platform === "win32"
  ? path.join(ROOT, "bin", "noetic_engine.exe")
  : path.join(ROOT, "bin", "noetic_engine_linux");

assert.ok(fs.existsSync(BIN), `Compiled engine binary not found at ${BIN}`);

// 1. Contract & Anti-LARP Mutant Suite (10/10)
const unitRes = spawnSync(BIN, ["test"], { encoding: "utf8" });
assert.equal(unitRes.status, 0, `noetic_engine test failed:\n${unitRes.stdout}\n${unitRes.stderr}`);
assert.match(unitRes.stdout, /SUMMARY: 10\/10 checks passed \(0 failures\)/);

// 2. Negative CLI contract test (unknown subcommand must return exit code 2)
const badRes = spawnSync(BIN, ["nonexistent-subcommand"], { encoding: "utf8" });
assert.equal(badRes.status, 2, `Expected exit 2 for unknown command, got ${badRes.status}`);

// 3. Deterministic property-based verification (`prop_check`) over stream ABI
function runStreamBatch(commands) {
  return new Promise((resolve, reject) => {
    const child = spawn(BIN, ["stream"], { stdio: ["pipe", "pipe", "pipe"] });
    const rl = readline.createInterface({ input: child.stdout });
    const lines = [];
    rl.on("line", (ln) => lines.push(JSON.parse(ln)));
    child.on("error", reject);
    child.on("close", (code) => {
      if (code !== 0) reject(new Error(`stream exited with ${code}`));
      else resolve(lines);
    });
    for (const c of commands) child.stdin.write(c + "\n");
    child.stdin.write("quit\n");
    child.stdin.end();
  });
}

async function prop_check(trials = 32) {
  let lcg = 0x5EED210n;
  const nextU32 = () => {
    lcg = (lcg * 6364136223846793005n + 1442695040888963407n) & ((1n << 64n) - 1n);
    return Number((lcg >> 32n) & 0xFFFFFFFFn);
  };

  const cmdsA = [];
  const cmdsB = [];
  for (let i = 0; i < trials; i++) {
    const seed = 5001 + (nextU32() % 24);
    const arm = nextU32() % 5;
    const mask = arm === 2 ? (1 << (nextU32() % 18)) : 0;
    const steps = 1 + (nextU32() % 20);
    cmdsA.push(`init ${seed} ${arm} ${mask}`, `step ${steps}`, "pci");
    cmdsB.push(`init ${seed} ${arm} ${mask}`, `step ${steps}`, "pci");
  }

  const [outA, outB] = await Promise.all([runStreamBatch(cmdsA), runStreamBatch(cmdsB)]);
  assert.equal(outA.length, trials * 3);
  assert.equal(outB.length, trials * 3);

  for (let i = 0; i < outA.length; i++) {
    const a = outA[i];
    const b = outB[i];
    assert.equal(a.version, "2.2.0");
    // Property 1: Twin-run 0-bit SHA-256 determinism
    assert.equal(a.sha256, b.sha256, `Determinism mismatch at frame ${i}`);
    // Property 2: Exact integer First-Law conservation (0 uq residual)
    assert.equal(a.energy_residual_uq, 0, `Non-zero energy residual at frame ${i}: ${a.energy_residual_uq}`);
    // Property 3: Exact Second-Law monotonicity (0 violations)
    assert.equal(a.second_law_violations, 0, `Second-law violation at frame ${i}`);
    // Property 4: Forked-Snapshot PCI* observer non-interference & non-ceiling guard
    assert.equal(a.pci_clean, 1, `PCI* observer interfered at frame ${i}`);
    assert.ok(a.pci_star >= 0.0 && a.pci_star <= 0.9501, `PCI* out of bounds: ${a.pci_star}`);
    // Property 5: Non-negative energy pools
    for (const [k, v] of Object.entries(a.ledger)) {
      assert.ok(v >= 0, `Negative pool ${k}=${v} at frame ${i}`);
    }
  }
  console.log(`[PASS] prop_check: ${trials} randomized seed/arm/ablation trials (${outA.length} frames) verified bit-exact, 0 uq drift, 0 2nd-law violations, 0-bit PCI* non-interference.`);
}

await prop_check(32);
console.log("[PASS] noetic_prop.test.mjs: all contract, negative, determinism & property-based checks passed.");
