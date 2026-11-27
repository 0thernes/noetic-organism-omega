#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Generate WCAG 2.2 AA Strict-CSP Zero-CDN Interactive Lean 4 Blueprint (`docs/blueprint.html`),
SPDX `LICENSES/` copies, `llms-full.txt`, FAIR `croissant.json` & `ro-crate-metadata.json`,
and LF-normalized `data/MANIFEST.sha256` & `CHECKSUMS.sha256` directly from verified artifacts."""

from __future__ import annotations

import hashlib
import html
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

BOOKKEEPING = {
    "N12_master_825_dedup_exact", "N13_cognitive_49_dedup_exact", "N16_wilcoxon_n48_exact",
    "N23_ecosystem_186_pure_lean_theorems", "N27_wse_diff_budget_bounded",
    "N31_ecosystem_194_pure_lean_theorems", "N34_five_gear_60s_exact_divisors",
    "N38_b158_5trit_radix_fits_u8", "N39_xls_shard_below_excel_limit",
    "N40_suite_825_exact_partition", "N41_suite_825_minus_5_mirrors",
    "N42_milestones_100_stage_partition", "N43_first15_plus_85_eq_100",
    "N44_hybrid_cube_1000_exact", "N45_substrates_75_exact",
    "N46_compiled_mutants_35_exact", "N47_ecosystem_224_pure_lean_theorems",
    "N52_refute_quantum_cap_11_fits_16384",
}

RECEIPT_CHECKS = {
    "N24_live_c11_receipt_verified",
    "N32_release_gates_live_c11_verified",
    "N48_live_c11_full_825_and_100m_verified",
    "N59_ledger11_live_receipt_zero_residual",
}


def sha256_file(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def classify(short: str) -> tuple[str, str, str, str]:
    if short.startswith("A"):
        return ("adversarial", "Adversarial Refutation", "lean/AdversarialTests.lean", "Spec.lean (Ledger11, ring0ApplyEdit, packB158, isValidSeed)")
    if short in RECEIPT_CHECKS:
        return ("receipt", "Live C11 Receipt Check", "lean/NoeticProofs.lean", "NoeticProofs.LiveC11Receipt (seed 5001)")
    if short in BOOKKEEPING:
        return ("bookkeeping", "Bookkeeping Arithmetic", "lean/NoeticProofs.lean", "Closed numeral literal (certifies no runtime behavior)")
    if short.startswith(("N55_", "N56_", "N57_", "N58_")):
        return ("ledger11", "11-Slot Ledger11 Model", "lean/NoeticProofs.lean", "Spec.lean (Ledger11, residual11, openTick11)")
    if short.startswith(("N01_", "N02_", "N03_", "N04_", "N05_", "N06_", "N07_", "N08_", "N09_", "N10_", "N36_", "N37_", "N49_", "N54_")):
        return ("ledger10", "10-Field Ledger10 Model", "lean/NoeticProofs.lean", "Spec.lean (Ledger10, poolSum, closedTick)")
    return ("helpers", "Encodings, Guard & Stats", "lean/NoeticProofs.lean", "Spec.lean (ring0ApplyEdit, packB158, sum1To, forkedObserverEval)")


def main() -> None:
    # 1. SPDX LICENSES/ directory (LF normalized)
    lic_dir = ROOT / "LICENSES"
    lic_dir.mkdir(exist_ok=True)
    (lic_dir / "Apache-2.0.txt").write_text(
        (ROOT / "LICENSE").read_text(encoding="utf-8").replace("\r\n", "\n"),
        encoding="utf-8",
        newline="\n",
    )
    (lic_dir / "CC-BY-4.0.txt").write_text(
        (ROOT / "LICENSE-DOCS").read_text(encoding="utf-8").replace("\r\n", "\n"),
        encoding="utf-8",
        newline="\n",
    )
    print("Wrote LICENSES/Apache-2.0.txt and LICENSES/CC-BY-4.0.txt")

    # 2. Generate docs/blueprint.html from lean/comparator.json & results/lean4-noetic-audit.txt
    comp = json.loads((ROOT / "lean" / "comparator.json").read_text(encoding="utf-8"))
    audit_lines = [
        ln.strip()
        for ln in (ROOT / "results" / "lean4-noetic-audit.txt").read_text(encoding="utf-8").splitlines()
        if ln.strip()
    ]
    axiom_map: dict[str, str] = {}
    for ln in audit_lines:
        m0 = re.match(r"^'([^']+)' does not depend on any axioms$", ln)
        if m0:
            axiom_map[m0.group(1)] = "Zero axioms"
            continue
        m1 = re.match(r"^'([^']+)' depends on axioms: \[([^\]]+)\]$", ln)
        if m1:
            axiom_map[m1.group(1)] = m1.group(2)

    rows_html: list[str] = []
    for idx, full in enumerate(comp["theorem_names"], 1):
        short = full.split(".", 1)[-1]
        cat_key, cat_label, mod_file, dep_txt = classify(short)
        ax = axiom_map.get(full, "UNKNOWN")
        ax_cls = "ax-zero" if ax == "Zero axioms" else ("ax-choice" if "Classical.choice" in ax else "ax-pq")
        gh_url = f"https://github.com/0thernes/noetic-organism-omega/blob/main/{mod_file}"
        rows_html.append(
            f'<tr data-cat="{cat_key}">'
            f"<td>{idx}</td>"
            f'<td><a href="{gh_url}"><code>{html.escape(short)}</code></a></td>'
            f'<td><span class="pill pill-{cat_key}">{html.escape(cat_label)}</span></td>'
            f'<td><span class="pill {ax_cls}">{html.escape(ax)}</span></td>'
            f"<td><code>{html.escape(mod_file)}</code></td>"
            f"<td>{html.escape(dep_txt)}</td>"
            f"</tr>"
        )

    bp_html = f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="Content-Security-Policy" content="default-src 'none'; style-src 'unsafe-inline'; script-src 'self'; img-src 'self' data:; font-src 'self'; base-uri 'none'; form-action 'none'">
  <title>Interactive Lean 4.35.0 Proof Blueprint (68 Theorems) — Noetic Organism Omega v2.2.0</title>
  <style>
    :root {{
      --bg: #0f172a; --surface: #1e293b; --surface-2: #0b1222; --border: #334155;
      --text: #f1f5f9; --muted: #cbd5e1; --accent: #38bdf8; --green: #34d399;
      --amber: #fbbf24; --violet: #c084fc; --rose: #fb7185;
    }}
    * {{ box-sizing: border-box; }}
    body {{
      margin: 0; font-family: system-ui, -apple-system, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: var(--bg); color: var(--text); line-height: 1.55;
    }}
    .skip-link {{
      position: absolute; left: -9999px; top: 0;
      background: #ffffff; color: #0f172a; padding: 0.75rem 1rem; font-weight: 700; z-index: 1000;
    }}
    .skip-link:focus {{ left: 0.5rem; top: 0.5rem; }}
    a {{ color: var(--accent); text-underline-offset: 2px; }}
    a:focus-visible, button:focus-visible, input:focus-visible, .dag-node:focus-visible {{
      outline: 3px solid var(--amber); outline-offset: 2px;
    }}
    header {{
      background: linear-gradient(135deg, #0b1222 0%, #1e293b 100%);
      border-bottom: 1px solid var(--border); padding: 1.75rem 1.25rem;
    }}
    .wrap {{ max-width: 1200px; margin: 0 auto; }}
    nav[aria-label="Primary"] {{ display: flex; flex-wrap: wrap; gap: 0.65rem; margin-bottom: 1rem; }}
    .nav-btn {{
      display: inline-block; padding: 0.4rem 0.85rem; border-radius: 6px;
      border: 1px solid var(--border); background: var(--surface); color: var(--text);
      text-decoration: none; font-weight: 600; font-size: 0.9rem;
    }}
    .nav-btn:hover {{ border-color: var(--accent); }}
    h1 {{ margin: 0 0 0.45rem; font-size: clamp(1.5rem, 2.8vw, 2.1rem); }}
    .subtitle {{ margin: 0; color: var(--muted); max-width: 85ch; }}
    main {{ padding: 1.75rem 1.25rem 3rem; }}
    .dag-card {{
      background: var(--surface); border: 1px solid var(--border); border-radius: 10px;
      padding: 1rem; margin-bottom: 1.75rem; overflow-x: auto;
    }}
    .dag-node {{ cursor: pointer; }}
    .dag-node:hover rect {{ stroke: #fbbf24; stroke-width: 2.5; }}
    .controls {{ display: flex; flex-wrap: wrap; gap: 0.5rem; align-items: center; margin-bottom: 1rem; }}
    .fbtn {{
      padding: 0.42rem 0.78rem; border-radius: 6px; border: 1px solid var(--border);
      background: var(--surface); color: var(--text); font-weight: 600; font-size: 0.86rem; cursor: pointer;
    }}
    .fbtn.active {{ background: #0284c7; border-color: #38bdf8; color: #ffffff; }}
    .search-box {{
      margin-left: auto; min-width: 240px; padding: 0.45rem 0.75rem;
      border-radius: 6px; border: 1px solid var(--border);
      background: var(--surface-2); color: var(--text); font-size: 0.9rem;
    }}
    .count-bar {{ margin: 0 0 0.75rem; color: var(--muted); font-size: 0.9rem; font-weight: 600; }}
    .table-wrap {{ overflow-x: auto; border: 1px solid var(--border); border-radius: 8px; }}
    table {{ width: 100%; border-collapse: collapse; background: var(--surface); }}
    th, td {{ text-align: left; padding: 0.55rem 0.7rem; border-bottom: 1px solid var(--border); font-size: 0.87rem; }}
    th {{ background: var(--surface-2); color: var(--text); font-weight: 700; position: sticky; top: 0; }}
    code {{ font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-size: 0.86em; }}
    .pill {{
      display: inline-block; padding: 0.16rem 0.5rem; border-radius: 999px;
      font-size: 0.77rem; font-weight: 700; white-space: nowrap;
      background: var(--surface-2); border: 1px solid var(--border);
    }}
    .pill-ledger11 {{ color: #38bdf8; border-color: #0369a1; }}
    .pill-ledger10 {{ color: #34d399; border-color: #047857; }}
    .pill-adversarial {{ color: #fb7185; border-color: #be123c; }}
    .pill-helpers {{ color: #c084fc; border-color: #7e22ce; }}
    .pill-receipt {{ color: #fbbf24; border-color: #b45309; }}
    .pill-bookkeeping {{ color: #cbd5e1; border-color: #475569; }}
    .ax-zero {{ color: #34d399; border-color: #065f46; }}
    .ax-pq {{ color: #38bdf8; border-color: #075985; }}
    .ax-choice {{ color: #fbbf24; border-color: #92400e; }}
    footer {{
      border-top: 1px solid var(--border); padding: 1.4rem 1.25rem 2.2rem;
      color: var(--muted); font-size: 0.88rem;
    }}
  </style>
</head>
<body>
  <a class="skip-link" href="#main-content">Skip to main content</a>
  <header>
    <div class="wrap">
      <nav aria-label="Primary">
        <a class="nav-btn" href="index.html">&larr; Project Overview</a>
        <a class="nav-btn" href="dist/Donahue_2026_noetic_organism_omega.pdf">White Paper PDF</a>
        <a class="nav-btn" href="results/lean4-noetic-audit.txt">Verbatim #print axioms</a>
        <a class="nav-btn" href="https://github.com/0thernes/noetic-organism-omega/tree/main/lean">Lean 4.35.0 Source</a>
      </nav>
      <h1>Interactive Lean 4.35.0 Proof Blueprint — 68 Theorems (<code>v2.2.0</code>)</h1>
      <p class="subtitle">
        Palomar 6-kernel isolated architecture (<code>lean4</code>, <code>leanchecker-paranoid</code>,
        <code>lean4lean</code>, <code>nanoda_bin</code>, <code>con-leche</code>, <code>con-ron</code>;
        zero Mathlib, zero <code>sorry</code>, zero <code>native_decide</code>).
        Breakdown: <strong>46</strong> parametric/adversarial proofs (including 11-slot <code>Ledger11</code> &amp;
        8 <code>AdversarialTests</code>) · <strong>18</strong> explicitly labeled bookkeeping numeral identities ·
        <strong>4</strong> live C11 receipt constant checks. Click any DAG node below to filter the table.
      </p>
    </div>
  </header>

  <main id="main-content" class="wrap">
    <section class="dag-card" aria-labelledby="dag-heading">
      <h2 id="dag-heading" style="margin:0 0 0.75rem; font-size:1.15rem;">Module &amp; Theorem Dependency DAG (Interactive)</h2>
      <svg viewBox="0 0 1080 310" width="100%" role="img" aria-label="Dependency graph from LiveC11Receipt and Spec through NoeticProofs, AdversarialTests, Challenge, Solution, and 6-Kernel Palomar Comparator">
        <defs>
          <marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 0 L 10 5 L 0 10 z" fill="#94a3b8"/>
          </marker>
        </defs>
        <line x1="210" y1="65" x2="275" y2="65" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="470" y1="65" x2="535" y2="65" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="372" y1="102" x2="145" y2="155" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="372" y1="102" x2="355" y2="155" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="372" y1="102" x2="565" y2="155" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="372" y1="102" x2="775" y2="155" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="372" y1="102" x2="965" y2="155" stroke="#94a3b8" stroke-width="2" marker-end="url(#arr)"/>
        <line x1="145" y1="218" x2="475" y2="250" stroke="#94a3b8" stroke-width="1.8" marker-end="url(#arr)"/>
        <line x1="355" y1="218" x2="515" y2="250" stroke="#94a3b8" stroke-width="1.8" marker-end="url(#arr)"/>
        <line x1="565" y1="218" x2="555" y2="250" stroke="#94a3b8" stroke-width="1.8" marker-end="url(#arr)"/>
        <line x1="775" y1="218" x2="600" y2="250" stroke="#94a3b8" stroke-width="1.8" marker-end="url(#arr)"/>
        <line x1="965" y1="218" x2="640" y2="250" stroke="#94a3b8" stroke-width="1.8" marker-end="url(#arr)"/>
        <line x1="740" y1="65" x2="800" y2="250" stroke="#38bdf8" stroke-width="2" stroke-dasharray="5,4" marker-end="url(#arr)"/>

        <g class="dag-node" data-filter-cat="receipt" tabindex="0" role="button" aria-label="Filter Live C11 Receipt checks">
          <rect x="20" y="28" width="190" height="74" rx="8" fill="#0b1222" stroke="#fbbf24" stroke-width="2"/>
          <text x="115" y="54" text-anchor="middle" fill="#fbbf24" font-size="13" font-weight="700">LiveC11Receipt.lean</text>
          <text x="115" y="72" text-anchor="middle" fill="#cbd5e1" font-size="11">Win64 == Linux 0-B diff</text>
          <text x="115" y="88" text-anchor="middle" fill="#94a3b8" font-size="10">4 receipt checks (N24,32,48,59)</text>
        </g>

        <g class="dag-node" data-filter-cat="all" tabindex="0" role="button" aria-label="Show all 68 theorems">
          <rect x="275" y="28" width="195" height="74" rx="8" fill="#0b1222" stroke="#38bdf8" stroke-width="2"/>
          <text x="372" y="54" text-anchor="middle" fill="#38bdf8" font-size="13" font-weight="700">NoeticProofs/Spec.lean</text>
          <text x="372" y="72" text-anchor="middle" fill="#cbd5e1" font-size="11">Pure defs (0 proofs, 0 sorry)</text>
          <text x="372" y="88" text-anchor="middle" fill="#94a3b8" font-size="10">Ledger10, Ledger11, Guards</text>
        </g>

        <g>
          <rect x="535" y="28" width="215" height="74" rx="8" fill="#0b1222" stroke="#c084fc" stroke-width="2"/>
          <text x="642" y="54" text-anchor="middle" fill="#c084fc" font-size="13" font-weight="700">Challenge.lean (Isolated)</text>
          <text x="642" y="72" text-anchor="middle" fill="#cbd5e1" font-size="11">Imports ONLY Spec.lean</text>
          <text x="642" y="88" text-anchor="middle" fill="#94a3b8" font-size="10">68 statement-only signatures</text>
        </g>

        <g class="dag-node" data-filter-cat="ledger10" tabindex="0" role="button" aria-label="Filter Ledger10 theorems">
          <rect x="45" y="155" width="200" height="63" rx="8" fill="#0b1222" stroke="#34d399" stroke-width="2"/>
          <text x="145" y="180" text-anchor="middle" fill="#34d399" font-size="12.5" font-weight="700">Ledger10 (14 thms)</text>
          <text x="145" y="198" text-anchor="middle" fill="#cbd5e1" font-size="11">N01..N10, N36..37, N49, N54</text>
        </g>

        <g class="dag-node" data-filter-cat="ledger11" tabindex="0" role="button" aria-label="Filter 11-slot Ledger11 theorems">
          <rect x="260" y="155" width="190" height="63" rx="8" fill="#0b1222" stroke="#38bdf8" stroke-width="2"/>
          <text x="355" y="180" text-anchor="middle" fill="#38bdf8" font-size="12.5" font-weight="700">Ledger11 (4 + N59)</text>
          <text x="355" y="198" text-anchor="middle" fill="#cbd5e1" font-size="11">Full 11-slot N55..N59</text>
        </g>

        <g class="dag-node" data-filter-cat="helpers" tabindex="0" role="button" aria-label="Filter encodings, region guard, and statistics theorems">
          <rect x="465" y="155" width="200" height="63" rx="8" fill="#0b1222" stroke="#c084fc" stroke-width="2"/>
          <text x="565" y="180" text-anchor="middle" fill="#c084fc" font-size="12.5" font-weight="700">Guards/Encodings (20)</text>
          <text x="565" y="198" text-anchor="middle" fill="#cbd5e1" font-size="11">N11, N14..22, N25..30, N60</text>
        </g>

        <g class="dag-node" data-filter-cat="adversarial" tabindex="0" role="button" aria-label="Filter 8 adversarial false-variant refutations">
          <rect x="680" y="155" width="190" height="63" rx="8" fill="#0b1222" stroke="#fb7185" stroke-width="2"/>
          <text x="775" y="180" text-anchor="middle" fill="#fb7185" font-size="12.5" font-weight="700">AdversarialTests (8)</text>
          <text x="775" y="198" text-anchor="middle" fill="#cbd5e1" font-size="11">A01..A08 (0 axioms)</text>
        </g>

        <g class="dag-node" data-filter-cat="bookkeeping" tabindex="0" role="button" aria-label="Filter 18 bookkeeping numeral identities">
          <rect x="885" y="155" width="175" height="63" rx="8" fill="#0b1222" stroke="#64748b" stroke-width="2"/>
          <text x="972" y="180" text-anchor="middle" fill="#cbd5e1" font-size="12.5" font-weight="700">Bookkeeping (18)</text>
          <text x="972" y="198" text-anchor="middle" fill="#94a3b8" font-size="11">Closed numeral literals</text>
        </g>

        <g>
          <rect x="330" y="250" width="530" height="46" rx="8" fill="#052e16" stroke="#34d399" stroke-width="2"/>
          <text x="595" y="278" text-anchor="middle" fill="#ecfdf5" font-size="13" font-weight="700">
            Solution.lean vs Challenge.lean &rarr; 6-Kernel Palomar Comparator + Negative Tamper Gate (68/68 PASS)
          </text>
        </g>
      </svg>
    </section>

    <section aria-labelledby="theorem-table-heading">
      <h2 id="theorem-table-heading" style="margin:0 0 0.75rem; font-size:1.25rem;">Complete 68-Theorem Ledger &amp; Verbatim Axiom Audit</h2>
      <div class="controls" role="toolbar" aria-label="Filter theorems by epistemic category">
        <button type="button" class="fbtn active" data-filter-cat="all" aria-pressed="true">All (68)</button>
        <button type="button" class="fbtn" data-filter-cat="ledger11" aria-pressed="false">Ledger11 (4)</button>
        <button type="button" class="fbtn" data-filter-cat="ledger10" aria-pressed="false">Ledger10 (14)</button>
        <button type="button" class="fbtn" data-filter-cat="helpers" aria-pressed="false">Guards &amp; Encodings (20)</button>
        <button type="button" class="fbtn" data-filter-cat="adversarial" aria-pressed="false">AdversarialTests (8)</button>
        <button type="button" class="fbtn" data-filter-cat="receipt" aria-pressed="false">Receipt Checks (4)</button>
        <button type="button" class="fbtn" data-filter-cat="bookkeeping" aria-pressed="false">Bookkeeping Numerals (18)</button>
        <label for="bp-search" style="position:absolute;left:-9999px;">Search theorems</label>
        <input id="bp-search" class="search-box" type="search" placeholder="Filter by name, axiom, or module..." aria-label="Search theorems">
      </div>
      <p id="bp-count" class="count-bar" role="status" aria-live="polite">Showing 68 of 68 theorems</p>
      <div class="table-wrap">
        <table>
          <thead>
            <tr>
              <th scope="col">#</th>
              <th scope="col">Theorem Declaration</th>
              <th scope="col">Epistemic Category</th>
              <th scope="col">Verbatim #print axioms</th>
              <th scope="col">Module</th>
              <th scope="col">Specification Anchor</th>
            </tr>
          </thead>
          <tbody id="bp-tbody">
            {"".join(rows_html)}
          </tbody>
        </table>
      </div>
    </section>
  </main>

  <footer>
    <div class="wrap">
      <p>
        Generated directly from <code>lean/comparator.json</code> &amp; <code>results/lean4-noetic-audit.txt</code> ·
        Strict CSP (zero CDN, zero inline script) · WCAG 2.2 AA ·
        Author: <strong>Alexander Stephen Donahue</strong> (ORCID <a href="https://orcid.org/0009-0006-8168-9648">0009-0006-8168-9648</a>) ·
        Apache-2.0 / CC-BY-4.0.
      </p>
    </div>
  </footer>
  <script src="blueprint.js"></script>
</body>
</html>
"""
    (ROOT / "docs" / "blueprint.html").write_text(bp_html, encoding="utf-8", newline="\n")
    print(f"Wrote docs/blueprint.html ({len(comp['theorem_names'])} theorems)")

    # 3. Generate llms-full.txt (LF normalized)
    sections = [
        ("README.md", ROOT / "README.md"),
        ("CLAIMS.md", ROOT / "CLAIMS.md"),
        ("LIMITATIONS.md", ROOT / "LIMITATIONS.md"),
        ("ERRATA.md", ROOT / "ERRATA.md"),
        ("docs/statement-fidelity.md", ROOT / "docs" / "statement-fidelity.md"),
        ("docs/MEGA_ATLAS_CROSSCHECK.md", ROOT / "docs" / "MEGA_ATLAS_CROSSCHECK.md"),
        ("docs/AI_DISCLOSURE.md", ROOT / "docs" / "AI_DISCLOSURE.md"),
        ("SUBMISSION_READINESS.md", ROOT / "SUBMISSION_READINESS.md"),
    ]
    parts = [
        "# Noetic Organism Omega (v2.2.0) — Full Technical Digest (llms-full.txt)",
        "Author: Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA; ORCID 0009-0006-8168-9648)",
        "Repository: https://github.com/0thernes/noetic-organism-omega",
        "Interactive Blueprint: https://0thernes.github.io/noetic-organism-omega/blueprint.html",
        "SWHID: swh:1:snp:b6ab6984080956100c967e802c6fb44e0da7ff52",
        "",
    ]
    for title, path in sections:
        parts.append(f"\n{'=' * 78}\n## FILE: {title}\n{'=' * 78}\n")
        parts.append(path.read_text(encoding="utf-8").replace("\r\n", "\n").strip())
    (ROOT / "llms-full.txt").write_text("\n".join(parts) + "\n", encoding="utf-8", newline="\n")
    print("Wrote llms-full.txt")

    # 4. Generate croissant.json (MLCommons Croissant 1.0)
    def file_obj(fid: str, rel: str, desc: str, mime: str) -> dict:
        p = ROOT / rel
        return {
            "@type": "cr:FileObject",
            "@id": fid,
            "name": p.name,
            "description": desc,
            "contentUrl": f"https://raw.githubusercontent.com/0thernes/noetic-organism-omega/main/{rel}",
            "encodingFormat": mime,
            "sha256": sha256_file(p),
            "contentSize": f"{p.stat().st_size} B",
        }

    croissant = {
        "@context": {
            "@language": "en",
            "@vocab": "https://schema.org/",
            "citeAs": "cr:citeAs",
            "column": "cr:column",
            "conformsTo": "dct:conformsTo",
            "cr": "http://mlcommons.org/croissant/",
            "dataType": {"@id": "cr:dataType", "@type": "@vocab"},
            "dct": "http://purl.org/dc/terms/",
            "extract": "cr:extract",
            "field": "cr:field",
            "fileObject": "cr:fileObject",
            "recordSet": "cr:recordSet",
            "sc": "https://schema.org/",
            "source": "cr:source",
        },
        "@type": "sc:Dataset",
        "conformsTo": "http://mlcommons.org/croissant/1.0",
        "name": "Noetic Organism Omega v2.2.0 Verification Receipts & 825-Check Trace Dataset",
        "alternateName": "noetic-organism-omega v2.2.0",
        "description": "Deterministic empirical verification receipts from Noetic Organism Omega v2.2.0 (825 Clean-Slate v3.0 checks across 9 suites, 18 structural ablations, 49 cognitive/CQM 5-arm evaluations, 6-tier scaling benchmarks, 22-entry live citation audit, and 68-theorem Lean 4.35.0 axiom transcripts). 29 consciousness-related proxies are strictly [INDICATOR-ONLY]; 0/1,900 Mega-Atlas problems retired.",
        "version": "2.2.0",
        "datePublished": "2026-11-26",
        "url": "https://0thernes.github.io/noetic-organism-omega/",
        "sameAs": "https://github.com/0thernes/noetic-organism-omega",
        "license": "https://creativecommons.org/licenses/by/4.0/",
        "citeAs": "Donahue, Alexander Stephen (2026). Noetic Organism Omega (v2.2.0). Independent Researcher, Spokane, WA, USA. ORCID: 0009-0006-8168-9648. https://github.com/0thernes/noetic-organism-omega",
        "creator": {
            "@type": "sc:Person",
            "name": "Alexander Stephen Donahue",
            "identifier": "https://orcid.org/0009-0006-8168-9648",
            "affiliation": "Independent Researcher, Spokane, WA, USA",
            "url": "https://0thernes.art",
        },
        "keywords": [
            "artificial-life", "c11", "lean4", "formal-verification", "thermodynamic-conservation",
            "ablation-study", "xpbd", "bitnet-b158", "philox", "cross-os-determinism",
        ],
        "distribution": [
            file_obj("trace-825-csv", "results/m8_e2_825_honest_trace_matrix.csv", "Full 825-check Clean-Slate v3.0 native C11 trace matrix (UTF-8 CSV).", "text/csv"),
            file_obj("ablation-e1-csv", "results/m8_e1_ablation_sweep.csv", "18-circuit structural ablation sweep over N=48 locked seeds.", "text/csv"),
            file_obj("cognitive-e2-csv", "results/m8_e2_cognitive_49.csv", "49-item 5-arm cognitive/CQM battery results.", "text/csv"),
            file_obj("scaling-e3-csv", "results/m8_e3_scaling_bench.csv", "6-tier single-thread scaling benchmark (N=1..4096).", "text/csv"),
            file_obj("citation-audit-json", "results/citation-audit.json", "Live Crossref/arXiv title+author alignment audit across all 22 paper references.", "application/json"),
            file_obj("lean-axioms-txt", "results/lean4-noetic-audit.txt", "Verbatim 68-theorem Lean 4.35.0 #print axioms audit transcript.", "text/plain"),
        ],
        "recordSet": [
            {
                "@type": "cr:RecordSet",
                "@id": "checks_825",
                "name": "checks_825",
                "description": "All 825 Clean-Slate v3.0 native C11 verification checks across 9 suites.",
                "field": [
                    {
                        "@type": "cr:Field",
                        "@id": f"checks_825/{col}",
                        "name": col,
                        "dataType": dtype,
                        "source": {"fileObject": {"@id": "trace-825-csv"}, "extract": {"column": col}},
                    }
                    for col, dtype in [
                        ("id", "sc:Text"), ("suite_id", "sc:Text"), ("title", "sc:Text"),
                        ("mirror", "sc:Text"), ("status", "sc:Text"), ("repo", "sc:Text"),
                        ("n_seeds", "sc:Integer"), ("intact", "sc:Float"), ("lesion", "sc:Float"),
                        ("d_z", "sc:Float"), ("holm_p", "sc:Float"), ("evidence", "sc:Text"),
                    ]
                ],
            }
        ],
    }
    (ROOT / "croissant.json").write_text(json.dumps(croissant, indent=2) + "\n", encoding="utf-8", newline="\n")
    print("Wrote croissant.json")

    # 5. Generate ro-crate-metadata.json (RO-Crate 1.1)
    ro_parts = [
        ("dist/Donahue_2026_noetic_organism_omega.pdf", "11-page AMS-LaTeX white paper (Tectonic, 0 overfull hboxes)", "application/pdf"),
        ("dist/arxiv_submission.tar.gz", "Deterministic clean-room arXiv source tarball", "application/gzip"),
        ("docs/index.html", "WCAG 2.2 AA strict-CSP GitHub Pages landing page", "text/html"),
        ("docs/blueprint.html", "Interactive 68-theorem Lean 4.35.0 Proof Blueprint SVG DAG", "text/html"),
        ("docs/DATASHEET.md", "Gebru et al. Datasheet for Datasets & Verification Receipts", "text/markdown"),
        ("docs/statement-fidelity.md", "Statement-fidelity audit matrix (68 Lean theorems vs C11)", "text/markdown"),
        ("docs/MEGA_ATLAS_CROSSCHECK.md", "Non-retirement cross-check against 1,900 Mega-Atlas problems (0/1,900)", "text/markdown"),
        ("CLAIMS.md", "Falsifiable Claims Ledger (C1..C9)", "text/markdown"),
        ("ERRATA.md", "Chronological errata including 2026-11-26 audit and v2.2.0 resolutions", "text/markdown"),
        ("SUBMISSION_READINESS.md", "Venue readiness & verification checklist", "text/markdown"),
        ("lean/NoeticProofs/Spec.lean", "Pure shared Lean 4.35.0 definitions (Ledger10, Ledger11, Guards)", "text/plain"),
        ("lean/NoeticProofs/LiveC11Receipt.lean", "Cross-OS 0-bit identical C11 execution receipt (seed 5001)", "text/plain"),
        ("lean/NoeticProofs.lean", "60 Lean 4.35.0 theorems (N01..N60)", "text/plain"),
        ("lean/AdversarialTests.lean", "8 zero-axiom adversarial false-variant refutations (A01..A08)", "text/plain"),
        ("results/lean4-noetic-audit.txt", "Verbatim 68-theorem #print axioms audit", "text/plain"),
        ("results/lean4-multi-kernel.txt", "Independent multi-kernel Lean 4.35.0 replay transcript", "text/plain"),
        ("results/citation-audit.json", "22/22 live Crossref/arXiv citation reachability & title/author alignment audit", "application/json"),
        ("results/m8_e2_825_honest_trace_matrix.csv", "825-check Clean-Slate v3.0 native C11 trace matrix", "text/csv"),
        ("results/m8_e2_825_honest_trace_summary.json", "825-check native C11 summary", "application/json"),
        ("results/m9_native_release_gates.json", "Native release gates receipt (7/7 PASS, GATE-11 10k 0-bit)", "application/json"),
        ("results/seed_firewall_ledger.json", "SHA-256 hash-chained Burn-on-Fail Seed Firewall ledger", "application/json"),
        ("croissant.json", "MLCommons Croissant 1.0 dataset metadata", "application/ld+json"),
        ("codemeta.json", "CodeMeta 2.0 scientific software metadata", "application/ld+json"),
        ("REUSE.toml", "SPDX REUSE dual-license manifest", "text/plain"),
    ]
    ro_graph: list[dict] = [
        {"@type": "CreativeWork", "@id": "ro-crate-metadata.json", "conformsTo": {"@id": "https://w3id.org/ro/crate/1.1"}, "about": {"@id": "./"}},
        {
            "@id": "./",
            "@type": "Dataset",
            "name": "Noetic Organism Omega (v2.2.0)",
            "version": "2.2.0",
            "datePublished": "2026-11-26",
            "description": "Cross-OS bit-identical portable C11 whole-organism cognitive engine (825/825 checks, 100/100 milestones, 13/13 gates, 5/5 caught mutants) paired with 68 pure Lean 4.35.0 Palomar theorems across 6 independent proof kernels, 11-page AMS-LaTeX white paper, interactive proof blueprint, and dual cryptographic timestamps.",
            "url": "https://0thernes.github.io/noetic-organism-omega/",
            "identifier": "https://github.com/0thernes/noetic-organism-omega",
            "sameAs": "https://archive.softwareheritage.org/swh:1:snp:b6ab6984080956100c967e802c6fb44e0da7ff52",
            "license": {"@id": "https://creativecommons.org/licenses/by/4.0/"},
            "author": {"@id": "https://orcid.org/0009-0006-8168-9648"},
            "hasPart": [{"@id": rel} for rel, _, _ in ro_parts] + [
                {"@id": "CHECKSUMS.sha256"}, {"@id": "CHECKSUMS.sha256.tsr"}, {"@id": "CHECKSUMS.sha256.ots"}
            ],
        },
        {
            "@id": "https://orcid.org/0009-0006-8168-9648",
            "@type": "Person",
            "name": "Alexander Stephen Donahue",
            "email": "0_0@0thernes.art",
            "affiliation": "Independent Researcher, Spokane, WA, USA",
            "url": "https://0thernes.art",
        },
        {
            "@id": "https://creativecommons.org/licenses/by/4.0/",
            "@type": "CreativeWork",
            "name": "Creative Commons Attribution 4.0 International (CC BY 4.0)",
            "url": "https://creativecommons.org/licenses/by/4.0/",
        },
    ]
    for rel, desc, mime in ro_parts:
        p = ROOT / rel
        ro_graph.append({
            "@id": rel,
            "@type": "File",
            "name": p.name,
            "description": desc,
            "encodingFormat": mime,
            "sha256": sha256_file(p),
            "contentSize": p.stat().st_size,
        })
    ro_graph.extend([
        {"@id": "CHECKSUMS.sha256", "@type": "File", "name": "CHECKSUMS.sha256", "description": "LF-normalized SHA-256 manifest", "encodingFormat": "text/plain"},
        {"@id": "CHECKSUMS.sha256.tsr", "@type": "File", "name": "CHECKSUMS.sha256.tsr", "description": "DigiCert RFC 3161 Trusted Timestamp token", "encodingFormat": "application/timestamp-reply"},
        {"@id": "CHECKSUMS.sha256.ots", "@type": "File", "name": "CHECKSUMS.sha256.ots", "description": "3-calendar Bitcoin OpenTimestamps proof", "encodingFormat": "application/octet-stream"},
    ])
    (ROOT / "ro-crate-metadata.json").write_text(
        json.dumps({"@context": "https://w3id.org/ro/crate/1.1/context", "@graph": ro_graph}, indent=2) + "\n",
        encoding="utf-8",
        newline="\n",
    )
    print("Wrote ro-crate-metadata.json")

    # 6. Regenerate LF-normalized data/MANIFEST.sha256 and CHECKSUMS.sha256
    res_files = sorted(p for p in (ROOT / "results").iterdir() if p.is_file())
    manifest_lines = [f"{sha256_file(p)}  results/{p.name}" for p in res_files]
    (ROOT / "data" / "MANIFEST.sha256").write_text("\n".join(manifest_lines) + "\n", encoding="utf-8", newline="\n")
    print(f"Wrote data/MANIFEST.sha256 ({len(manifest_lines)} entries)")

    checksum_targets = [
        "VERSION",
        "LICENSE",
        "LICENSE-DOCS",
        "REUSE.toml",
        "CITATION.cff",
        "codemeta.json",
        "croissant.json",
        "ro-crate-metadata.json",
        "dist/Donahue_2026_noetic_organism_omega.pdf",
        "dist/arxiv_submission.tar.gz",
        "paper/main.tex",
        "paper/refs.bib",
        "docs/index.html",
        "docs/blueprint.html",
        "docs/blueprint.js",
        "docs/DATASHEET.md",
        "lean/lean-toolchain",
        "lean/lakefile.toml",
        "lean/lake-manifest.json",
        "lean/NoeticProofs/Spec.lean",
        "lean/NoeticProofs/LiveC11Receipt.lean",
        "lean/NoeticProofs.lean",
        "lean/AdversarialTests.lean",
        "lean/Challenge.lean",
        "lean/Solution.lean",
        "lean/VerifyAxioms.lean",
        "lean/comparator.json",
        "lean/formalization.yaml",
        "results/lean4-noetic-audit.txt",
        "results/lean4-multi-kernel.txt",
        "results/citation-audit.json",
        "results/m8_e2_825_honest_trace_summary.json",
        "results/m9_native_release_gates.json",
        "results/seed_firewall_ledger.json",
        "results/m8_e3_scaling_bench.json",
        "data/MANIFEST.sha256",
    ]
    cs_lines = [f"{sha256_file(ROOT / rel)}  {rel}" for rel in checksum_targets]
    (ROOT / "CHECKSUMS.sha256").write_text("\n".join(cs_lines) + "\n", encoding="utf-8", newline="\n")
    print(f"Wrote CHECKSUMS.sha256 ({len(cs_lines)} entries)")


if __name__ == "__main__":
    main()
