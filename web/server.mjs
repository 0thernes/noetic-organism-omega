import http from "node:http";
import fs from "node:fs";
import path from "node:path";
import { spawn } from "node:child_process";
import readline from "node:readline";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ROOT = path.resolve(__dirname, "..");
const PORT = Number(process.env.PORT || 8795);
const BIN = process.platform === "win32"
  ? path.join(ROOT, "bin", "noetic_engine.exe")
  : path.join(ROOT, "bin", "noetic_engine_linux");

const ALLOWED_STATIC = new Set(["index.html", "app.js"]);
const MIME = {
  ".html": "text/html; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8"
};

const SECURITY_HEADERS = {
  "X-Content-Type-Options": "nosniff",
  "Content-Security-Policy":
    "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; img-src 'self' data:; connect-src 'self'; frame-ancestors 'none'; base-uri 'none'; form-action 'none'"
};

const MAX_BODY_BYTES = 65536;
const MAX_PENDING = 16;

let worker = null;
let rl = null;
const pending = [];

function rejectAll(reason) {
  while (pending.length > 0) {
    const j = pending.shift();
    clearTimeout(j.timer);
    j.reject(new Error(reason));
  }
}

function startWorker() {
  if (rl) { try { rl.close(); } catch {} rl = null; }
  if (worker) { try { worker.kill(); } catch {} worker = null; }
  rejectAll("worker restarted");

  const w = spawn(BIN, ["stream"], { stdio: ["pipe", "pipe", "pipe"] });
  const r = readline.createInterface({ input: w.stdout });
  worker = w;
  rl = r;

  r.on("line", (line) => {
    if (worker !== w) return;
    const job = pending.shift();
    if (job) { clearTimeout(job.timer); job.resolve(line); }
  });
  w.on("error", (err) => {
    if (worker !== w) return;
    worker = null; rl = null;
    rejectAll(`spawn error: ${err.message}`);
  });
  w.on("exit", (code) => {
    if (worker !== w) return;
    worker = null; rl = null;
    rejectAll(`worker exit ${code}`);
  });
}

function sendCmd(cmd) {
  if (pending.length >= MAX_PENDING) {
    const e = new Error("Command queue full"); e.status = 429;
    return Promise.reject(e);
  }
  if (!worker || !worker.stdin || !worker.stdin.writable) startWorker();
  const w = worker;
  return new Promise((resolve, reject) => {
    const job = { resolve, reject, timer: null };
    job.timer = setTimeout(() => {
      const i = pending.indexOf(job);
      if (i !== -1) pending.splice(i, 1);
      const e = new Error("Engine timeout"); e.status = 504;
      reject(e);
      if (worker === w) { try { w.kill(); } catch {} }
    }, 15000);
    pending.push(job);
    w.stdin.write(cmd + "\n");
  });
}

function isLoopback(req) {
  const host = String(req.headers.host || "");
  if (!/^(127\.0\.0\.1|localhost|\[::1\])(:\d+)?$/i.test(host)) return false;
  const origin = req.headers.origin;
  if (origin && !/^https?:\/\/(127\.0\.0\.1|localhost|\[::1\])(:\d+)?$/i.test(String(origin))) return false;
  const fsSite = req.headers["sec-fetch-site"];
  if (fsSite !== undefined) {
    const s = String(fsSite).toLowerCase();
    if (s !== "same-origin" && s !== "none") return false;
  }
  if (req.method === "POST") {
    const ct = String(req.headers["content-type"] || "");
    if (!/^application\/json(\s*;.*)?$/i.test(ct)) return false;
  }
  return true;
}

function readJson(req) {
  return new Promise((resolve, reject) => {
    const chunks = [];
    let bytes = 0, done = false;
    req.on("data", (c) => {
      if (done) return;
      bytes += c.length;
      if (bytes > MAX_BODY_BYTES) {
        done = true;
        const e = new Error("Payload too large"); e.status = 413;
        req.pause(); reject(e); return;
      }
      chunks.push(c);
    });
    req.on("end", () => {
      if (done) return;
      done = true;
      try { resolve(JSON.parse(Buffer.concat(chunks).toString("utf8") || "{}")); }
      catch { const e = new Error("Bad JSON"); e.status = 400; reject(e); }
    });
    req.on("error", (e) => { if (!done) { done = true; reject(e); } });
  });
}

function clampInt(v, def, lo, hi) {
  const n = Number.parseInt(v, 10);
  return Number.isFinite(n) ? Math.max(lo, Math.min(hi, n)) : def;
}

function sendJson(res, status, body, extra = {}) {
  res.writeHead(status, {
    ...SECURITY_HEADERS,
    "Content-Type": "application/json; charset=utf-8",
    "Cache-Control": "no-store",
    ...extra
  });
  res.end(typeof body === "string" ? body : JSON.stringify(body));
}

function serveResultJson(res, filename) {
  const fp = path.join(ROOT, "results", filename);
  fs.readFile(fp, "utf8", (err, txt) => {
    if (err) return sendJson(res, 404, { error: `Result ${filename} not yet generated` });
    sendJson(res, 200, txt);
  });
}

startWorker();

const server = http.createServer(async (req, res) => {
  let u;
  try { u = new URL(req.url || "/", `http://127.0.0.1:${PORT}`); }
  catch { return sendJson(res, 400, { error: "Bad URL" }); }

  if (u.pathname.startsWith("/api/")) {
    if (!isLoopback(req)) return sendJson(res, 403, { error: "Forbidden host/origin/content-type" });

    if (u.pathname === "/api/state" && req.method === "GET") {
      try { return sendJson(res, 200, await sendCmd("state")); }
      catch (e) { return sendJson(res, e.status || 500, { error: e.message }); }
    }
    if (u.pathname === "/api/e1" && req.method === "GET") return serveResultJson(res, "m8_e1_ablation_sweep.json");
    if (u.pathname === "/api/e2" && req.method === "GET") return serveResultJson(res, "m8_e2_cognitive_49.json");
    if (u.pathname === "/api/e3" && req.method === "GET") return serveResultJson(res, "m8_e3_scaling_bench.json");
    if (u.pathname === "/api/gates" && req.method === "GET") return serveResultJson(res, "m9_native_release_gates.json");
    if (u.pathname === "/api/firewall" && req.method === "GET") return serveResultJson(res, "seed_firewall_ledger.json");
    if (u.pathname === "/api/trace825" && req.method === "GET") return serveResultJson(res, "m8_e2_825_honest_trace_summary.json");
    if (u.pathname === "/api/hw24" && req.method === "GET") return serveResultJson(res, "m10_hw24_receipt.json");
    if (u.pathname === "/api/soft40" && req.method === "GET") return serveResultJson(res, "m10_soft40_receipt.json");
    if (u.pathname === "/api/freak100" && req.method === "GET") return serveResultJson(res, "m10_freak100_receipt.json");
    if (u.pathname === "/api/novel100" && req.method === "GET") return serveResultJson(res, "m10_novel100_receipt.json");
    if (u.pathname === "/api/life500" && req.method === "GET") return serveResultJson(res, "m10_life500_receipt.json");
    if (u.pathname === "/api/milestones100" && req.method === "GET") return serveResultJson(res, "m10_milestones_100_receipt.json");
    if (u.pathname === "/api/g1" && req.method === "GET") return serveResultJson(res, "m9_g1_foundation_gate_receipt.json");

    if (u.pathname === "/api/cmd") {
      if (req.method !== "POST") return sendJson(res, 405, { error: "Use POST /api/cmd" }, { Allow: "POST" });
      try {
        const msg = await readJson(req);
        let cmd = null;
        if (msg.op === "state") cmd = "state";
        else if (msg.op === "step") cmd = `step ${clampInt(msg.n, 2, 1, 60)}`;
        else if (msg.op === "init") {
          const sd = clampInt(msg.seed, 5001, 1, 99999999);
          const arm = clampInt(msg.arm, 0, 0, 4);
          const mask = clampInt(msg.mask, 0, 0, 262143);
          cmd = `init ${sd} ${arm} ${mask}`;
        } else if (msg.op === "ablate") {
          cmd = `ablate ${clampInt(msg.mask, 0, 0, 262143)}`;
        } else if (msg.op === "pci") {
          cmd = "pci";
        } else if (msg.op === "dna" && typeof msg.hex === "string" && /^[0-9a-fA-F]{64,256}$/.test(msg.hex)) {
          cmd = `dna ${msg.hex}`;
        }
        if (!cmd) return sendJson(res, 400, { error: "Invalid op" });
        return sendJson(res, 200, await sendCmd(cmd));
      } catch (e) {
        return sendJson(res, e.status || 500, { error: e.message });
      }
    }
    return sendJson(res, 404, { error: "Unknown API" });
  }

  if (req.method !== "GET" && req.method !== "HEAD") {
    res.writeHead(405, { ...SECURITY_HEADERS, Allow: "GET, HEAD" });
    return res.end("Method not allowed");
  }
  const rel = u.pathname === "/" ? "index.html" : u.pathname.replace(/^\/+/, "");
  if (!ALLOWED_STATIC.has(rel)) {
    res.writeHead(404, SECURITY_HEADERS);
    return res.end("Not found");
  }
  const fp = path.join(__dirname, rel);
  fs.readFile(fp, (err, data) => {
    if (err) { res.writeHead(404, SECURITY_HEADERS); return res.end("Not found"); }
    res.writeHead(200, { ...SECURITY_HEADERS, "Content-Type": MIME[path.extname(fp)] || "application/octet-stream", "Cache-Control": "no-store" });
    res.end(req.method === "HEAD" ? undefined : data);
  });
});

function cleanup() {
  if (rl) { try { rl.close(); } catch {} }
  if (worker) { try { worker.kill(); } catch {} }
  process.exit(0);
}
process.on("SIGINT", cleanup);
process.on("SIGTERM", cleanup);

server.listen(PORT, "127.0.0.1", () => {
  console.log(`NOETIC ORGANISM OMEGA v2.0.0 Observatory listening on http://127.0.0.1:${PORT}/`);
});
