# Operations, Backups & Offsite Mirror Runbook (`docs/operations.md`)

## 1. Local Observatory Operations (`web/server.mjs`)

- **Start:** `node web/server.mjs` (listens strictly on `http://127.0.0.1:8795/`).
- **Health Check (`A34`):** `curl http://127.0.0.1:8795/healthz` returns `{"status":"ok","version":"2.1.0","worker_alive":true}`.
- **Worker Recovery:** If the isolated C11 `noetic_engine stream` worker exits or exceeds its 15s command timeout, `web/server.mjs` rejects in-flight requests cleanly (`504`) and automatically spawns a fresh worker on the next request.
- **Stop:** Send `SIGINT` or `SIGTERM`; the server closes `readline`, terminates the C11 child worker, and exits `0`.

## 2. Backups, Git Bundle & Offsite Mirror (`A35`)

Because `noetic-organism-omega` is a self-contained open research repository with no external database migrations, disaster recovery relies on three redundant layers:

1. **Primary Remote Mirror:** GitHub (`https://github.com/0thernes/noetic-organism-omega`, branch `main`).
2. **Offline Git Bundle Backup:** Create a complete, self-contained offline archive (all commits, trees, and tags) via:
   ```bash
   git bundle create ../noetic-organism-omega-v2.1.0.bundle --all
   git bundle verify ../noetic-organism-omega-v2.1.0.bundle
   ```
3. **Offsite Archival Mirrors:**
   - Local secondary drive mirror under `I:\A-Life God Plan\A-Life Master Folder Build` (external archive drive separate from `Z:\` ReFS Dev Drive).
   - Long-term public preservation via Software Heritage (`https://archive.softwareheritage.org/save/`) and `0thernes/0thernes-research-archive` pinned submodule registration.
   - Integrity verification after any restore via `CHECKSUMS.sha256` and `data/MANIFEST.sha256`.
