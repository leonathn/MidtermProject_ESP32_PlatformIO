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
<title>ESP32-S3 RTOS Lab</title>
<style>
* { margin:0; padding:0; box-sizing:border-box; }
:root { 
  --bg:#0f172a; 
  --card:#1e293b; 
  --card-dark:#0f1729;
  --border:#1e3a5f; 
  --fg:#f1f5f9; 
  --muted:#94a3b8; 
  --cyan:#06b6d4;
  --green:#10b981;
  --red:#ef4444;
  --yellow:#f59e0b;
}
body { background:var(--bg); color:var(--fg); font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif; }

/* Header */
.header { 
  background:rgba(15,23,41,0.95); 
  backdrop-filter:blur(10px);
  border-bottom:1px solid var(--border); 
  padding:12px 20px;
  position:sticky;
  top:0;
  z-index:100;
}
.header-top { display:flex; justify-content:space-between; align-items:center; margin-bottom:8px; }
.header-title { display:flex; align-items:center; gap:10px; font-size:22px; font-weight:700; color:var(--cyan); }
.chip-icon { font-size:24px; }
.status-badges { display:flex; gap:6px; flex-wrap:wrap; }
.badge { 
  display:inline-flex; 
  align-items:center; 
  gap:4px; 
  padding:6px 12px; 
  border-radius:999px; 
  font-size:12px; 
  font-weight:600;
  border:2px solid;
}
.badge-success { background:rgba(16,185,129,0.15); border-color:var(--green); color:var(--green); }
.badge-warning { background:rgba(239,68,68,0.15); border-color:var(--red); color:var(--red); }
.header-info { display:flex; gap:12px; font-size:12px; color:var(--muted); align-items:center; flex-wrap:wrap; }

/* Tabs */
.tabs { display:flex; gap:6px; margin-bottom:16px; }
.tab { 
  background:transparent; 
  border:none; 
  color:var(--muted); 
  padding:10px 24px; 
  border-radius:10px; 
  font-size:14px; 
  font-weight:600;
  cursor:pointer;
  transition:all 0.2s;
}
.tab:hover { background:rgba(6,182,212,0.1); color:var(--cyan); }
.tab.active { background:var(--card-dark); color:var(--cyan); }

/* Main Content */
.container { max-width:1400px; margin:0 auto; padding:16px; }
.tab-content { display:none; }
.tab-content.active { display:block; }

/* Cards */
.card { background:var(--card); border:1px solid var(--border); border-radius:16px; padding:20px; margin-bottom:16px; }
.card-title { display:flex; align-items:center; gap:8px; font-size:16px; font-weight:600; margin-bottom:16px; color:var(--fg); }

/* Live Sensors */
.sensor-grid { display:grid; gap:16px; }
.sensor-block { margin-bottom:0; }
.sensor-value { font-size:48px; font-weight:700; display:flex; align-items:baseline; gap:8px; margin:8px 0; }
.sensor-icon { font-size:32px; }
.sensor-unit { font-size:24px; color:var(--muted); }
.sensor-band { font-size:14px; color:var(--muted); margin-bottom:8px; }
.sensor-band span { font-weight:700; }
.progress-bar { 
  background:var(--card-dark); 
  height:12px; 
  border-radius:999px; 
  overflow:hidden;
  position:relative;
}
.progress-fill { 
  height:100%; 
  background:linear-gradient(90deg, var(--cyan), var(--green)); 
  border-radius:999px; 
  transition:width 0.3s;
}

/* Config */
.config-grid { display:grid; grid-template-columns:1fr 1fr; gap:20px; }
.config-section { }
.config-section h3 { font-size:16px; margin-bottom:12px; display:flex; align-items:center; gap:8px; }
label { display:block; font-size:11px; color:var(--muted); margin-top:10px; margin-bottom:3px; text-transform:uppercase; letter-spacing:0.5px; }
input, select { 
  width:100%; 
  padding:10px 12px; 
  background:var(--card-dark); 
  border:1px solid var(--border); 
  border-radius:10px; 
  color:var(--fg); 
  font-size:14px;
}
input:focus, select:focus { outline:none; border-color:var(--cyan); }
.hint { font-size:11px; color:var(--muted); font-style:italic; margin-top:6px; }
button { 
  background:var(--cyan); 
  color:#0f172a; 
  border:none; 
  padding:10px 20px; 
  border-radius:10px; 
  font-weight:700; 
  font-size:14px;
  cursor:pointer;
  transition:all 0.2s;
  margin-top:12px;
}
button:hover { transform:translateY(-1px); box-shadow:0 4px 12px rgba(6,182,212,0.4); }
.btn-group { display:flex; gap:10px; margin-top:12px; }
.btn-outline { background:transparent; border:2px solid var(--cyan); color:var(--cyan); padding:8px 16px; }
.btn-outline.active { background:var(--cyan); color:#0f172a; }

/* Controls */
.control-section { margin-bottom:20px; }
.led-preview { display:flex; gap:12px; margin-top:12px; align-items:center; }
.led { width:32px; height:32px; border-radius:50%; background:var(--card-dark); border:2px solid var(--border); }

/* Task Monitor */
.task-list { display:grid; gap:8px; }
.task-item { 
  background:var(--card-dark); 
  padding:12px; 
  border-radius:10px; 
  display:flex; 
  justify-content:space-between; 
  align-items:center;
}
.task-info h4 { font-size:14px; font-weight:600; margin-bottom:3px; }
.task-info .small { font-size:11px; color:var(--muted); }
.task-status { text-align:right; }
.status-badge { 
  display:inline-block; 
  padding:4px 10px; 
  border-radius:6px; 
  font-size:10px; 
  font-weight:600;
  text-transform:uppercase;
}
.status-running { background:var(--green); color:#0f172a; }
.status-waiting { background:var(--yellow); color:#0f172a; }

.small { font-size:11px; color:var(--muted); }
@media (max-width:960px) { 
  .config-grid { grid-template-columns:1fr; }
  .header-top { flex-direction:column; align-items:flex-start; gap:8px; }
  .sensor-value { font-size:36px; }
  .sensor-icon { font-size:28px; }
  .sensor-unit { font-size:20px; }
}
@media (max-width:640px) {
  .card { padding:16px; }
  .container { padding:12px; }
  .header { padding:10px 12px; }
  .sensor-value { font-size:32px; }
}
</style>
</head>
<body>

<!-- Header -->
<div class="header">
  <div class="header-top">
    <div class="header-title">
      <span class="chip-icon">💠</span>
      ESP32-S3 RTOS Lab
    </div>
    <div class="status-badges">
      <div class="badge badge-success" id="tempBadge">
        🌡️ <span id="tempBadgeText">NORMAL</span>
      </div>
      <div class="badge badge-success" id="humBadge">
        💧 <span id="humBadgeText">COMFORT</span>
      </div>
    </div>
  </div>
  <div class="header-info">
    <span>📡 <strong>AP:</strong> <span id="apName">ESP32-S3-LAB</span></span>
    <span>🌐 <strong>IP:</strong> <span id="ipAddr">192.168.4.1</span></span>
    <span>⏱️ <strong>Uptime:</strong> <span id="uptime">0s</span></span>
  </div>
</div>

<!-- Tabs -->
<div class="container">
  <div class="tabs">
    <button class="tab active" onclick="showTab('dashboard')">Dashboard</button>
    <button class="tab" onclick="showTab('configuration')">Configuration</button>
    <button class="tab" onclick="showTab('controls')">Controls</button>
    <button class="tab" onclick="showTab('events')">Events</button>
  </div>

  <!-- Dashboard Tab -->
  <div id="dashboard" class="tab-content active">
    <div class="card">
      <div class="card-title">📊 Live Sensors</div>
      <div style="display:grid; grid-template-columns:1fr 1fr; gap:16px;">
        
        <!-- Temperature -->
        <div class="sensor-block">
          <div style="display:flex; align-items:center; gap:10px;">
            <span class="sensor-icon">🌡️</span>
            <div style="flex:1;">
              <div class="sensor-value">
                <span id="tempValue">--</span>
                <span class="sensor-unit">°C</span>
              </div>
              <div class="sensor-band">Band: <span id="tband">-</span></div>
              <div class="progress-bar">
                <div id="tempProgress" class="progress-fill" style="width:0%"></div>
              </div>
            </div>
          </div>
        </div>

        <!-- Humidity -->
        <div class="sensor-block">
          <div style="display:flex; align-items:center; gap:10px;">
            <span class="sensor-icon">💧</span>
            <div style="flex:1;">
              <div class="sensor-value">
                <span id="humValue">--</span>
                <span class="sensor-unit">%</span>
              </div>
              <div class="sensor-band">Band: <span id="hband">-</span></div>
              <div class="progress-bar">
                <div id="humProgress" class="progress-fill" style="width:0%"></div>
              </div>
            </div>
          </div>
        </div>

      </div>
    </div>

    <!-- Task Monitor -->
    <div class="card">
      <div class="card-title">⚙️ Task Monitor</div>
      <div class="task-list">
        <div class="task-item">
          <div class="task-info">
            <h4>Task 1: DHT20 Sensor</h4>
            <div class="small">Producer (Core 1, P:3)</div>
          </div>
          <div class="task-status">
            <span id="task1status" class="status-badge status-running">RUNNING</span>
            <div class="small" style="margin-top:4px">Runs: <span id="task1runs">0</span></div>
          </div>
        </div>
        
        <div class="task-item">
          <div class="task-info">
            <h4>Task 2: LED Control</h4>
            <div class="small">Consumer (Core 0, P:2) ← semBandChanged</div>
          </div>
          <div class="task-status">
            <span id="task2status" class="status-badge status-waiting">WAITING</span>
            <div class="small" style="margin-top:4px">Takes: <span id="task2takes">0</span> | Runs: <span id="task2runs">0</span></div>
          </div>
        </div>
        
        <div class="task-item">
          <div class="task-info">
            <h4>Task 3: NeoPixel Humidity</h4>
            <div class="small">Consumer (Core 0, P:2) ← semHumChanged</div>
          </div>
          <div class="task-status">
            <span id="task3status" class="status-badge status-waiting">WAITING</span>
            <div class="small" style="margin-top:4px">Takes: <span id="task3takes">0</span> | Runs: <span id="task3runs">0</span></div>
          </div>
        </div>
        
        <div class="task-item">
          <div class="task-info">
            <h4>Task 5: LCD Display</h4>
            <div class="small">Consumer (Core 0, P:1) ← semLcdUpdate</div>
          </div>
          <div class="task-status">
            <span id="task5status" class="status-badge status-waiting">WAITING</span>
            <div class="small" style="margin-top:4px">Runs: <span id="task5runs">0</span></div>
          </div>
        </div>
      </div>
    </div>

    <!-- TinyML -->
    <div class="card">
      <div class="card-title">🤖 TinyML Anomaly Detection</div>
      <div class="sensor-band">Anomaly Score: <span id="tinyScore">-</span></div>
      <div class="progress-bar" style="margin:16px 0">
        <div id="tinyProgress" class="progress-fill" style="width:0%; background:linear-gradient(90deg, var(--green), var(--yellow), var(--red))"></div>
      </div>
      <div class="small">Status: <strong id="tinyStatus">Waiting...</strong> | Runs: <strong id="tinyRuns">0</strong></div>
    </div>
  </div>

  <!-- Configuration Tab -->
  <div id="configuration" class="tab-content">
    <div class="card">
      <div class="card-title">🌡️ Thresholds Configuration</div>
      <div class="config-grid">
        <div class="config-section">
          <h3>🌡️ Task 1 — LED / Temperature</h3>
          <label>COLD max (°C)</label>
          <input id="tcold" type="number" step="0.1" value="20.0" autocomplete="off">
          
          <label>NORMAL max (°C)</label>
          <input id="tnorm" type="number" step="0.1" value="30.0" autocomplete="off">
          
          <label>HOT max (°C)</label>
          <input id="thot" type="number" step="0.1" value="40.0" autocomplete="off">
          
          <div class="hint">CRITICAL is ≥ HOT max</div>
        </div>

        <div class="config-section">
          <h3>💧 Task 2 — Humidity / NeoPixel 45</h3>
          <label>DRY max (%)</label>
          <input id="hdry" type="number" step="0.1" value="40.0" autocomplete="off">
          
          <label>COMFORT max (%)</label>
          <input id="hcomf" type="number" step="0.1" value="60.0" autocomplete="off">
          
          <label>HUMID max (%)</label>
          <input id="hhum" type="number" step="0.1" value="80.0" autocomplete="off">
          
          <div class="hint">WET is ≥ HUMID max</div>
        </div>
      </div>
      
      <button id="btnSave">💾 Save Thresholds</button>
      <div class="small" id="saveMsg" style="margin-top:8px"></div>
    </div>

    <!-- WiFi Config -->
    <div class="card">
      <div class="card-title">📡 WiFi Configuration</div>
      <div class="config-grid">
        <div>
          <label>Mode</label>
          <select id="wifiMode">
            <option value="ap">Access Point</option>
            <option value="sta">Station</option>
          </select>
        </div>
        <div>
          <label>SSID</label>
          <input id="wifiSsid" placeholder="your-ssid" autocomplete="off">
        </div>
        <div style="grid-column:1/-1">
          <label>Password</label>
          <input id="wifiPass" placeholder="your-password" type="password" autocomplete="off">
        </div>
      </div>
      <button id="btnWifi">🌐 Apply WiFi Settings</button>
      <div class="small" id="wifiMsg" style="margin-top:8px"></div>
    </div>
  </div>

  <!-- Controls Tab -->
  <div id="controls" class="tab-content">
    <div class="card">
      <div class="card-title">💡 UI NeoPixel (GPIO 6)</div>
      <div class="btn-group">
        <button id="btnUiOff" class="btn-outline">OFF</button>
        <button id="btnUiBar" class="btn-outline">BAR</button>
        <button id="btnUiDemo" class="btn-outline">DEMO</button>
      </div>
      <div class="small" style="margin-top:12px">4-LED Preview:</div>
      <div class="led-preview">
        <div class="led" id="led0"></div>
        <div class="led" id="led1"></div>
        <div class="led" id="led2"></div>
        <div class="led" id="led3"></div>
      </div>
    </div>
  </div>

  <!-- Events Tab -->
  <div id="events" class="tab-content">
    <div class="card">
      <div class="card-title">📋 System Events</div>
      <div class="small">Event log and diagnostics will appear here...</div>
    </div>
  </div>

</div>

<script>
// Tab switching
function showTab(tabName) {
  document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
  document.getElementById(tabName).classList.add('active');
  event.target.classList.add('active');
}

// Track which input is being edited
const fields = ["tcold","tnorm","thot","hdry","hcomf","hhum"];
const isEditing = {};
const userValues = {};
fields.forEach(id => {
  const el = document.getElementById(id);
  el.addEventListener("focus", () => { isEditing[id] = true; });
  el.addEventListener("blur",  () => { 
    userValues[id] = el.value;
    setTimeout(() => { isEditing[id] = false; }, 100);
  });
  el.addEventListener("input", () => { userValues[id] = el.value; });
});

async function poll(){
  try{
    const r = await fetch('/state');
    const j = await r.json();

    // Update header badges
    document.getElementById('tempBadgeText').textContent = j.tBand;
    const tempBadge = document.getElementById('tempBadge');
    if (j.tBand === 'CRITICAL' || j.tBand === 'HOT') {
      tempBadge.className = 'badge badge-warning';
    } else {
      tempBadge.className = 'badge badge-success';
    }

    document.getElementById('humBadgeText').textContent = j.hBand;
    const humBadge = document.getElementById('humBadge');
    if (j.hBand === 'WET' || j.hBand === 'DRY') {
      humBadge.className = 'badge badge-warning';
    } else {
      humBadge.className = 'badge badge-success';
    }

    // Update uptime
    const secs = Math.floor(j.ms / 1000);
    document.getElementById('uptime').textContent = secs < 60 ? secs+'s' : 
      secs < 3600 ? Math.floor(secs/60)+'m '+(secs%60)+'s' :
      Math.floor(secs/3600)+'h '+Math.floor((secs%3600)/60)+'m';

    // Update sensor values
    document.getElementById('tempValue').textContent = isNaN(j.tC) ? '--' : j.tC.toFixed(1);
    document.getElementById('humValue').textContent = isNaN(j.rh) ? '--' : j.rh.toFixed(1);
    document.getElementById('tband').textContent = j.tBand;
    document.getElementById('hband').textContent = j.hBand;

    // Update progress bars
    const tempPercent = Math.min(100, Math.max(0, (j.tC / 50) * 100));
    document.getElementById('tempProgress').style.width = tempPercent + '%';
    
    const humPercent = Math.min(100, Math.max(0, j.rh));
    document.getElementById('humProgress').style.width = humPercent + '%';

    // Update config inputs (only if not editing)
    if (!isEditing["tcold"] && !userValues["tcold"]) document.getElementById('tcold').value = j.tcold;
    if (!isEditing["tnorm"] && !userValues["tnorm"]) document.getElementById('tnorm').value = j.tnorm;
    if (!isEditing["thot"] && !userValues["thot"]) document.getElementById('thot').value  = j.thot;
    if (!isEditing["hdry"] && !userValues["hdry"]) document.getElementById('hdry').value  = j.hdry;
    if (!isEditing["hcomf"] && !userValues["hcomf"]) document.getElementById('hcomf').value = j.hcomf;
    if (!isEditing["hhum"] && !userValues["hhum"]) document.getElementById('hhum').value  = j.hhum;

    // Update UI mode buttons
    document.querySelectorAll('#btnUiOff, #btnUiBar, #btnUiDemo').forEach(b => b.classList.remove('active'));
    if (j.uiMode === 0) document.getElementById('btnUiOff').classList.add('active');
    else if (j.uiMode === 1) document.getElementById('btnUiBar').classList.add('active');
    else document.getElementById('btnUiDemo').classList.add('active');

    // WiFi mode
    const wm = document.getElementById('wifiMode');
    if (wm && (j.wifiMode === "ap" || j.wifiMode === "sta")) {
      wm.value = j.wifiMode;
    }

    // Update task monitor
    if (document.getElementById('task1runs')) {
      document.getElementById('task1runs').textContent = j.dht_runs || 0;
      document.getElementById('task2runs').textContent = j.led_runs || 0;
      document.getElementById('task3runs').textContent = j.neo_runs || 0;
      document.getElementById('task5runs').textContent = j.lcd_runs || 0;
      
      document.getElementById('task2takes').textContent = j.takeTemp || 0;
      document.getElementById('task3takes').textContent = j.takeHum || 0;
      
      // Update task status badges
      const now = j.ms;
      const updateStatus = (id, lastMs, runs) => {
        const el = document.getElementById(id);
        if (!el) return;
        const delta = now - lastMs;
        if (runs > 0 && delta < 2000) {
          el.textContent = 'RUNNING';
          el.className = 'status-badge status-running';
        } else if (runs > 0) {
          el.textContent = 'WAITING';
          el.className = 'status-badge status-waiting';
        } else {
          el.textContent = 'IDLE';
          el.className = 'status-badge';
          el.style.background = '#6c757d';
          el.style.color = '#fff';
        }
      };
      
      updateStatus('task1status', j.dht_last_ms, j.dht_runs);
      updateStatus('task2status', j.led_last_ms, j.led_runs);
      updateStatus('task3status', j.neo_last_ms, j.neo_runs);
      updateStatus('task5status', j.lcd_last_ms, j.lcd_runs);
    }

    // Update TinyML
    const score = j.tiny_score;
    if (document.getElementById('tinyProgress')) {
      const scorePercent = Math.min(100, Math.max(0, score * 100));
      document.getElementById('tinyProgress').style.width = scorePercent + '%';
      document.getElementById('tinyScore').textContent = isNaN(score) ? '-' : score.toFixed(3);
      document.getElementById('tinyRuns').textContent = j.tinyml_runs || 0;
      
      const statusEl = document.getElementById('tinyStatus');
      if (isNaN(score)) {
        statusEl.textContent = 'Waiting...';
      } else if (score < 0.3) {
        statusEl.textContent = 'Normal';
      } else if (score < 0.7) {
        statusEl.textContent = 'Unusual';
      } else {
        statusEl.textContent = 'Anomaly!';
      }
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
  
  // Clear user values and editing flags after successful save
  fields.forEach(id => {
    isEditing[id] = false;
    userValues[id] = null;
  });
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
