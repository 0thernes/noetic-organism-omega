#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Live Citation & URL Reachability Verifier (Playbook v1.3 Pass 5):
1. Cross-checks every `\\cite{...}` in `paper/main.tex` against `paper/refs.bib`
   (zero undefined citations, zero uncited orphans, zero duplicate keys).
2. Verifies every DOI via the DOI Handle REST API (`https://doi.org/api/handles/<doi>`)
   and every URL via live HTTP HEAD/GET reachability.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BIB_PATH = ROOT / "paper" / "refs.bib"
TEX_PATH = ROOT / "paper" / "main.tex"


def check_doi(doi: str, timeout: int = 15) -> tuple[bool, str]:
    api_url = f"https://doi.org/api/handles/{urllib.parse.quote(doi.strip(), safe='/')}"
    req = urllib.request.Request(api_url, headers={"User-Agent": "NoeticOrganismOmega-CitationVerifier/2.1"}, method="GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            if resp.getcode() == 200:
                payload = json.loads(resp.read().decode("utf-8"))
                if payload.get("responseCode") == 1:
                    return True, "DOI OK (200)"
                return False, f"DOI code {payload.get('responseCode')}"
            return False, f"HTTP {resp.getcode()}"
    except Exception as exc:
        return False, str(exc)


def check_url(url: str, timeout: int = 15) -> tuple[bool, str]:
    headers = {"User-Agent": "Mozilla/5.0 (compatible; NoeticOrganismOmega-CitationVerifier/2.1; +https://github.com/0thernes/noetic-organism-omega)"}
    for method in ("HEAD", "GET"):
        req = urllib.request.Request(url, headers=headers, method=method)
        try:
            with urllib.request.urlopen(req, timeout=timeout) as resp:
                code = resp.getcode()
                if 200 <= code < 400:
                    return True, f"HTTP {code}"
        except urllib.error.HTTPError as e:
            if method == "HEAD" and e.code in (403, 405, 429):
                continue
            return False, f"HTTP {e.code}"
        except Exception as exc:
            if method == "HEAD":
                continue
            return False, str(exc)
    return False, "unreachable"


def main() -> None:
    parser = argparse.ArgumentParser(description="Verify citations in paper/refs.bib and paper/main.tex.")
    parser.add_argument("--offline", action="store_true", help="Only validate BibTeX syntax & TeX cross-references without network calls")
    args = parser.parse_args()

    if not BIB_PATH.exists():
        print(f"FAIL: {BIB_PATH} not found", file=sys.stderr)
        sys.exit(1)

    raw_bib = BIB_PATH.read_text(encoding="utf-8")
    entries = [e.strip() for e in re.findall(r"@\w+\{([^,]+),", raw_bib)]
    dupes = sorted({k for k in entries if entries.count(k) > 1})
    if dupes:
        print(f"FAIL: duplicate BibTeX keys: {dupes}", file=sys.stderr)
        sys.exit(1)

    urls = list(dict.fromkeys(u.strip() for u in re.findall(r"url\s*=\s*\{([^}]+)\}", raw_bib)))
    dois = list(dict.fromkeys(d.strip() for d in re.findall(r"doi\s*=\s*\{([^}]+)\}", raw_bib)))

    if TEX_PATH.exists():
        raw_tex = TEX_PATH.read_text(encoding="utf-8")
        tex_nocomment = "\n".join(re.sub(r"(^|[^\\])%.*", r"\1", ln) for ln in raw_tex.splitlines())
        cited: set[str] = set()
        for m in re.finditer(r"\\cite(?:\[[^\]]*\])?\{([^}]+)\}", tex_nocomment):
            for k in m.group(1).split(","):
                k = k.strip()
                if k:
                    cited.add(k)
        bib_set = set(entries)
        missing = sorted(cited - bib_set)
        uncited = sorted(bib_set - cited)
        if missing:
            print(f"FAIL: undefined \\cite keys in main.tex: {missing}", file=sys.stderr)
            sys.exit(1)
        if uncited:
            print(f"FAIL: uncited orphan BibTeX entries in refs.bib: {uncited}", file=sys.stderr)
            sys.exit(1)
        print(f"Cross-check OK: {len(cited)}/{len(entries)} BibTeX keys cited in paper/main.tex (0 missing, 0 orphans).")

    print(f"Parsed {len(entries)} BibTeX entries ({len(urls)} URLs + {len(dois)} DOIs) from {BIB_PATH.name}.")
    if len(entries) < 14:
        print(f"FAIL: expected at least 14 verified citations in paper/refs.bib, got {len(entries)}", file=sys.stderr)
        sys.exit(1)

    if args.offline:
        print("PASS (offline syntax & cross-reference check).")
        return

    failures = []
    for u in urls:
        ok, status = check_url(u)
        print(f"  [{'OK  ' if ok else 'FAIL'}] {status:<14} {u}")
        if not ok:
            failures.append((u, status))
    for d in dois:
        ok, status = check_doi(d)
        print(f"  [{'OK  ' if ok else 'FAIL'}] {status:<14} doi:{d}")
        if not ok:
            failures.append((f"doi:{d}", status))

    total = len(urls) + len(dois)
    if failures:
        print(f"\nFAIL: {len(failures)}/{total} citation target(s) failed live verification!", file=sys.stderr)
        sys.exit(1)
    print(f"\nPASS: All {total}/{total} citation URLs & DOIs across {len(entries)} entries verified live!")


if __name__ == "__main__":
    main()
