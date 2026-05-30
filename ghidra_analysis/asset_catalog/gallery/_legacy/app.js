// Bulanci Asset Catalog gallery — single-file vanilla frontend.
//
// Renders the merged catalog (catalog.json + registry.toml + coverage.json)
// fetched from /api/catalog as a filterable tile grid.  Click a tile to
// open the detail panel with full xref dump + preview.

const $  = (sel) => document.querySelector(sel);
const $$ = (sel) => Array.from(document.querySelectorAll(sel));

const state = {
  payload:    null,     // raw merged catalog payload
  visible:    [],       // entries after filters applied
  filters:    {
    named:    null,     // 'named' | 'unnamed' | null
    category: null,     // category label or null
    class:    null,     // className or null
    search:   "",
  },
  selected:   null,     // selected entry.key or null
};

// ---------------------------------------------------------------------------
// boot
// ---------------------------------------------------------------------------
(async function boot() {
  try {
    const resp = await fetch("/api/catalog", { cache: "no-store" });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    state.payload = await resp.json();
  } catch (err) {
    $("#grid").textContent = `failed to load catalog: ${err.message}`;
    return;
  }
  renderSummary();
  renderFacets();
  bindEvents();
  applyFilters();
})();

// ---------------------------------------------------------------------------
// rendering
// ---------------------------------------------------------------------------
function renderSummary() {
  const { totals, coverage } = state.payload;
  $("#summary").innerHTML =
    `<strong>${totals.assets}</strong> assets &middot; ` +
    `<strong>${totals.named}</strong> named &middot; ` +
    `<strong>${totals.unnamed}</strong> to-do<br>` +
    `coverage: <strong>${coverage.withConcreteXref ?? "?"}</strong> concrete, ` +
    `${coverage.orphanOnly ?? 0} orphan, ${coverage.deadOnly ?? 0} dead`;
}

function renderFacets() {
  const { totals, categoryCounts, classCounts } = state.payload;

  // Name status — fixed two options.
  renderFacetList($("#facet-named"), [
    ["all",     null,       totals.assets],
    ["named",   "named",    totals.named],
    ["unnamed", "unnamed",  totals.unnamed],
  ], "named");

  // Category — sort by count desc, then alpha.
  const catItems = Object.entries(categoryCounts)
    .sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0]))
    .map(([k, v]) => [k, k, v]);
  catItems.unshift(["all", null, totals.assets]);
  renderFacetList($("#facet-category"), catItems, "category");

  // Class — sort alpha.
  const clsItems = Object.entries(classCounts)
    .sort((a, b) => a[0].localeCompare(b[0]))
    .map(([k, v]) => [k, k, v]);
  clsItems.unshift(["all", null, totals.assets]);
  renderFacetList($("#facet-class"), clsItems, "class");
}

function renderFacetList(ul, items, filterKey) {
  ul.innerHTML = "";
  for (const [label, value, count] of items) {
    const li = document.createElement("li");
    li.dataset.value = value ?? "";
    li.innerHTML = `<span>${label}</span><span class="count">${count}</span>`;
    if (state.filters[filterKey] === value) li.classList.add("active");
    li.addEventListener("click", () => {
      state.filters[filterKey] = value;
      renderFacets();
      applyFilters();
    });
    ul.appendChild(li);
  }
}

function renderActiveFilters() {
  const f = state.filters;
  const chunks = [];
  if (f.named)    chunks.push(`name: <strong>${f.named}</strong>`);
  if (f.category) chunks.push(`category: <strong>${f.category}</strong>`);
  if (f.class)    chunks.push(`class: <strong>${f.class}</strong>`);
  if (f.search)   chunks.push(`search: <strong>${escapeHtml(f.search)}</strong>`);

  const el = $("#active-filters");
  if (chunks.length === 0) { el.innerHTML = ""; return; }
  el.innerHTML = chunks.map(c => `<div>${c}</div>`).join("") +
                 `<button class="clear-all" type="button">clear all</button>`;
  el.querySelector(".clear-all").addEventListener("click", () => {
    state.filters = { named: null, category: null, class: null, search: "" };
    $("#search").value = "";
    renderFacets();
    applyFilters();
  });
}

// ---------------------------------------------------------------------------
// filtering
// ---------------------------------------------------------------------------
function applyFilters() {
  const f = state.filters;
  const q = (f.search || "").toLowerCase().trim();
  state.visible = state.payload.entries.filter(e => {
    if (f.named === "named"    && !e.slug) return false;
    if (f.named === "unnamed"  &&  e.slug) return false;
    if (f.category && !e.categories.includes(f.category)) return false;
    if (f.class    && e.className !== f.class) return false;
    if (q) {
      const hay = [
        e.idHex, String(e.id),
        e.slug || "", e.notes || "",
        (e.tags || []).join(" "),
        e.className,
        e.categories.join(" "),
      ].join(" ").toLowerCase();
      if (!hay.includes(q)) return false;
    }
    return true;
  });
  renderGrid();
  renderActiveFilters();
}

function renderGrid() {
  const grid = $("#grid");
  grid.innerHTML = "";
  if (state.visible.length === 0) {
    grid.innerHTML = `<div style="color: var(--fg-faint); padding: 20px;">no matches</div>`;
    return;
  }
  // Use a DocumentFragment + cap render to keep first paint snappy on
  // wide filters that match 300+ tiles.
  const frag = document.createDocumentFragment();
  for (const e of state.visible) {
    frag.appendChild(renderTile(e));
  }
  grid.appendChild(frag);
}

function renderTile(e) {
  const tile = document.createElement("div");
  tile.className = "tile";
  tile.dataset.key = e.key;
  if (state.selected === e.key) tile.classList.add("selected");

  const preview = document.createElement("div");
  preview.className = "tile-preview";

  // Pick the right placeholder per class.  Audio / text classes have
  // a `preview` filename (the .mp3 / .wav / .txt) but the browser
  // can't decode those as images, so we render a class-specific
  // glyph instead.  Real images still use <img> with image-rendering
  // pixelated for crisp pixel art.
  const isAudio   = e.className === "Mp3" || e.className === "AudioBank";
  const isPoem    = e.className === "Poem";
  const isText    = (e.className === "Script" || e.className === "HistoryScript"
                 || e.className === "DsmInner");
  const isVisual  = e.preview && !isAudio && !isPoem && !isText;

  if (isVisual) {
    const img = document.createElement("img");
    img.src = `/api/preview/${e.key}`;
    img.alt = e.idHex;
    img.loading = "lazy";
    preview.appendChild(img);
  } else if (isPoem && e.poemText) {
    // Render the first few aligned lines as the thumbnail.
    preview.classList.add("ph-poem");
    preview.innerHTML = renderPoem(e.poemText, { maxLines: 5, condensed: true });
  } else if (isAudio) {
    preview.classList.add("placeholder", "ph-audio");
    preview.innerHTML = `<div class="ph-icon">&#9836;</div>` +
                        `<div class="ph-label">${e.className.toLowerCase()}</div>`;
  } else if (isText || isPoem) {
    preview.classList.add("placeholder", "ph-text");
    preview.innerHTML = `<div class="ph-icon">&#9776;</div>` +
                        `<div class="ph-label">${e.className.toLowerCase()}</div>`;
  } else {
    preview.classList.add("placeholder");
    preview.textContent = "no preview";
  }
  preview.insertAdjacentHTML(
    "beforeend",
    `<span class="class-badge">${e.className}</span>`,
  );
  if (e.deadOnly) {
    preview.insertAdjacentHTML(
      "beforeend",
      `<span class="flag-badge flag-dead">DEAD</span>`,
    );
  } else if (e.orphanOnly) {
    preview.insertAdjacentHTML(
      "beforeend",
      `<span class="flag-badge flag-orphan">ORPHAN</span>`,
    );
  }
  tile.appendChild(preview);

  const body = document.createElement("div");
  body.className = "tile-body";
  body.innerHTML = `
    <div class="tile-id">${e.idHex} &middot; ${e.id}</div>
    <div class="tile-slug ${e.slug ? "" : "unnamed"}">${e.slug || "(unnamed)"}</div>
    <div class="tile-cats">${e.categories.join(" · ")}</div>
  `;
  tile.appendChild(body);

  tile.addEventListener("click", () => showDetail(e));
  return tile;
}

// ---------------------------------------------------------------------------
// detail panel
// ---------------------------------------------------------------------------
function showDetail(e) {
  // Stop any animation timer left by the previous detail view.
  if (state._animTimer) { clearInterval(state._animTimer); state._animTimer = null; }

  // Surgically move the .selected class — DO NOT re-render the whole
  // grid here, that would destroy and recreate every <img> in the
  // grid and trigger a refetch of all 200+ previews on every click.
  state.selected = e.key;
  $$(".tile.selected").forEach(t => t.classList.remove("selected"));
  document.querySelector(`.tile[data-key="${e.key}"]`)?.classList.add("selected");

  $("#detail").classList.remove("hidden");
  const title = $("#detail-title");
  title.innerHTML = `
    <div class="d-slug ${e.slug ? "" : "unnamed"}">${e.slug || "(unnamed)"}</div>
    <div class="d-id">${e.idHex} &middot; ${e.id} &middot; ${e.className}</div>
  `;
  $("#detail-body").innerHTML = renderDetailBody(e);

  // Wire up category-click → filter (so user can jump to "all assets in same script").
  $$("#detail-body .cat").forEach(el => {
    el.addEventListener("click", () => {
      state.filters.category = el.dataset.cat;
      renderFacets();
      applyFilters();
    });
  });
}

function renderDetailBody(e) {
  return [
    renderPreviewSection(e),
    renderMetaSection(e),
    renderCategoriesSection(e),
    renderNotesSection(e),
    renderXrefsSection(e),
    renderFilesSection(e),
  ].join("");
}

function renderPreviewSection(e) {
  let body;
  if (e.className === "Mp3") {
    body = `<audio controls src="/api/preview/${e.key}"></audio>`;
  } else if (e.className === "AudioBank") {
    body = `<audio controls src="/api/preview/${e.key}"></audio>`;
  } else if (e.className === "Poem" && e.poemText) {
    body = `<div class="poem-detail">${renderPoem(e.poemText, { maxLines: 0 })}</div>`;
  } else if (e.anim && e.anim.kind === "gif") {
    // Strip = the wide stitched atlas PNG.  Use anim.strip explicitly
    // (set server-side) so we hit a different URL than the GIF above.
    const stripImg = e.anim.strip
      ? `<img class="anim-strip" src="/api/file/${encodeURIComponent(e.anim.strip)}" alt="${e.idHex} strip" />`
      : "";
    body = `
      <div class="anim-stack">
        <img class="anim-gif" src="/api/file/${encodeURIComponent(e.anim.filename)}" alt="${e.idHex}" />
        ${stripImg}
        <div class="anim-caption">animated${e.anim.strip ? " &middot; static atlas strip below" : ""}</div>
      </div>`;
  } else if (e.anim && e.anim.kind === "frames") {
    body = renderJpegAnimPlayer(e);
  } else if (e.preview) {
    body = `<img src="/api/preview/${e.key}" alt="${e.idHex}" />`;
  } else if (e.className === "Script" || e.className === "HistoryScript") {
    body = `<div class="none">preview: see &quot;Files&quot; below for raw text/asm</div>`;
  } else {
    body = `<div class="none">no preview for ${e.className}</div>`;
  }
  return `<section class="preview-large">${body}</section>`;
}

// ---------------------------------------------------------------------------
// poem rendering
// ---------------------------------------------------------------------------

// Parse `[CENTER] ...` / `[LEFT] ...` / `[RIGHT] ...` markers and emit HTML.
// `maxLines === 0` means render everything; otherwise truncate.
// `condensed` mode produces tighter line spacing for thumbnails.
function renderPoem(raw, { maxLines = 0, condensed = false } = {}) {
  const lines = raw.replace(/\r\n/g, "\n").split("\n");
  const out = [];
  let kept = 0;
  let truncated = false;
  for (const rawLine of lines) {
    const line = rawLine.trim();
    if (line === "") {
      // Skip blank lines in condensed mode; keep as spacers in detail.
      if (!condensed) out.push(`<div class="poem-blank"></div>`);
      continue;
    }
    let align = "left";
    let text = line;
    const m = /^\[(LEFT|CENTER|RIGHT|TITLE)\]\s*(.*)$/i.exec(line);
    if (m) {
      const tag = m[1].toUpperCase();
      if (tag === "TITLE") align = "center";
      else                 align = tag.toLowerCase();
      text = m[2];
    }
    out.push(`<div class="poem-line poem-${align}">${escapeHtml(text)}</div>`);
    kept++;
    if (maxLines > 0 && kept >= maxLines) {
      truncated = lines.length > kept;
      break;
    }
  }
  const cls = condensed ? "poem-block poem-condensed" : "poem-block";
  return `<div class="${cls}">${out.join("")}${truncated ? `<div class="poem-more">…</div>` : ""}</div>`;
}

// Animated viewer for BitmapJpegAnim — the unpacker exports each frame
// as a separate .jpg, so we cycle them client-side at ~12 fps.  When
// the asset has an audio sidecar we drive playback from a single
// transport button that toggles both the frame timer AND the audio in
// lockstep.  Cleanup of the previous animation timer happens in
// `showDetail` before re-rendering body.
function renderJpegAnimPlayer(e) {
  const frames = e.anim.frames;
  const audio  = e.anim.audioFile;
  const pid    = `anim-${e.key}-${Math.random().toString(36).slice(2, 8)}`;
  queueMicrotask(() => bindJpegAnimPlayer(pid, frames, audio));
  return `
    <div class="anim-stack" data-player="${pid}">
      <img id="${pid}-img" class="anim-jpeg-frame"
           src="/api/file/${encodeURIComponent(frames[0])}" alt="frame 0" />
      <div class="anim-caption" id="${pid}-caption">
        <button type="button" id="${pid}-toggle" class="anim-toggle">&#9654; play</button>
        <span class="anim-frame-counter">frame <span class="cur">1</span> / ${frames.length}</span>
        ${audio ? `<span class="anim-time"><span class="cur-t">0:00</span> / <span class="dur-t">…</span></span>` : ""}
      </div>
      ${audio ? `<audio id="${pid}-audio" preload="metadata"
                          src="/api/file/${encodeURIComponent(audio)}"></audio>` : ""}
    </div>
  `;
}

function bindJpegAnimPlayer(pid, frames, audioFile) {
  // Cancel any previous detail panel's timer.
  if (state._animTimer) { clearInterval(state._animTimer); state._animTimer = null; }

  const img    = document.getElementById(`${pid}-img`);
  const curEl  = document.querySelector(`#${pid}-caption .cur`);
  const toggle = document.getElementById(`${pid}-toggle`);
  const audio  = audioFile ? document.getElementById(`${pid}-audio`) : null;
  const curT   = audioFile ? document.querySelector(`#${pid}-caption .cur-t`) : null;
  const durT   = audioFile ? document.querySelector(`#${pid}-caption .dur-t`) : null;
  if (!img || !toggle) return;

  const FPS = 12;
  let frameIdx = 0;
  let playing  = false;

  const showFrame = (i) => {
    frameIdx = ((i % frames.length) + frames.length) % frames.length;
    img.src = `/api/file/${encodeURIComponent(frames[frameIdx])}`;
    if (curEl) curEl.textContent = String(frameIdx + 1);
  };
  const tick = () => {
    if (!document.body.contains(img)) {
      clearInterval(state._animTimer); state._animTimer = null; return;
    }
    if (audio) {
      // Slave frames to audio playhead so seeks update visuals too.
      const t = audio.currentTime;
      showFrame(Math.floor(t * FPS));
      if (curT) curT.textContent = fmtClock(t);
    } else {
      showFrame(frameIdx + 1);
    }
  };
  const start = () => {
    if (playing) return;
    playing = true;
    if (audio) { audio.play().catch(() => { /* autoplay blocked etc. */ }); }
    state._animTimer = setInterval(tick, 1000 / FPS);
    toggle.innerHTML = "&#10073;&#10073; pause";
  };
  const stop = () => {
    if (!playing) return;
    playing = false;
    if (audio) audio.pause();
    if (state._animTimer) { clearInterval(state._animTimer); state._animTimer = null; }
    toggle.innerHTML = "&#9654; play";
  };

  toggle.addEventListener("click", () => playing ? stop() : start());

  if (audio) {
    audio.addEventListener("loadedmetadata", () => {
      if (durT) durT.textContent = fmtClock(audio.duration);
    });
    // If the audio reaches its end, stop the timer and reset transport.
    audio.addEventListener("ended", () => {
      stop();
      showFrame(0);
      if (curT) curT.textContent = "0:00";
    });
    // Cover the case where audio is paused/resumed via media keys.
    audio.addEventListener("play",  () => { if (!playing) start(); });
    audio.addEventListener("pause", () => { if (playing)  stop(); });
  }
}

function fmtClock(secs) {
  if (!Number.isFinite(secs)) return "0:00";
  const m = Math.floor(secs / 60);
  const s = Math.floor(secs - m * 60);
  return `${m}:${String(s).padStart(2, "0")}`;
}

function renderMetaSection(e) {
  const rows = [
    ["resource id", `${e.idHex} (${e.id})`],
    ["class",        `${e.className} (#${e.classId})`],
    ["size",         e.size ? `${e.size} bytes` : "—"],
  ];
  if (e.indexResourceIDHex) {
    rows.push(["index id", `${e.indexResourceIDHex} (${e.indexResourceID})  — absorbed AudioBankIndex`]);
  }
  if (e.audioBank) {
    rows.push(["samples", `${e.audioBank.sampleCount}`]);
    rows.push(["total",   `${e.audioBank.totalBankBytes} bytes`]);
  }
  return `
    <section>
      <h3>Metadata</h3>
      <div class="kv">
        ${rows.map(([k, v]) => `<span class="k">${k}</span><span class="v">${v}</span>`).join("")}
      </div>
      ${e.audioBank ? renderBankSamples(e.audioBank, e.key) : ""}
    </section>
  `;
}

function renderBankSamples(bank, bankKey) {
  return `
    <h3 style="margin-top: 14px;">Bank samples</h3>
    <div class="audio-sample-list">
      ${bank.samples.map(s =>
        `<div class="row">
          <span class="idx">#${s.index}</span>
          <span>@${s.offsetInBank}</span>
          <span>${s.byteLen} B</span>
          <audio controls preload="none" src="/api/preview/${bankKey}/sample/${s.index}"></audio>
        </div>`
      ).join("")}
    </div>
  `;
}

function renderCategoriesSection(e) {
  if (!e.categories || e.categories.length === 0) return "";
  return `
    <section>
      <h3>Categories</h3>
      <div class="cat-list">
        ${e.categories.map(c => `<span class="cat" data-cat="${c}">${c}</span>`).join("")}
      </div>
      ${e.tags && e.tags.length > 0
        ? `<div class="tag-list">${e.tags.map(t => `<span class="tag">${t}</span>`).join("")}</div>`
        : ""}
    </section>
  `;
}

function renderNotesSection(e) {
  if (e.notes) {
    return `
      <section>
        <h3>Notes</h3>
        <div class="notes-block">${escapeHtml(e.notes)}</div>
      </section>
    `;
  }
  return `
    <section>
      <h3>Notes</h3>
      <div class="notes-block empty">No notes yet.</div>
      <div class="notes-hint">
        Add to <code>ghidra_analysis/asset_catalog/registry.toml</code>:<br>
        <code>[asset."${e.key}"]</code><br>
        <code>slug = "your_slug"</code><br>
        <code>notes = "what / where / why"</code><br>
        then refresh the browser.
      </div>
    </section>
  `;
}

function renderXrefsSection(e) {
  const groups = [
    ["script",     "Script references"],
    ["insn",       "Instruction xrefs (Ghidra disassembly)"],
    ["data",       "Data xrefs"],
    ["ghidraText", "Ghidra decomp text"],
    ["idaText",    "IDA decomp text"],
    ["analysisMd", "Analysis markdown notes"],
  ];
  const out = [];
  let total = 0;
  for (const [k, label] of groups) {
    const arr = e.xrefs[k] || [];
    if (arr.length === 0) continue;
    total += arr.length;
    out.push(renderXrefGroup(label, arr, k));
  }
  if (total === 0) {
    out.push(`<div class="notes-block empty">No cross-references.</div>`);
  }
  return `<section><h3>Cross-references (${total})</h3>${out.join("")}</section>`;
}

function renderXrefGroup(label, items, kind) {
  // Cap each group's rendered count to keep panel performant.  Most
  // groups are small; only ghidraText/idaText occasionally explode.
  const CAP = 30;
  const truncated = items.length > CAP ? items.slice(0, CAP) : items;
  const rows = truncated.map(x => renderXrefRow(x, kind)).join("");
  const more = items.length > CAP
    ? `<div class="xref-row loc">... ${items.length - CAP} more (see catalog.md)</div>`
    : "";
  return `
    <div class="xref-group">
      <h4>${label}<span class="count">${items.length}</span></h4>
      ${rows}${more}
    </div>
  `;
}

function renderXrefRow(x, kind) {
  if (kind === "script") {
    return `<div class="xref-row">
      <span class="loc">${x.sourcePack}/${x.scriptClass} ${x.scriptId} @${x.offset}</span>
      <span class="snippet"> ${escapeHtml(x.snippet || "")}</span>
    </div>`;
  }
  const loc = [x.file, x.line && `:${x.line}`].filter(Boolean).join("");
  const fn = x.func ? `<span class="loc"> in ${escapeHtml(x.func)}</span>` : "";
  return `<div class="xref-row">
    <span class="loc">${escapeHtml(loc)}</span>${fn}
    <div class="snippet">${escapeHtml(x.snippet || "")}</div>
  </div>`;
}

function renderFilesSection(e) {
  if (!e.files || e.files.length === 0) return "";
  const rows = e.files.map(f => {
    const url = `/api/file/${encodeURIComponent(f)}`;
    return `<div class="xref-row"><a href="${url}" target="_blank">${escapeHtml(f)}</a></div>`;
  }).join("");
  return `
    <section>
      <h3>Source files (unpacked/overlay/)</h3>
      ${rows}
    </section>
  `;
}

// ---------------------------------------------------------------------------
// events
// ---------------------------------------------------------------------------
function bindEvents() {
  $("#detail-close").addEventListener("click", closeDetail);

  // Debounced search.
  let searchTimer = null;
  $("#search").addEventListener("input", (ev) => {
    clearTimeout(searchTimer);
    searchTimer = setTimeout(() => {
      state.filters.search = ev.target.value;
      applyFilters();
    }, 80);
  });

  document.addEventListener("keydown", (ev) => {
    if (ev.key === "Escape" && !$("#detail").classList.contains("hidden")) {
      closeDetail();
    }
  });
}

function closeDetail() {
  if (state._animTimer) { clearInterval(state._animTimer); state._animTimer = null; }
  $("#detail").classList.add("hidden");
  state.selected = null;
  // Surgical deselect, same reasoning as in showDetail().
  $$(".tile.selected").forEach(t => t.classList.remove("selected"));
}

// ---------------------------------------------------------------------------
// util
// ---------------------------------------------------------------------------
function escapeHtml(s) {
  return String(s || "").replace(/[&<>"']/g, c => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", "\"": "&quot;", "'": "&#39;"
  }[c]));
}
