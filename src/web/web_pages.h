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
.gauge-wrap{background:#0e121b;border:1px solid #141824;border-radius:18px;height:36px;position:relative;overflow:hidden;margin:12px 0}
.gauge-fill{height:100%;background:linear-gradient(90deg,#1dd17c 0%,#ffb020 55%,#ff5d5d 100%);width:0%;transition:width .3s ease-out}
.gauge-label{position:absolute;top:8px;left:12px;font-size:14px;font-weight:600}
.status-normal{color:var(--ok);font-weight:600}
.status-unusual{color:var(--hot);font-weight:600}
.status-wait{color:var(--muted);font-weight:600}
.flex{display:flex;gap:10px;flex-wrap:wrap}
input, select{width:100%;padding:6px 8px;border-radius:8px;border:1px solid #222;background:#0e1015;color:#fff;margin-bottom:6px}
button{background:#1dd17c;color:#000;border:none;border-radius:10px;padding:7px 16px;font-weight:600;cursor:pointer}
.btn-ghost{background:transparent;border:1px solid #1dd17c;color:#1dd17c}
.small{font-size:11px;color:var(--muted);margin-top:8px}
@keyframes pulse{0%,100%{border-color:var(--hot);box-shadow:0 0 0 rgba(255,93,93,0)}50%{border-color:var(--hot);box-shadow:0 0 20px rgba(255,93,93,0.5)}}
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
    
    <!-- Task Semaphore Visualization -->
    <div class="card">
      <div class="section-title">⚙️ TASK SEMAPHORE MONITOR</div>
      <div style="margin-bottom:10px">
        <div style="display:flex;justify-content:space-between;align-items:center;padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <div>
            <strong>Task 1: DHT20 Sensor</strong>
            <div class="small" style="margin-top:2px">Producer (Core 1, P:3)</div>
          </div>
          <div style="text-align:right">
            <span id="task1status" class="badge" style="background:#28a745">RUNNING</span>
            <div class="small" style="margin-top:2px">Runs: <span id="task1runs">0</span></div>
          </div>
        </div>
        
        <div style="display:flex;justify-content:space-between;align-items:center;padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <div>
            <strong>Task 2: LED Control</strong>
            <div class="small" style="margin-top:2px">Consumer (Core 0, P:2) ← semBandChanged</div>
          </div>
          <div style="text-align:right">
            <span id="task2status" class="badge" style="background:#ffc107;color:#000">WAITING</span>
            <div class="small" style="margin-top:2px">Takes: <span id="task2takes">0</span> | Runs: <span id="task2runs">0</span></div>
          </div>
        </div>
        
        <div style="display:flex;justify-content:space-between;align-items:center;padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <div>
            <strong>Task 3: NeoPixel Hum</strong>
            <div class="small" style="margin-top:2px">Consumer (Core 0, P:2) ← semHumChanged</div>
          </div>
          <div style="text-align:right">
            <span id="task3status" class="badge" style="background:#ffc107;color:#000">WAITING</span>
            <div class="small" style="margin-top:2px">Takes: <span id="task3takes">0</span> | Runs: <span id="task3runs">0</span></div>
          </div>
        </div>
        
        <div style="display:flex;justify-content:space-between;align-items:center;padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <div>
            <strong>Task 5: LCD Display</strong>
            <div class="small" style="margin-top:2px">Consumer (Core 0, P:1) ← semLcdUpdate</div>
          </div>
          <div style="text-align:right">
            <span id="task5status" class="badge" style="background:#ffc107;color:#000">WAITING</span>
            <div class="small" style="margin-top:2px">Runs: <span id="task5runs">0</span></div>
          </div>
        </div>
      </div>
      
      <div style="background:#0e121b;padding:10px;border-radius:6px">
        <strong>Semaphore Events:</strong>
        <div style="display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:8px">
          <div>
            <div class="small">semBandChanged (Temp)</div>
            <div style="font-size:1.2em;font-weight:bold;color:var(--ok)">
              <span id="semTempGives">0</span> → <span id="semTempTakes">0</span>
            </div>
          </div>
          <div>
            <div class="small">semHumChanged (Hum)</div>
            <div style="font-size:1.2em;font-weight:bold;color:var(--blue)">
              <span id="semHumGives">0</span> → <span id="semHumTakes">0</span>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="card">
      <div class="section-title">TASK 5 — TinyML Anomaly Score</div>
      <div class="gauge-wrap">
        <div id="tinyGauge" class="gauge-fill"></div>
        <div id="tinyGaugeLabel" class="gauge-label">-</div>
      </div>
      <p class="small">Inference score updates when the TinyML task acquires <code>gLive</code> sensor data &amp; runs inference. 0 = normal, 1 = unusual.</p>
      <div class="flex">
        <span class="badge">Status: <span id="tinyStatus" class="status-wait">Waiting...</span></span>
        <span class="badge">Runs: <span id="tinyRuns">0</span></span>
        <span class="badge">Last: <span id="tinyAgo">-</span></span>
      </div>
    </div>
    
    <!-- System Health Dashboard -->
    <div class="card">
      <div class="section-title">🖥️ SYSTEM HEALTH</div>
      <div style="display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:10px">
        <div style="background:#0e121b;padding:10px;border-radius:8px">
          <div class="small" style="color:var(--muted)">Free Heap</div>
          <div style="font-size:1.4em;font-weight:700;color:var(--ok)">
            <span id="freeHeap">-</span> KB
          </div>
          <div class="small" style="margin-top:4px">Min: <span id="minFreeHeap">-</span> KB</div>
        </div>
        <div style="background:#0e121b;padding:10px;border-radius:8px">
          <div class="small" style="color:var(--muted)">WiFi RSSI</div>
          <div style="font-size:1.4em;font-weight:700" id="rssiColor">
            <span id="wifiRSSI">-</span> dBm
          </div>
          <div class="small" style="margin-top:4px" id="rssiQuality">-</div>
        </div>
      </div>
      <div style="background:#0e121b;padding:10px;border-radius:8px;margin-top:10px">
        <div class="small" style="color:var(--muted)">System Uptime</div>
        <div style="font-size:1.4em;font-weight:700;color:var(--blue)" id="uptime">-</div>
      </div>
    </div>
    
    <!-- Historical Data Chart -->
    <div class="card">
      <div class="section-title">📊 HISTORICAL DATA (Last 50 readings)</div>
      <canvas id="historyChart" width="100%" height="200" style="max-height:200px"></canvas>
      <p class="small" style="margin-top:8px">Blue: Temperature (°C) | Orange: Humidity (%)</p>
    </div>
    
    <!-- Alert System -->
    <div class="card" id="alertCard" style="border:2px solid var(--border)">
      <div style="display:flex;justify-content:space-between;align-items:center">
        <div class="section-title">🔔 ALERT SYSTEM</div>
        <button id="btnAlertToggle" class="btn-ghost" style="padding:4px 12px;font-size:12px">ON</button>
      </div>
      <div id="alertStatus" style="margin-top:10px">
        <div style="padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <span class="badge" id="alertTemp" style="background:#28a745">✓ Temperature OK</span>
        </div>
        <div style="padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <span class="badge" id="alertHum" style="background:#28a745">✓ Humidity OK</span>
        </div>
        <div style="padding:8px;background:#0e121b;border-radius:6px;margin-bottom:6px">
          <span class="badge" id="alertAnomaly" style="background:#28a745">✓ No Anomaly</span>
        </div>
      </div>
      <div style="margin-top:10px;padding:8px;background:#0e121b;border-radius:6px">
        <div class="small">Alert Counter: <strong><span id="alertCount">0</span></strong></div>
        <div class="small">Last Alert: <span id="lastAlert">-</span></div>
      </div>
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
const userValues = {};  // Store user-entered values
fields.forEach(id => {
  const el = document.getElementById(id);
  el.addEventListener("focus", () => { 
    isEditing[id] = true; 
  });
  el.addEventListener("blur",  () => { 
    userValues[id] = el.value;  // Save user value on blur
    setTimeout(() => { isEditing[id] = false; }, 100);  // Delay to prevent immediate overwrite
  });
  el.addEventListener("input", () => {
    userValues[id] = el.value;  // Save as user types
  });
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

    // only update if user not editing AND no user value stored
    if (!isEditing["tcold"] && !userValues["tcold"]) document.getElementById('tcold').value = j.tcold;
    if (!isEditing["tnorm"] && !userValues["tnorm"]) document.getElementById('tnorm').value = j.tnorm;
    if (!isEditing["thot"] && !userValues["thot"]) document.getElementById('thot').value  = j.thot;
    if (!isEditing["hdry"] && !userValues["hdry"]) document.getElementById('hdry').value  = j.hdry;
    if (!isEditing["hcomf"] && !userValues["hcomf"]) document.getElementById('hcomf').value = j.hcomf;
    if (!isEditing["hhum"] && !userValues["hhum"]) document.getElementById('hhum').value  = j.hhum;

    document.getElementById('uimode').textContent =
      (j.uiMode==0?'OFF':(j.uiMode==1?'BAR':'DEMO'));

    // wifi mode (show current)
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
      
      document.getElementById('semTempGives').textContent = j.giveTemp || 0;
      document.getElementById('semTempTakes').textContent = j.takeTemp || 0;
      document.getElementById('semHumGives').textContent = j.giveHum || 0;
      document.getElementById('semHumTakes').textContent = j.takeHum || 0;
      
      // Update task status badges based on activity
      const now = j.ms;
      const updateStatus = (id, lastMs, runs) => {
        const el = document.getElementById(id);
        if (!el) return;
        const delta = now - lastMs;
        if (runs > 0 && delta < 2000) {
          el.textContent = 'RUNNING';
          el.style.background = '#28a745';
          el.style.color = '#fff';
        } else if (runs > 0) {
          el.textContent = 'WAITING';
          el.style.background = '#ffc107';
          el.style.color = '#000';
        } else {
          el.textContent = 'IDLE';
          el.style.background = '#6c757d';
          el.style.color = '#fff';
        }
      };
      
      updateStatus('task1status', j.dht_last_ms, j.dht_runs);
      updateStatus('task2status', j.led_last_ms, j.led_runs);
      updateStatus('task3status', j.neo_last_ms, j.neo_runs);
      updateStatus('task5status', j.lcd_last_ms, j.lcd_runs);
    }

    const gauge = document.getElementById('tinyGauge');
    if (gauge) {
      const score = j.tiny_score;
      const gaugeLabel = document.getElementById('tinyGaugeLabel');
      const statusEl = document.getElementById('tinyStatus');
      const runsEl = document.getElementById('tinyRuns');
      const agoEl = document.getElementById('tinyAgo');

      const valid = typeof score === 'number' && !isNaN(score);
      if (valid) {
        const pct = Math.min(100, Math.max(0, score * 100));
        gauge.style.width = pct + '%';
        gaugeLabel.textContent = score.toFixed(3);
        const status = score >= 0.6 ? 'Unusual' : 'Normal';
        statusEl.textContent = status;
        statusEl.className = status === 'Unusual' ? 'status-unusual' : 'status-normal';
      } else {
        gauge.style.width = '0%';
        gaugeLabel.textContent = '-';
        statusEl.textContent = 'Waiting...';
        statusEl.className = 'status-wait';
      }

      runsEl.textContent = (typeof j.tiny_runs === 'number') ? j.tiny_runs : 0;
      if (j.tiny_last_ms && j.tiny_last_ms > 0) {
        const delta = Math.max(0, (j.ms - j.tiny_last_ms) / 1000);
        agoEl.textContent = delta.toFixed(1) + 's ago';
      } else {
        agoEl.textContent = '-';
      }
    }

  }catch(e){
    // ignore
  }
}

// Poll system health
async function pollHealth() {
  try {
    const r = await fetch('/health');
    const j = await r.json();
    
    // Update heap memory
    const freeKB = (j.freeHeap / 1024).toFixed(1);
    const minKB = (j.minFreeHeap / 1024).toFixed(1);
    document.getElementById('freeHeap').textContent = freeKB;
    document.getElementById('minFreeHeap').textContent = minKB;
    
    // Update WiFi RSSI with color coding
    const rssi = j.wifiRSSI;
    const rssiEl = document.getElementById('rssiColor');
    const qualityEl = document.getElementById('rssiQuality');
    
    if (rssi === 0) {
      document.getElementById('wifiRSSI').textContent = 'AP Mode';
      rssiEl.style.color = 'var(--blue)';
      qualityEl.textContent = 'Access Point';
    } else {
      document.getElementById('wifiRSSI').textContent = rssi;
      if (rssi > -50) {
        rssiEl.style.color = 'var(--ok)';
        qualityEl.textContent = 'Excellent';
      } else if (rssi > -70) {
        rssiEl.style.color = 'var(--warn)';
        qualityEl.textContent = 'Good';
      } else {
        rssiEl.style.color = 'var(--hot)';
        qualityEl.textContent = 'Weak';
      }
    }
    
    // Update uptime in health card
    const days = Math.floor(j.uptime / 86400);
    const hrs = Math.floor((j.uptime % 86400) / 3600);
    const mins = Math.floor((j.uptime % 3600) / 60);
    const secs = j.uptime % 60;
    let uptimeStr = '';
    if (days > 0) uptimeStr += days + 'd ';
    if (hrs > 0 || days > 0) uptimeStr += hrs + 'h ';
    if (mins > 0 || hrs > 0 || days > 0) uptimeStr += mins + 'm ';
    uptimeStr += secs + 's';
    document.getElementById('uptime').textContent = uptimeStr;
    
  } catch(e) {
    console.error('health poll error', e);
  }
}

// Poll alerts
async function pollAlerts() {
  try {
    const r = await fetch('/alerts');
    const j = await r.json();
    
    const card = document.getElementById('alertCard');
    const tempEl = document.getElementById('alertTemp');
    const humEl = document.getElementById('alertHum');
    const anomalyEl = document.getElementById('alertAnomaly');
    
    let hasAlert = false;
    
    // Temperature alert
    if (j.tempCritical) {
      tempEl.textContent = '⚠️ Temperature CRITICAL!';
      tempEl.style.background = 'var(--hot)';
      tempEl.style.color = '#fff';
      hasAlert = true;
    } else {
      tempEl.textContent = '✓ Temperature OK';
      tempEl.style.background = '#28a745';
      tempEl.style.color = '#fff';
    }
    
    // Humidity alert
    if (j.humCritical) {
      humEl.textContent = '⚠️ Humidity WET!';
      humEl.style.background = 'var(--hot)';
      humEl.style.color = '#fff';
      hasAlert = true;
    } else {
      humEl.textContent = '✓ Humidity OK';
      humEl.style.background = '#28a745';
      humEl.style.color = '#fff';
    }
    
    // Anomaly alert
    if (j.anomalyDetected) {
      anomalyEl.textContent = '⚠️ Anomaly Detected!';
      anomalyEl.style.background = 'var(--warn)';
      anomalyEl.style.color = '#000';
      hasAlert = true;
    } else {
      anomalyEl.textContent = '✓ No Anomaly';
      anomalyEl.style.background = '#28a745';
      anomalyEl.style.color = '#fff';
    }
    
    // Update card border to flash on alert
    if (hasAlert && j.enabled) {
      card.style.borderColor = 'var(--hot)';
      card.style.animation = 'pulse 1s infinite';
      
      // Browser notification (if permissions granted)
      if (Notification.permission === 'granted') {
        if (j.tempCritical) {
          new Notification('ESP32 Alert', { 
            body: 'Temperature CRITICAL!',
            icon: '🌡️'
          });
        }
      }
    } else {
      card.style.borderColor = 'var(--border)';
      card.style.animation = 'none';
    }
    
    // Update counter and last alert time
    document.getElementById('alertCount').textContent = j.alertCount;
    if (j.lastAlertTime > 0) {
      const ago = ((Date.now() - j.lastAlertTime) / 1000).toFixed(0);
      document.getElementById('lastAlert').textContent = ago + 's ago';
    }
    
    // Update toggle button
    const btnToggle = document.getElementById('btnAlertToggle');
    if (j.enabled) {
      btnToggle.textContent = 'ON';
      btnToggle.style.background = 'var(--ok)';
      btnToggle.style.color = '#000';
    } else {
      btnToggle.textContent = 'OFF';
      btnToggle.style.background = 'transparent';
      btnToggle.style.color = 'var(--muted)';
    }
    
  } catch(e) {
    console.error('alerts poll error', e);
  }
}

// Draw historical chart
let chartData = { temp: [], hum: [], time: [] };
async function pollHistory() {
  try {
    const r = await fetch('/history');
    const j = await r.json();
    chartData = j;
    drawChart();
  } catch(e) {
    console.error('history poll error', e);
  }
}

function drawChart() {
  const canvas = document.getElementById('historyChart');
  if (!canvas || !chartData.temp.length) return;
  
  const ctx = canvas.getContext('2d');
  const w = canvas.width = canvas.offsetWidth;
  const h = canvas.height = 200;
  
  // Clear
  ctx.fillStyle = '#0e121b';
  ctx.fillRect(0, 0, w, h);
  
  const len = chartData.temp.length;
  if (len < 2) return;
  
  // Find min/max for scaling
  const tempMin = Math.min(...chartData.temp);
  const tempMax = Math.max(...chartData.temp);
  const humMin = Math.min(...chartData.hum);
  const humMax = Math.max(...chartData.hum);
  
  const margin = 30;
  const plotW = w - 2 * margin;
  const plotH = h - 2 * margin;
  
  // Draw temperature line (blue)
  ctx.strokeStyle = '#54c8ff';
  ctx.lineWidth = 2;
  ctx.beginPath();
  for (let i = 0; i < len; i++) {
    const x = margin + (i / (len - 1)) * plotW;
    const y = h - margin - ((chartData.temp[i] - tempMin) / (tempMax - tempMin || 1)) * plotH;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();
  
  // Draw humidity line (orange)
  ctx.strokeStyle = '#ffb020';
  ctx.lineWidth = 2;
  ctx.beginPath();
  for (let i = 0; i < len; i++) {
    const x = margin + (i / (len - 1)) * plotW;
    const y = h - margin - ((chartData.hum[i] - humMin) / (humMax - humMin || 1)) * plotH;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();
  
  // Draw labels
  ctx.fillStyle = '#a9b2c2';
  ctx.font = '11px system-ui';
  ctx.fillText('T: ' + tempMin.toFixed(1) + '-' + tempMax.toFixed(1) + '°C', 5, 15);
  ctx.fillText('H: ' + humMin.toFixed(1) + '-' + humMax.toFixed(1) + '%', 5, 30);
}

setInterval(poll, 500);
setInterval(pollHealth, 2000);
setInterval(pollAlerts, 1000);
setInterval(pollHistory, 3000);

poll();
pollHealth();
pollAlerts();
pollHistory();

// Request notification permission
if ('Notification' in window && Notification.permission === 'default') {
  Notification.requestPermission();
}

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

// Alert toggle
document.getElementById('btnAlertToggle').addEventListener('click', async ()=>{
  const r = await fetch('/alerts/toggle');
  const msg = await r.text();
  console.log(msg);
  pollAlerts(); // Immediate refresh
});
</script>
</body>
</html>
)HTML";

#endif // WEB_PAGES_H
