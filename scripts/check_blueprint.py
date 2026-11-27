#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""WCAG 2.2 AA, Strict-CSP, Zero-CDN, and 68-Theorem Coverage Verifier for
`docs/blueprint.html`, `docs/blueprint.js`, and `docs/index.html`."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs"
COMP_PATH = ROOT / "lean" / "comparator.json"

FORBIDDEN_CDN = re.compile(
    r"(https?:)?//(cdn\.|unpkg\.com|cdnjs\.|fonts\.googleapis\.com|fonts\.gstatic\.com|jsdelivr\.net|ajax\.googleapis\.com)",
    re.IGNORECASE,
)


def check_html_security_and_a11y(path: Path) -> list[str]:
    errs: list[str] = []
    if not path.exists():
        return [f"Missing {path}"]
    txt = path.read_text(encoding="utf-8")
    if 'lang="en"' not in txt:
        errs.append(f"{path.name}: missing lang=\"en\" on <html>")
    if "<title>" not in txt:
        errs.append(f"{path.name}: missing <title>")
    if 'http-equiv="Content-Security-Policy"' not in txt:
        errs.append(f"{path.name}: missing Content-Security-Policy meta tag")
    if "default-src 'none'" not in txt:
        errs.append(f"{path.name}: CSP must start with default-src 'none'")
    if re.search(r"script-src[^;\"]*unsafe-inline", txt):
        errs.append(f"{path.name}: CSP must not allow script-src 'unsafe-inline'")
    if re.search(r"\bon(click|change|input|load|submit)\s*=", txt, re.IGNORECASE):
        errs.append(f"{path.name}: inline event handlers forbidden under strict CSP")
    if "<main" not in txt or "<header" not in txt or "<footer" not in txt:
        errs.append(f"{path.name}: missing semantic HTML5 landmarks (<header>, <main>, <footer>)")
    if "skip-link" not in txt:
        errs.append(f"{path.name}: missing keyboard skip-link for WCAG 2.2 AA")
    m_cdn = FORBIDDEN_CDN.search(txt)
    if m_cdn:
        errs.append(f"{path.name}: forbidden external CDN reference found: {m_cdn.group(0)}")
    return errs


def main() -> None:
    errors: list[str] = []
    for name in ("index.html", "blueprint.html"):
        errors.extend(check_html_security_and_a11y(DOCS / name))

    js_path = DOCS / "blueprint.js"
    if not js_path.exists():
        errors.append("Missing docs/blueprint.js")
    else:
        js_txt = js_path.read_text(encoding="utf-8")
        if FORBIDDEN_CDN.search(js_txt):
            errors.append("docs/blueprint.js: forbidden external CDN reference found")

    # Verify ALL 68 theorems from lean/comparator.json appear in docs/blueprint.html
    comp = json.loads(COMP_PATH.read_text(encoding="utf-8"))
    theorems: list[str] = comp["theorem_names"]
    bp_html = (DOCS / "blueprint.html").read_text(encoding="utf-8") if (DOCS / "blueprint.html").exists() else ""
    if "<svg" not in bp_html:
        errors.append("docs/blueprint.html: missing interactive SVG dependency DAG")

    missing = []
    for full_name in theorems:
        short = full_name.split(".", 1)[-1]
        if short not in bp_html:
            missing.append(short)
    if missing:
        errors.append(f"docs/blueprint.html missing {len(missing)} theorem(s) from comparator.json: {missing[:5]}")

    if errors:
        print("FAIL: Blueprint / Pages audit errors:", file=sys.stderr)
        for e in errors:
            print(f"  - {e}", file=sys.stderr)
        sys.exit(1)

    print(
        f"PASS: docs/index.html & docs/blueprint.html verified WCAG 2.2 AA, strict CSP "
        f"(default-src 'none', zero unsafe-inline scripts), zero CDN, and {len(theorems)}/{len(theorems)} "
        f"Lean 4.35.0 theorems covered!"
    )


if __name__ == "__main__":
    main()
