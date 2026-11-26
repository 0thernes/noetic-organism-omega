# Security Policy

## Supported Versions

| Version | Supported |
|---|---|
| `2.1.x` | Yes (active) |
| `< 2.1.0` | No (upgrade to `v2.1.0`) |

## Reporting a Vulnerability

If you discover a memory-safety issue in the C11 engine (`src/*.c`), a loopback/CSRF/origin bypass in the local Web Observatory (`web/server.mjs`), a Ring-0 ACL or seed-firewall bypass, or a Lean soundness/comparator discrepancy:

- **Email:** `0_0@0thernes.art` (Alexander Stephen Donahue)
- **Expected Response:** Initial acknowledgement within 72 hours; root-cause analysis, regression test, and fix disclosure on `main` once validated.
- Please include exact reproduction commands, compiler/OS versions, and minimal proof-of-concept inputs.

## Security Boundaries

See `docs/threat-model.md` for the full STRIDE threat model covering C11 ABI ingestion, loopback-only HTTP enforcement (`127.0.0.1`), CSP headers, command-queue caps, Ring-0 read-only ACL traps, and cryptographic seed-firewall tamper detection.
