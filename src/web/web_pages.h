#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

/* ====== HTML Dashboard Page ====== */
static const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>ESP32-S3 Env Dashboard</title>
<style>
:root { --bg:#0b0d12; --card:#121621; --border:#1b1f2a; --fg:#eef; --muted:#a9b2c2; --ok:#1dd17c; --warn:#ffb020; --hot:#ff5d5d; --blue:#54c8ff; }
body{background:var(--bg);color:var(--fg);font-family:system-ui,Segoe UI,Roboto;margin:0;padding:16px}
h1{margin:0 0 16px}
.grid{display:grid;grid-template-columns:1.1fr 0.9fr;gap:16px}
.card{background:var(--card);border:1px solid var(--border);border-radius:16px;padding:16px;margin-bottom:16px}
.badge{display:inline-flex;gap:6px;align-items:center;background:#0e121b;border:1px solid rgba(255,255,255,.05);border-radius:999px;padding:4px 10px;font-size:12px}
.section-title{font-size:13px;color:var(--muted);text-transform:uppercase;margin:16px 0 6px}
.hbar-wrap{background:#0e121b;border:1px solid #141824;border-radius:14px;height:28px;position:relative;overflow:hidden}
.hbar{height:100%;background:linear-gradient(90deg,#54c8ff 0%,#1dd17c 40%,#ffb020 70%,#ff5d5d 100%);width:0%;transition:width .25s ease-out}
.hbar-label{position:absolute;top:3px;left:8px;font-size:12px}
.band{font-weight:700}
.band.COLD{color:var(--blue)}
.band.NORMAL{color:var(--ok)}
.band.HOT{color:var(--warn)}
.band.CRITICAL{color:var(--hot)}
.flex{display:flex;gap:10px;flex-wrap:wrap}
input, select{width:100%;padding:6px 8px;border-radius:8px;border:1px solid #222;background:#0e1015;color:#fff;margin-bottom:6px}
button{background:#1dd17c;color:#000;border:none;border-radius:10px;padding:7px 16px;font-weight:600;cursor:pointer}
.btn-ghost{background:transparent;border:1px solid #1dd17c;color:#1dd17c}
.small{font-size:11px;color:var(--muted);margin-top:8px}
@media (max-width:960px){.grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<h1>ESP32-S3 Env Dashboard</h1>
<div class="grid">
  <!-- LEFT: live -->
  <div>
    <div class="card">
      <div class="flex">
        <span class="badge">Uptime: <span id="uptime">0.0s</span></span>
        <span class="badge">Temp: <span id="tband" class="band">-</span></span>
        <span class="badge">Hum: <span id="hband" class="band">-</span></span>
        <span class="badge">UI strip: <span id="uimode">OFF</span></span>
      </div>
      <h3 style="margin-top:14px">Humidity</h3>
      <div class="hbar-wrap">
        <div id="hbar" class="hbar"></div>
        <div id="hbar-label" class="hbar-label">- %</div>
      </div>
      <p class="small">GPIO 45 = auto humidity pixel. GPIO 6 = 4-px user bar.</p>
      <table style="width:100%;margin-top:10px">
        <tr><td>Temperature</td><td id="tempv" style="text-align:right">-</td></tr>
        <tr><td>Humidity</td><td id="humv" style="text-align:right">-</td></tr>
        <tr><td>LED</td><td id="ledv" style="text-align:right">-</td></tr>
        <tr><td>LED Blink</td><td id="blinkv" style="text-align:right">-</td></tr>
      </table>
    </div>
  </div>

  <!-- RIGHT: settings + wifi -->
  <div>
    <div class="card">
      <div class="section-title">TASK 1 — LED (TEMPERATURE)</div>
      <label>COLD max (°C)</label><input id="tcold" type="number" step="0.1" value="20.0" autocomplete="off">
      <label>NORMAL max (°C)</label><input id="tnorm" type="number" step="0.1" value="30.0" autocomplete="off">
      <label>HOT max (°C)</label><input id="thot" type="number" step="0.1" value="40.0" autocomplete="off">

      <div class="section-title">TASK 2 — HUMIDITY NEOPIXEL (GPIO 45)</div>
      <label>DRY max (%)</label><input id="hdry" type="number" step="0.1" value="40.0" autocomplete="off">
      <label>COMFORT max (%)</label><input id="hcomf" type="number" step="0.1" value="60.0" autocomplete="off">
      <label>HUMID max (%)</label><input id="hhum" type="number" step="0.1" value="80.0" autocomplete="off">

      <div class="section-title">TASK 2b — UI BAR (GPIO 6, 4 PX)</div>
      <div class="flex">
        <button id="btnUiOff" class="btn-ghost">OFF</button>
        <button id="btnUiBar" class="btn-ghost">BAR</button>
        <button id="btnUiDemo" class="btn-ghost">DEMO</button>
      </div>

      <div class="section-title">TASK 3 — LCD</div>
      <p class="small">LCD refreshes whenever the DHT task gives <code>semLcdUpdate</code>.</p>

      <button id="btnSave" style="margin-top:12px">Save thresholds</button>
      <p id="saveMsg" class="small"></p>
    </div>

    <div class="card">
      <div class="section-title">Wi-Fi Config</div>
      <label>Mode</label>
      <select id="wifiMode">
        <option value="ap">Access Point</option>
        <option value="sta">Station</option>
      </select>
      <label>SSID</label><input id="wifiSsid" placeholder="your-ssid" autocomplete="off">
      <label>Password</label><input id="wifiPass" placeholder="your-password" type="password" autocomplete="off">
      <button id="btnWifi" style="margin-top:8px">Apply Wi-Fi</button>
      <p id="wifiMsg" class="small"></p>
    </div>
  </div>
</div>

<script>
// track which input is being edited so poll() doesn't overwrite it
const fields = ["tcold","tnorm","thot","hdry","hcomf","hhum"];
const isEditing = {};
fields.forEach(id => {
  const el = document.getElementById(id);
  el.addEventListener("focus", () => { isEditing[id] = true; });
  el.addEventListener("blur",  () => { isEditing[id] = false; });
});

async function poll(){
  try{
    const r = await fetch('/state');
    const j = await r.json();

    document.getElementById('uptime').textContent = (j.ms/1000).toFixed(1)+'s';
    document.getElementById('tband').textContent = j.tBand;
    document.getElementById('tband').className  = 'band '+j.tBand;
    document.getElementById('hband').textContent = j.hBand;
    document.getElementById('hband').className  = 'band '+j.hBand;

    document.getElementById('tempv').textContent = isNaN(j.tC)?'-':j.tC.toFixed(1)+' °C';
    document.getElementById('humv').textContent  = isNaN(j.rh)?'-':j.rh.toFixed(1)+' %';
    document.getElementById('ledv').textContent  = j.led? 'ON':'OFF';
    document.getElementById('blinkv').textContent = j.blink_on+'/'+j.blink_off+' ms';

    const hv = Math.min(100, Math.max(0, j.rh));
    document.getElementById('hbar').style.width = hv + '%';
    document.getElementById('hbar-label').textContent = hv.toFixed(1)+' %';

    // only update if user not editing
    if (!isEditing["tcold"]) document.getElementById('tcold').value = j.tcold;
    if (!isEditing["tnorm"]) document.getElementById('tnorm').value = j.tnorm;
    if (!isEditing["thot"])  document.getElementById('thot').value  = j.thot;
    if (!isEditing["hdry"])  document.getElementById('hdry').value  = j.hdry;
    if (!isEditing["hcomf"]) document.getElementById('hcomf').value = j.hcomf;
    if (!isEditing["hhum"])  document.getElementById('hhum').value  = j.hhum;

    document.getElementById('uimode').textContent =
      (j.uiMode==0?'OFF':(j.uiMode==1?'BAR':'DEMO'));

    // wifi mode (show current)
    const wm = document.getElementById('wifiMode');
    if (wm && (j.wifiMode === "ap" || j.wifiMode === "sta")) {
      wm.value = j.wifiMode;
    }

  }catch(e){
    // ignore
  }
}
setInterval(poll, 500);
poll();

document.getElementById('btnSave').addEventListener('click', async ()=>{
  const tc = parseFloat(document.getElementById('tcold').value);
  const tn = parseFloat(document.getElementById('tnorm').value);
  const th = parseFloat(document.getElementById('thot').value);
  const hd = parseFloat(document.getElementById('hdry').value);
  const hc = parseFloat(document.getElementById('hcomf').value);
  const hh = parseFloat(document.getElementById('hhum').value);
  const msgEl = document.getElementById('saveMsg');

  if ([tc,tn,th,hd,hc,hh].some(x => isNaN(x))) {
    msgEl.textContent = 'Please enter valid numbers.';
    return;
  }
  if (!(tc < tn && tn < th)) {
    msgEl.textContent = 'Temp: COLD < NORMAL < HOT';
    return;
  }
  if (!(hd < hc && hc < hh)) {
    msgEl.textContent = 'Humidity: DRY < COMFORT < HUMID';
    return;
  }

  const qs = `/set?tcold=${tc}&tnorm=${tn}&thot=${th}&hdry=${hd}&hcomf=${hc}&hhum=${hh}`;
  const resp = await fetch(qs);
  msgEl.textContent = await resp.text();
  fields.forEach(id => isEditing[id] = false);
});

// ui buttons
document.getElementById('btnUiOff').addEventListener('click', async ()=>{
  const r = await fetch('/ui/off'); document.getElementById('saveMsg').textContent = await r.text();
});
document.getElementById('btnUiBar').addEventListener('click', async ()=>{
  const r = await fetch('/ui/bar'); document.getElementById('saveMsg').textContent = await r.text();
});
document.getElementById('btnUiDemo').addEventListener('click', async ()=>{
  const r = await fetch('/ui/demo'); document.getElementById('saveMsg').textContent = await r.text();
});

// wifi config
document.getElementById('btnWifi').addEventListener('click', async ()=>{
  const mode = document.getElementById('wifiMode').value;
  const ssid = encodeURIComponent(document.getElementById('wifiSsid').value.trim());
  const pass = encodeURIComponent(document.getElementById('wifiPass').value.trim());
  const url = `/wifi?mode=${mode}&ssid=${ssid}&pass=${pass}`;
  const r = await fetch(url);
  document.getElementById('wifiMsg').textContent = await r.text();
});
</script>
</body>
</html>
)HTML";

#endif // WEB_PAGES_H
