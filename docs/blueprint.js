// SPDX-License-Identifier: Apache-2.0
// Strict-CSP zero-CDN interactive filter for docs/blueprint.html
(function () {
  "use strict";
  const searchInput = document.getElementById("bp-search");
  const filterBtns = Array.from(document.querySelectorAll("[data-filter-cat]"));
  const rows = Array.from(document.querySelectorAll("#bp-tbody tr"));
  const statusEl = document.getElementById("bp-count");
  const dagNodes = Array.from(document.querySelectorAll(".dag-node[data-filter-cat]"));

  let activeCat = "all";

  function applyFilters() {
    const q = (searchInput ? searchInput.value : "").trim().toLowerCase();
    let shown = 0;
    for (const tr of rows) {
      const cat = tr.getAttribute("data-cat") || "";
      const txt = (tr.textContent || "").toLowerCase();
      const catOk = activeCat === "all" || cat === activeCat;
      const qOk = !q || txt.includes(q);
      const vis = catOk && qOk;
      tr.hidden = !vis;
      if (vis) shown++;
    }
    if (statusEl) {
      statusEl.textContent = "Showing " + shown + " of " + rows.length + " theorems";
    }
    for (const btn of filterBtns) {
      const isCur = btn.getAttribute("data-filter-cat") === activeCat;
      btn.setAttribute("aria-pressed", isCur ? "true" : "false");
      btn.classList.toggle("active", isCur);
    }
  }

  for (const btn of filterBtns) {
    btn.addEventListener("click", function () {
      activeCat = btn.getAttribute("data-filter-cat") || "all";
      applyFilters();
    });
  }
  for (const node of dagNodes) {
    node.addEventListener("click", function () {
      activeCat = node.getAttribute("data-filter-cat") || "all";
      applyFilters();
      const tbl = document.getElementById("theorem-table-heading");
      if (tbl) tbl.scrollIntoView({ behavior: "smooth", block: "start" });
    });
    node.addEventListener("keydown", function (ev) {
      if (ev.key === "Enter" || ev.key === " ") {
        ev.preventDefault();
        activeCat = node.getAttribute("data-filter-cat") || "all";
        applyFilters();
      }
    });
  }
  if (searchInput) {
    searchInput.addEventListener("input", applyFilters);
  }
  applyFilters();
})();
