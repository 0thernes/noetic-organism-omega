#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Live Citation Reachability & Crossref/arXiv Title+Author Alignment Verifier (Playbook v1.3):
1. Cross-checks every `\\cite{...}` in `paper/main.tex` against `paper/refs.bib`
   (zero undefined citations, zero uncited orphans, zero duplicate keys).
2. Verifies every DOI via DOI Handle REST API + Crossref `/works/<doi>` title & author alignment.
3. Verifies every arXiv eprint via live HTTP + arXiv Atom API title & author alignment.
4. Verifies URL-only entries via live HTTP + Crossref bibliographic search alignment.
5. Writes structured audit report to `results/citation-audit.json`.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import unicodedata
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BIB_PATH = ROOT / "paper" / "refs.bib"
TEX_PATH = ROOT / "paper" / "main.tex"
AUDIT_OUT = ROOT / "results" / "citation-audit.json"
USER_AGENT = "NoeticOrganismOmega-CitationVerifier/2.2 (https://github.com/0thernes/noetic-organism-omega; mailto:0_0@0thernes.art)"

STOPWORDS = {
    "a", "an", "and", "as", "at", "by", "for", "from", "in", "into",
    "is", "it", "of", "on", "or", "the", "to", "towards", "toward", "with",
}


def strip_latex(text: str) -> str:
    s = re.sub(r"\\['`^\"~=.]\{?([A-Za-z])\}?", r"\1", text)
    s = s.replace("{", "").replace("}", "").replace("--", " ").replace("~", " ")
    s = unicodedata.normalize("NFKD", s).encode("ascii", "ignore").decode("ascii")
    return re.sub(r"\s+", " ", s).strip()


def norm_tokens(text: str) -> set[str]:
    clean = strip_latex(text).lower()
    toks = re.findall(r"[a-z0-9]+", clean)
    return {t for t in toks if len(t) >= 2 and t not in STOPWORDS}


def title_overlap(bib_title: str, remote_title: str) -> float:
    a = norm_tokens(bib_title)
    b = norm_tokens(remote_title)
    if not a or not b:
        return 0.0
    inter = len(a & b)
    return inter / min(len(a), len(b))


def extract_first_surname(author_field: str) -> str:
    first = author_field.split(" and ")[0].strip()
    first = strip_latex(first)
    if "," in first:
        surname = first.split(",", 1)[0].strip()
    else:
        surname = first.split()[-1].strip() if first.split() else ""
    # Take last word for multi-part surnames like "de Moura" -> "moura"
    parts = re.findall(r"[a-z]+", surname.lower())
    return parts[-1] if parts else ""


def parse_bibtex(raw: str) -> list[dict[str, str]]:
    entries: list[dict[str, str]] = []
    # Split on top-level @type{key,
    starts = [m.start() for m in re.finditer(r"(?m)^@\w+\{", raw)]
    for idx, pos in enumerate(starts):
        end = starts[idx + 1] if idx + 1 < len(starts) else len(raw)
        chunk = raw[pos:end].strip()
        m_head = re.match(r"@(\w+)\{([^,]+),", chunk)
        if not m_head:
            continue
        rec: dict[str, str] = {"type": m_head.group(1).lower(), "key": m_head.group(2).strip()}
        body = chunk[m_head.end():]
        # Parse balanced brace fields
        i = 0
        n = len(body)
        while i < n:
            m_fld = re.search(r"\b([A-Za-z][A-Za-z0-9_-]*)\s*=\s*\{", body[i:])
            if not m_fld:
                break
            fname = m_fld.group(1).lower()
            val_start = i + m_fld.end()
            depth = 1
            j = val_start
            while j < n and depth > 0:
                if body[j] == "{":
                    depth += 1
                elif body[j] == "}":
                    depth -= 1
                j += 1
            rec[fname] = body[val_start : j - 1].strip()
            i = j
        entries.append(rec)
    return entries


def check_url(url: str, timeout: int = 15) -> tuple[bool, str]:
    headers = {"User-Agent": "Mozilla/5.0 (compatible; " + USER_AGENT + ")"}
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


def check_doi_handle(doi: str, timeout: int = 15) -> tuple[bool, str]:
    api_url = f"https://doi.org/api/handles/{urllib.parse.quote(doi.strip(), safe='/')}"
    req = urllib.request.Request(api_url, headers={"User-Agent": USER_AGENT}, method="GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            if resp.getcode() == 200:
                payload = json.loads(resp.read().decode("utf-8"))
                if payload.get("responseCode") == 1:
                    return True, "DOI Handle OK (200)"
                return False, f"DOI handle responseCode={payload.get('responseCode')}"
            return False, f"HTTP {resp.getcode()}"
    except Exception as exc:
        return False, str(exc)


def fetch_crossref_doi(doi: str, timeout: int = 20) -> tuple[bool, dict]:
    api_url = f"https://api.crossref.org/works/{urllib.parse.quote(doi.strip(), safe='/')}"
    req = urllib.request.Request(api_url, headers={"User-Agent": USER_AGENT}, method="GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            return True, data.get("message", {})
    except Exception as exc:
        return False, {"error": str(exc)}


def fetch_arxiv_meta(eprint: str, timeout: int = 20) -> tuple[bool, dict]:
    api_url = f"https://export.arxiv.org/api/query?id_list={urllib.parse.quote(eprint.strip())}"
    req = urllib.request.Request(api_url, headers={"User-Agent": USER_AGENT}, method="GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            xml_bytes = resp.read()
        root = ET.fromstring(xml_bytes)
        ns = {"atom": "http://www.w3.org/2005/Atom"}
        entry = root.find("atom:entry", ns)
        if entry is None:
            return False, {"error": "no entry"}
        title_el = entry.find("atom:title", ns)
        authors = [
            strip_latex(a.text or "")
            for a in entry.findall("atom:author/atom:name", ns)
            if a.text
        ]
        return True, {
            "title": re.sub(r"\s+", " ", (title_el.text or "").strip()) if title_el is not None else "",
            "authors": authors,
        }
    except Exception as exc:
        return False, {"error": str(exc)}


def fetch_crossref_search(title: str, surname: str, timeout: int = 20) -> tuple[bool, dict]:
    q = urllib.parse.urlencode({
        "query.title": strip_latex(title),
        "query.author": surname,
        "rows": "3",
    })
    api_url = f"https://api.crossref.org/works?{q}"
    req = urllib.request.Request(api_url, headers={"User-Agent": USER_AGENT}, method="GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            items = data.get("message", {}).get("items", [])
            if items:
                return True, items[0]
            return False, {"error": "no crossref search hit"}
    except Exception as exc:
        return False, {"error": str(exc)}


def main() -> None:
    parser = argparse.ArgumentParser(description="Verify citations in paper/refs.bib and paper/main.tex.")
    parser.add_argument("--offline", action="store_true", help="Only validate BibTeX syntax & TeX cross-references without network calls")
    parser.add_argument("--write-audit", action="store_true", help="Write results/citation-audit.json (always written in live mode)")
    args = parser.parse_args()

    if not BIB_PATH.exists():
        print(f"FAIL: {BIB_PATH} not found", file=sys.stderr)
        sys.exit(1)

    raw_bib = BIB_PATH.read_text(encoding="utf-8")
    parsed = parse_bibtex(raw_bib)
    keys = [e["key"] for e in parsed]
    dupes = sorted({k for k in keys if keys.count(k) > 1})
    if dupes:
        print(f"FAIL: duplicate BibTeX keys: {dupes}", file=sys.stderr)
        sys.exit(1)

    if TEX_PATH.exists():
        raw_tex = TEX_PATH.read_text(encoding="utf-8")
        tex_nocomment = "\n".join(re.sub(r"(^|[^\\])%.*", r"\1", ln) for ln in raw_tex.splitlines())
        cited: set[str] = set()
        for m in re.finditer(r"\\cite(?:\[[^\]]*\])?\{([^}]+)\}", tex_nocomment):
            for k in m.group(1).split(","):
                k = k.strip()
                if k:
                    cited.add(k)
        bib_set = set(keys)
        missing = sorted(cited - bib_set)
        uncited = sorted(bib_set - cited)
        if missing:
            print(f"FAIL: undefined \\cite keys in main.tex: {missing}", file=sys.stderr)
            sys.exit(1)
        if uncited:
            print(f"FAIL: uncited orphan BibTeX entries in refs.bib: {uncited}", file=sys.stderr)
            sys.exit(1)
        print(f"Cross-check OK: {len(cited)}/{len(parsed)} BibTeX keys cited in paper/main.tex (0 missing, 0 orphans).")

    if len(parsed) < 14:
        print(f"FAIL: expected at least 14 verified citations in paper/refs.bib, got {len(parsed)}", file=sys.stderr)
        sys.exit(1)

    if args.offline:
        print(f"PASS (offline syntax & cross-reference check for {len(parsed)} entries).")
        return

    records: list[dict] = []
    failures: list[tuple[str, str]] = []

    for e in parsed:
        key = e["key"]
        bib_title = strip_latex(e.get("title", ""))
        surname = extract_first_surname(e.get("author", ""))
        doi = e.get("doi", "").strip()
        eprint = e.get("eprint", "").strip()
        url = e.get("url", "").strip()

        rec: dict = {
            "key": key,
            "bib_title": bib_title,
            "first_author_surname": surname,
            "doi": doi or None,
            "eprint": eprint or None,
            "url": url or None,
            "reachable": False,
            "reachability_status": "",
            "metadata_source": "",
            "remote_title": "",
            "title_overlap": 0.0,
            "author_matched": False,
            "status": "FAIL",
        }

        if doi:
            ok_h, st_h = check_doi_handle(doi)
            rec["reachable"] = ok_h
            rec["reachability_status"] = st_h
            ok_cr, msg = fetch_crossref_doi(doi)
            rec["metadata_source"] = "crossref-doi"
            if ok_cr:
                rtitle = strip_latex((msg.get("title") or [""])[0])
                rec["remote_title"] = rtitle
                ov = title_overlap(bib_title, rtitle)
                rec["title_overlap"] = round(ov, 4)
                people = msg.get("author", []) + msg.get("editor", [])
                fams = " ".join(strip_latex(p.get("family", "") + " " + p.get("name", "")) for p in people).lower()
                rec["author_matched"] = bool(surname and surname in fams)
                if ok_h and ov >= 0.50 and rec["author_matched"]:
                    rec["status"] = "PASS"
                else:
                    failures.append((key, f"DOI={doi} reach={ok_h} ov={ov:.2f} auth={rec['author_matched']} remote={rtitle!r}"))
            else:
                failures.append((key, f"Crossref lookup failed for {doi}: {msg.get('error')}"))
        elif eprint:
            target_url = url or f"https://arxiv.org/abs/{eprint}"
            ok_u, st_u = check_url(target_url)
            rec["reachable"] = ok_u
            rec["reachability_status"] = st_u
            ok_ax, meta = fetch_arxiv_meta(eprint)
            rec["metadata_source"] = "arxiv-api"
            if ok_ax:
                rtitle = strip_latex(meta.get("title", ""))
                rec["remote_title"] = rtitle
                ov = title_overlap(bib_title, rtitle)
                rec["title_overlap"] = round(ov, 4)
                auth_str = " ".join(meta.get("authors", [])).lower()
                rec["author_matched"] = bool(surname and surname in auth_str)
                if ok_u and ov >= 0.50 and rec["author_matched"]:
                    rec["status"] = "PASS"
                else:
                    failures.append((key, f"arXiv={eprint} reach={ok_u} ov={ov:.2f} auth={rec['author_matched']}"))
            else:
                failures.append((key, f"arXiv API failed for {eprint}: {meta.get('error')}"))
        elif url:
            ok_u, st_u = check_url(url)
            rec["reachable"] = ok_u
            rec["reachability_status"] = st_u
            ok_cs, msg = fetch_crossref_search(bib_title, surname)
            rec["metadata_source"] = "crossref-bibliographic-search"
            if ok_cs:
                rtitle = strip_latex((msg.get("title") or [""])[0])
                rec["remote_title"] = rtitle
                ov = title_overlap(bib_title, rtitle)
                rec["title_overlap"] = round(ov, 4)
                people = msg.get("author", []) + msg.get("editor", [])
                fams = " ".join(strip_latex(p.get("family", "") + " " + p.get("name", "")) for p in people).lower()
                rec["author_matched"] = bool(surname and surname in fams)
                if ok_u and ov >= 0.50 and rec["author_matched"]:
                    rec["status"] = "PASS"
                else:
                    failures.append((key, f"URL={url} reach={ok_u} ov={ov:.2f} auth={rec['author_matched']}"))
            else:
                failures.append((key, f"Crossref search failed for {key}: {msg.get('error')}"))
        else:
            failures.append((key, "missing doi/eprint/url"))

        records.append(rec)
        print(
            f"  [{rec['status']:<4}] {key:<16} | {rec['reachability_status']:<20} | "
            f"ov={rec['title_overlap']:.2f} auth={int(rec['author_matched'])} | {rec['metadata_source']}"
        )

    audit_doc = {
        "schema": "noetic-citation-audit/v2.2",
        "generated_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "bib_file": "paper/refs.bib",
        "tex_file": "paper/main.tex",
        "total_entries": len(records),
        "passed": sum(1 for r in records if r["status"] == "PASS"),
        "failed": len(failures),
        "entries": records,
    }
    AUDIT_OUT.parent.mkdir(parents=True, exist_ok=True)
    AUDIT_OUT.write_text(json.dumps(audit_doc, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(f"Wrote live citation audit -> {AUDIT_OUT}")

    if failures:
        print(f"\nFAIL: {len(failures)}/{len(records)} citation(s) failed live reachability or title/author alignment:", file=sys.stderr)
        for k, reason in failures:
            print(f"  - {k}: {reason}", file=sys.stderr)
        sys.exit(1)
    print(f"\nPASS: All {len(records)}/{len(records)} citations verified live (reachability + Crossref/arXiv title & author alignment)!")


if __name__ == "__main__":
    main()
