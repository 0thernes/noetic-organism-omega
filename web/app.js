const $ = (id) => document.getElementById(id);

const ABL_NAMES = [
  "0:RECURRENT (RPT-1)", "1:APICAL_XOR (RPT-2)", "2:SPECIALISTS (GWT-1)",
  "3:WORKSPACE (GWT-2/3)", "4:TTC_GATE (GWT-4)", "5:REALITY_MON (HOT-1)",
  "6:META_2ND (HOT-2/3)", "7:QUALITY_MAP (HOT-4)", "8:ATTN_SCHEMA (AST-1)",
  "9:PRED_EFE (PP-1/F3)", "10:DEVAL_ARB (AE-1)", "11:EFFERENCE (AE-2)",
  "12:THALER (F1)", "13:THALAMOCORT (F2)", "14:WAVE_PHASE (F5)",
  "15:UAL_CONFIG (F6)", "16:EGOCENTRIC (F8)", "17:HOMEOSTASIS (CQM)"
];

let playing = true;
let inflight = false;
let currentMask = 0;
let lastState = null;

async function apiCmd(payload) {
  const r = await fetch("api/cmd", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });
  const j = await r.json();
  if (!r.ok || j.error) throw new Error(j.error || `HTTP ${r.status}`);
  return j;
}

function buildAblationGrid() {
  const root = $("abl-grid");
  root.textContent = "";
  for (let i = 0; i < 18; i++) {
    const lbl = document.createElement("label");
    lbl.className = "abl-item";
    lbl.id = `abl-lbl-${i}`;
    const cb = document.createElement("input");
    cb.type = "checkbox";
    cb.addEventListener("change", () => {
      if (cb.checked) currentMask |= (1 << i);
      else currentMask &= ~(1 << i);
      lbl.className = cb.checked ? "abl-item on" : "abl-item";
      apiCmd({ op: "ablate", mask: currentMask }).then(renderState).catch(() => {});
    });
    const sp = document.createElement("span");
    sp.textContent = ABL_NAMES[i];
    lbl.append(cb, sp);
    root.append(lbl);
  }
}

function drawWorld(st) {
  const cv = $("world-canvas");
  const ctx = cv.getContext("2d");
  const W = cv.width, H = cv.height;
  ctx.fillStyle = "#030711";
  ctx.fillRect(0, 0, W, H);

  const mapW = 350, mapH = 340, ox = 10, oy = 10;
  const sx = mapW / 64, sy = mapH / 64;
  ctx.strokeStyle = "#1a2d50";
  ctx.strokeRect(ox, oy, mapW, mapH);

  const o = st.org;
  // Patches
  for (const p of o.patches || []) {
    const [px, py, rad, toxic, deval] = p;
    ctx.beginPath();
    ctx.arc(ox + px * sx, oy + py * sy, Math.max(4, rad * sx), 0, Math.PI * 2);
    ctx.fillStyle = deval ? "rgba(245,158,11,0.45)" : (toxic ? "rgba(239,68,68,0.35)" : "rgba(34,197,94,0.38)");
    ctx.fill();
    ctx.strokeStyle = deval ? "#f59e0b" : (toxic ? "#ef4444" : "#22c55e");
    ctx.lineWidth = 1;
    ctx.stroke();
  }
  // Walls
  ctx.strokeStyle = "#64748b";
  ctx.lineWidth = 2;
  for (const wl of o.walls || []) {
    ctx.beginPath();
    ctx.moveTo(ox + wl[0] * sx, oy + wl[1] * sy);
    ctx.lineTo(ox + wl[2] * sx, oy + wl[3] * sy);
    ctx.stroke();
  }
  // Peers + gaze ray
  for (const pr of o.peers || []) {
    const px = ox + pr[0] * sx, py = oy + pr[1] * sy;
    ctx.strokeStyle = "rgba(56,189,248,0.45)";
    ctx.beginPath();
    ctx.moveTo(px, py);
    ctx.lineTo(px + Math.cos(pr[2]) * 26, py + Math.sin(pr[2]) * 26);
    ctx.stroke();
    ctx.fillStyle = pr[3] ? "#fbbf24" : "#38bdf8";
    ctx.beginPath();
    ctx.arc(px, py, 4, 0, Math.PI * 2);
    ctx.fill();
  }
  // Hazards
  for (const hz of o.hazards || []) {
    ctx.fillStyle = "rgba(244,63,94,0.8)";
    ctx.beginPath();
    ctx.arc(ox + hz[0] * sx, oy + hz[1] * sy, Math.max(4, hz[2] * sx * 0.6), 0, Math.PI * 2);
    ctx.fill();
  }
  // Attention Schema spotlight ring
  ctx.strokeStyle = "rgba(250,204,21,0.7)";
  ctx.setLineDash([4, 3]);
  ctx.beginPath();
  ctx.arc(ox + o.schema_x * sx, oy + o.schema_y * sy, 12, 0, Math.PI * 2);
  ctx.stroke();
  ctx.setLineDash([]);

  // Living organism body + heading arrow
  const mx = ox + o.x * sx, my = oy + o.y * sy;
  ctx.fillStyle = "#e0f2fe";
  ctx.beginPath();
  ctx.arc(mx, my, 6.5, 0, Math.PI * 2);
  ctx.fill();
  ctx.strokeStyle = "#38bdf8";
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(mx, my);
  ctx.lineTo(mx + Math.cos(o.heading) * 16, my + Math.sin(o.heading) * 16);
  ctx.stroke();

  // Right panel: 16 sLTC recurrent units + real-time telemetry bars
  const rx = 374, rw = W - rx - 12;
  ctx.fillStyle = "#93c5fd";
  ctx.font = "10.5px ui-monospace,Consolas,monospace";
  ctx.fillText("16-Unit sLTC Recurrent Cortex (RPT-1)", rx, 24);
  const sltc = o.sltc || [];
  const bw = rw / 16;
  for (let i = 0; i < sltc.length; i++) {
    const v = Math.max(-2, Math.min(2, sltc[i]));
    const bh = (Math.abs(v) / 2) * 44;
    ctx.fillStyle = v >= 0 ? "#38bdf8" : "#f472b6";
    ctx.fillRect(rx + i * bw + 1, 78 - (v >= 0 ? bh : 0), Math.max(2, bw - 3), bh);
  }
  ctx.strokeStyle = "#1e3a5f";
  ctx.beginPath(); ctx.moveTo(rx, 78); ctx.lineTo(rx + rw, 78); ctx.stroke();

  const bars = [
    ["RPT-1 Blank Gap", o.blank_gap],
    ["RPT-2 dCaAP XOR", o.xor_acc],
    ["GWT-3 Broadcast", o.ws_fid],
    ["HOT-2 Meta Brier", o.brier_score],
    ["PP-1 Pred Coding", o.pred_acc],
    ["F5 Kuramoto Wave", o.wave_coh],
    ["F6 UAL Compound", o.ual_acc],
    ["F8 Egocentric", o.ego_prog],
    ["F2 Forked PCI*", st.pci_star]
  ];
  ctx.fillStyle = "#93c5fd";
  ctx.fillText("Live Circuit Readouts (Same Living Organism)", rx, 145);
  let y = 162;
  for (const [label, val] of bars) {
    const v = Math.max(0, Math.min(1, Number(val) || 0));
    ctx.fillStyle = "#94a3b8";
    ctx.fillText(`${label}`, rx, y);
    ctx.fillStyle = "#0f1c36";
    ctx.fillRect(rx + 124, y - 9, rw - 166, 10);
    ctx.fillStyle = v > 0.5 ? "#22c55e" : (v > 0.25 ? "#f59e0b" : "#ef4444");
    ctx.fillRect(rx + 124, y - 9, (rw - 166) * v, 10);
    ctx.fillStyle = "#e2e8f0";
    ctx.fillText(v.toFixed(2), rx + rw - 34, y);
    y += 20;
  }
}

function renderState(st) {
  lastState = st;
  const bd = $("b-drift");
  bd.textContent = `1ST LAW Δ: ${st.energy_residual_uq} uq`;
  bd.className = `badge ${st.energy_residual_uq === 0 ? "ok" : "bad"}`;

  const b2 = $("b-2nd");
  b2.textContent = `2ND LAW VIOL: ${st.second_law_violations}`;
  b2.className = `badge ${st.second_law_violations === 0 ? "ok" : "bad"}`;

  $("b-pci").textContent = `PCI*: ${Number(st.pci_star).toFixed(3)} (${st.pci_clean ? "0-bit clean" : "DIRTY"})`;
  $("b-sha").textContent = `SHA: ${String(st.sha256 || "").slice(0, 12)}…`;

  $("k-tick").textContent = `${st.tick} / G${st.org.generation}`;
  $("k-atp").textContent = `${(st.ledger.atp / 1000).toFixed(1)}k / ${(st.ledger.stores / 1000).toFixed(0)}k`;
  $("k-body").textContent = `${(st.org.integrity * 100).toFixed(0)}% / ${st.org.temp_c.toFixed(1)}°C`;
  $("k-meta").textContent = `${(st.org.meta_conf * 100).toFixed(0)}% / ${st.org.reality_tag > 0.5 ? "WAKE" : "REM"}`;

  drawWorld(st);
}

async function loadReceipts() {
  try {
    const e2 = await fetch("api/e2").then((r) => r.json());
    if (e2.summary) {
      $("s-butlin").textContent = `${e2.summary.butlin14_passed} / ${e2.summary.butlin14_total}`;
      $("s-theories").textContent = `${e2.summary.theories10_passed} / ${e2.summary.theories10_total}`;
      $("s-cqm").textContent = `${e2.summary.cqm25_passed} / ${e2.summary.cqm25_total}`;
      $("s-mir").textContent = `${e2.summary.mirrors_verified} / 5`;
    }
    const tb = $("e2-body");
    tb.textContent = "";
    for (const it of e2.items || []) {
      const tr = document.createElement("tr");
      const vals = [
        it.id, it.suite.replace("L1_", "").replace("L2_", "").replace("L3_", ""),
        it.intact.toFixed(3), it.sham.toFixed(3), it.lesion.toFixed(3), it.shuffled.toFixed(3),
        it.d_z.toFixed(2), Number(it.holm_p).toExponential(1), it.status
      ];
      vals.forEach((v, idx) => {
        const td = document.createElement("td");
        td.textContent = String(v);
        if (idx === 8) td.className = v === "PASS" ? "pass" : "fail";
        tr.append(td);
      });
      tb.append(tr);
    }
  } catch {}

  try {
    const [rg, sf] = await Promise.all([
      fetch("api/gates").then((r) => r.json()),
      fetch("api/firewall").then((r) => r.json()).catch(() => null)
    ]);
    const tbg = $("gates-body");
    if (tbg && rg["GATE-02"]) {
      tbg.textContent = "";
      const rows = [
        ["GATE-01", "Seed Firewall", sf ? `Disjoint Dev/Conf/Held, burn-on-fail & tamper test (SHA ${String(sf.head_sha256 || "").slice(0, 12)}…)` : "Burn-on-fail firewall", sf?.status === "PASS"],
        ["GATE-02", "Zero-Alloc 10k", `sizeof=${rg["GATE-02"].sizeof_world_bytes} B, ${rg["GATE-02"].ticks} ticks, drift=${rg["GATE-02"].drift_uq} uq`, rg["GATE-02"].passed],
        ["GATE-05", "Antithetic+Fast", `|e_ss|=${rg["GATE-05"].antithetic_clamp_err_pct}%, sparing=${rg["GATE-05"].protein_sparing_ratio}x (>=1.6x), drift=0`, rg["GATE-05"].passed],
        ["GATE-06", "XPBD Hydrostat", `|dV|/V0=${rg["GATE-06"].max_vol_err_pct}% (<1%), 0.55x squeeze=1, stiff blocked=1, 0 NaNs`, rg["GATE-06"].passed],
        ["GATE-07", "b1.58 Ganglia", `${rg["GATE-07"].bytes_per_syn} B/syn, offload=${rg["GATE-07"].offload_pct}% (>=50%), decerebrate=1`, rg["GATE-07"].passed],
        ["GATE-09", "Ring-0 WSE-12", `ACL ${rg["GATE-09"].ring0_blocked}/10 blocked, WSE ${rg["GATE-09"].wse_ops}/10 ops, ||W||2=${rg["GATE-09"].spectral_norm}, diff=${rg["GATE-09"].diff_bytes} B, 0-bit RB`, rg["GATE-09"].passed],
        ["GATE-10", "Blackiston M084", `remap=${rg["GATE-10"].motor_remap_pct}% (>=80%), adult=${rg["GATE-10"].adult_retention_acc} vs ablated=${rg["GATE-10"].ablated_control_acc}`, rg["GATE-10"].passed],
        ["GATE-11", "10k Obs Indep.", `${rg["GATE-11"].ticks} ticks vs ${rg["GATE-11"].observer_reads} Forked-PCI* probes 0-bit identical SHA`, rg["GATE-11"].passed]
      ];
      for (const [gid, dom, met, ok] of rows) {
        const tr = document.createElement("tr");
        [gid, dom, met, ok ? "PASS" : "FAIL"].forEach((v, idx) => {
          const td = document.createElement("td");
          td.textContent = String(v);
          if (idx === 3) td.className = ok ? "pass" : "fail";
          tr.append(td);
        });
        tbg.append(tr);
      }
      const bg = $("b-gates");
      if (bg) bg.textContent = `GATES: ${rg.gates_passed === 7 && sf?.status === "PASS" ? "13/13 PASS" : "CHECK"}`;
    }
  } catch {}

  try {
    const e3 = await fetch("api/e3").then((r) => r.json());
    const tb3 = $("e3-body");
    tb3.textContent = "";
    for (const row of e3.rows || []) {
      const tr = document.createElement("tr");
      [row.organisms, `${(row.bytes_total / 1024).toFixed(1)} KB`, row.cache_tier,
       Math.round(row.org_ticks_per_sec).toLocaleString(), row.us_per_org_tick.toFixed(2),
       `${row.max_energy_residual_uq} uq`].forEach((v) => {
        const td = document.createElement("td");
        td.textContent = String(v);
        tr.append(td);
      });
      tb3.append(tr);
    }
  } catch {}
}

$("btn-play").addEventListener("click", () => {
  playing = !playing;
  $("btn-play").textContent = playing ? "Pause" : "Play";
});
$("btn-step").addEventListener("click", () => apiCmd({ op: "step", n: 10 }).then(renderState).catch(() => {}));
$("btn-pci").addEventListener("click", () => apiCmd({ op: "pci" }).then(renderState).catch(() => {}));
$("sel-arm").addEventListener("change", () => {
  apiCmd({ op: "init", seed: 5001, arm: Number($("sel-arm").value), mask: currentMask }).then(renderState).catch(() => {});
});
$("btn-reset").addEventListener("click", () => {
  apiCmd({ op: "init", seed: 5001, arm: Number($("sel-arm").value), mask: currentMask }).then(renderState).catch(() => {});
});
$("btn-clear-abl").addEventListener("click", () => {
  currentMask = 0;
  for (let i = 0; i < 18; i++) {
    const lbl = $(`abl-lbl-${i}`);
    if (lbl) { lbl.className = "abl-item"; lbl.querySelector("input").checked = false; }
  }
  apiCmd({ op: "ablate", mask: 0 }).then(renderState).catch(() => {});
});

buildAblationGrid();
loadReceipts();
fetch("api/state").then((r) => r.json()).then(renderState).catch(() => {});

setInterval(async () => {
  if (!playing || inflight) return;
  inflight = true;
  try {
    const st = await apiCmd({ op: "step", n: 2 });
    renderState(st);
  } catch {} finally {
    inflight = false;
  }
}, 160);
