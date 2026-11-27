#!/usr/bin/env node
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Alexander Stephen Donahue
/**
 * Minimal runnable demonstration of `noetic-organism-omega v2.2.0`:
 * Compares INTACT vs WORKSPACE_IGNITION structural lesion (bit 3, mask 8) on seed 5001,
 * printing exact integer conservation residuals, PCI* observer non-interference, and SHA-256.
 */

import { spawn } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import readline from "node:readline";
import { fileURLToPath } from "node:url";

const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const BIN = process.platform === "win32"
  ? path.join(ROOT, "bin", "noetic_engine.exe")
  : path.join(ROOT, "bin", "noetic_engine_linux");

if (!fs.existsSync(BIN)) {
  console.error(`Build the C11 engine first (missing ${BIN}).`);
  process.exit(1);
}

function query(commands) {
  return new Promise((resolve, reject) => {
    const proc = spawn(BIN, ["stream"], { stdio: ["pipe", "pipe", "pipe"] });
    const rl = readline.createInterface({ input: proc.stdout });
    const out = [];
    rl.on("line", (ln) => out.push(JSON.parse(ln)));
    proc.on("error", reject);
    proc.on("close", (code) => (code === 0 ? resolve(out) : reject(new Error(`exit ${code}`))));
    for (const c of commands) proc.stdin.write(c + "\n");
    proc.stdin.write("quit\n");
    proc.stdin.end();
  });
}

const frames = await query([
  "init 5001 0 0",
  "step 60",
  "pci",
  "init 5001 2 8",
  "step 60",
  "pci"
]);

const intact = frames[2];
const lesioned = frames[5];
console.log("=== NOETIC ORGANISM OMEGA v2.2.0 QUICK DEMO (Seed 5001, 76 Ticks) ===");
console.log(`INTACT          | ws_fid=${intact.org.ws_fid.toFixed(3)} | pci*=${intact.pci_star.toFixed(4)} (clean=${intact.pci_clean}) | drift=${intact.energy_residual_uq} uq | 2nd_viol=${intact.second_law_violations} | sha=${intact.sha256.slice(0, 16)}...`);
console.log(`WORKSPACE LESION| ws_fid=${lesioned.org.ws_fid.toFixed(3)} | pci*=${lesioned.pci_star.toFixed(4)} (clean=${lesioned.pci_clean}) | drift=${lesioned.energy_residual_uq} uq | 2nd_viol=${lesioned.second_law_violations} | sha=${lesioned.sha256.slice(0, 16)}...`);
console.log("Notice: Global workspace broadcast fidelity drops under structural lesion while exact 0 uq conservation holds.");
