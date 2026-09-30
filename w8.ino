#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ap_ssid = "ESP32_Config_AP";
const char* ap_password = "123456789";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED x2 (LED1 = ของเดิม GPIO5, LED2 = เพิ่มใหม่ GPIO18)
const int ledPin1 = 5;
const int ledPin2 = 18;
WebServer server(80);
int ledState1 = 0;
int ledState2 = 0;

// หน้าเว็บทั้งหมด inline 100% (ห้ามดึง CDN/Google Fonts ภายนอก เพราะ ESP32 เป็น AP ไม่มีเน็ต)
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Control Center</title>
<style>
:root{
  --bg0:#0b1220; --bg1:#111c33;
  --card:#16223c; --card2:#1b2a49;
  --line:rgba(148,163,184,.16);
  --txt:#f1f5f9; --muted:#9fb0c7;
  --temp-a:#fbbf24; --temp-b:#ef4444;
  --hum-a:#22d3ee; --hum-b:#3b82f6;
  --green:#22c55e; --red:#ef4444;
  --focus:#38bdf8;
  --r:20px;
}
*{box-sizing:border-box; -webkit-tap-highlight-color:transparent;}
html,body{margin:0;padding:0;}
body{
  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,"Noto Sans Thai",Arial,sans-serif;
  font-size:16px; line-height:1.5; color:var(--txt);
  background:radial-gradient(1200px 600px at 20% -10%,#1d3a6e 0%,transparent 55%),
             radial-gradient(1000px 500px at 90% 0%,#5b21b6 0%,transparent 50%),
             linear-gradient(180deg,var(--bg1),var(--bg0));
  min-height:100dvh; padding:20px 14px 40px;
  display:flex; justify-content:center;
}
.wrap{width:100%; max-width:720px;}
header.top{
  display:flex; align-items:center; gap:12px; margin-bottom:16px;
}
.logo{
  width:44px;height:44px;border-radius:14px;flex:0 0 44px;
  display:grid;place-items:center;
  background:linear-gradient(135deg,#3b82f6,#8b5cf6);
  box-shadow:0 6px 18px rgba(59,130,246,.35);
}
.logo svg{width:26px;height:26px;fill:#fff;}
h1{font-size:20px;margin:0;font-weight:700;letter-spacing:.2px;}
.sub{font-size:13px;color:var(--muted);margin:2px 0 0;}
.statusbar{
  margin-left:auto; display:flex; align-items:center; gap:8px;
  background:rgba(34,197,94,.12); border:1px solid rgba(34,197,94,.35);
  color:#bbf7d0; font-size:13px; font-weight:600;
  padding:8px 12px; border-radius:999px; white-space:nowrap;
}
.dot{width:9px;height:9px;border-radius:50%;background:var(--green);box-shadow:0 0 10px var(--green);}
.dot.off{background:var(--red);box-shadow:0 0 10px var(--red);}
.grid{display:grid; gap:14px; grid-template-columns:repeat(auto-fit,minmax(250px,1fr));}
.card{
  background:linear-gradient(180deg,var(--card2),var(--card));
  border:1px solid var(--line); border-radius:var(--r);
  padding:18px 16px 16px; box-shadow:0 10px 30px rgba(0,0,0,.35);
}
.card-head{display:flex;align-items:center;gap:10px;margin-bottom:6px;}
.chip{width:36px;height:36px;border-radius:12px;display:grid;place-items:center;flex:0 0 36px;}
.chip svg{width:22px;height:22px;fill:#fff;}
.chip.temp{background:linear-gradient(135deg,#f59e0b,#ef4444);}
.chip.hum{background:linear-gradient(135deg,#06b6d4,#3b82f6);}
.card-head b{font-size:15px;}
.card-head small{display:block;font-size:12px;color:var(--muted);font-weight:400;}
.badge{margin-left:auto;font-size:12px;font-weight:700;padding:5px 10px;border-radius:999px;border:1px solid var(--line);background:rgba(148,163,184,.12);color:var(--muted);white-space:nowrap;}
.badge.ok{background:rgba(34,197,94,.14);border-color:rgba(34,197,94,.4);color:#bbf7d0;}
.badge.warn{background:rgba(251,191,36,.14);border-color:rgba(251,191,36,.45);color:#fde68a;}
.badge.hot{background:rgba(239,68,68,.15);border-color:rgba(239,68,68,.45);color:#fecaca;}
.gauge{width:100%;max-width:280px;margin:2px auto 0;display:block;}
.g-val{text-align:center;margin-top:-8px;}
.g-num{font-size:38px;font-weight:800;font-variant-numeric:tabular-nums;letter-spacing:.5px;line-height:1.1;}
.g-unit{font-size:16px;color:var(--muted);font-weight:600;}
.g-sub{font-size:13px;color:var(--muted);margin-top:2px;}
.g-scale{display:flex;justify-content:space-between;font-size:11px;color:var(--muted);padding:6px 8px 0;font-variant-numeric:tabular-nums;}
.needle{transition:transform .3s ease-out;transform-origin:100px 100px;}
.arc-fg{transition:stroke-dashoffset .3s ease-out;}
.section-title{display:flex;align-items:center;gap:10px;margin:20px 2px 12px;}
.section-title h2{font-size:16px;margin:0;}
.section-title span{font-size:12px;color:var(--muted);}
.led-grid{display:grid;gap:14px;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));}
.led-card{
  background:linear-gradient(180deg,var(--card2),var(--card));
  border:1px solid var(--line);border-radius:var(--r);padding:18px 16px;
  box-shadow:0 10px 30px rgba(0,0,0,.35);
  display:flex;flex-direction:column;gap:12px;
}
.led-top{display:flex;align-items:center;gap:12px;}
.bulb{
  width:52px;height:52px;border-radius:16px;display:grid;place-items:center;flex:0 0 52px;
  background:rgba(148,163,184,.12);border:1px solid var(--line);
  transition:background .25s ease,box-shadow .25s ease,border-color .25s ease;
}
.bulb svg{width:30px;height:30px;fill:#7d8aa0;transition:fill .25s ease;}
.led-card.on .bulb{background:radial-gradient(circle at 50% 35%,#fef9c3,#fde047 55%,#f59e0b);border-color:#fde047;box-shadow:0 0 24px rgba(253,224,71,.55);}
.led-card.on .bulb svg{fill:#713f12;}
.led-name{font-weight:700;font-size:15px;}
.led-name small{display:block;color:var(--muted);font-weight:400;font-size:12px;}
.led-state{margin-left:auto;font-size:13px;font-weight:800;padding:6px 12px;border-radius:999px;background:rgba(148,163,184,.14);color:var(--muted);border:1px solid var(--line);}
.led-card.on .led-state{background:rgba(34,197,94,.15);color:#bbf7d0;border-color:rgba(34,197,94,.45);}
.led-btn{
  touch-action:manipulation;cursor:pointer;
  min-height:52px;width:100%;border:none;border-radius:14px;
  font-size:16px;font-weight:800;color:#fff;
  display:flex;align-items:center;justify-content:center;gap:10px;
  background:linear-gradient(135deg,#16a34a,#22c55e);
  box-shadow:0 6px 16px rgba(34,197,94,.35);
  transition:transform .15s ease,filter .15s ease,opacity .15s ease;
}
.led-btn svg{width:22px;height:22px;fill:#fff;}
.led-card.on .led-btn{background:linear-gradient(135deg,#dc2626,#ef4444);box-shadow:0 6px 16px rgba(239,68,68,.35);}
.led-btn:active{transform:scale(.97);}
.led-btn:disabled{opacity:.6;cursor:wait;}
.led-btn:focus-visible{outline:3px solid var(--focus);outline-offset:2px;}
footer{margin-top:16px;text-align:center;color:var(--muted);font-size:12px;}
footer b{color:var(--txt);}
#toast{
  position:fixed;left:50%;bottom:22px;transform:translateX(-50%) translateY(20px);
  background:#0f172a;color:#f1f5f9;border:1px solid var(--line);
  padding:10px 16px;border-radius:12px;font-size:14px;opacity:0;pointer-events:none;
  transition:opacity .25s ease,transform .25s ease;max-width:92vw;text-align:center;
}
#toast.show{opacity:1;transform:translateX(-50%) translateY(0);}
@media (prefers-reduced-motion:reduce){
  *{transition:none !important;animation:none !important;}
}
</style>
</head>
<body>
<div class="wrap">
  <header class="top">
    <div class="logo" aria-hidden="true">
      <svg viewBox="0 0 24 24"><path d="M12 2a7 7 0 0 0-4 12.7V17a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1v-2.3A7 7 0 0 0 12 2zm-2 17v1a2 2 0 0 0 4 0v-1h-4z"/></svg>
    </div>
    <div>
      <h1>ESP32 Control Center</h1>
      <p class="sub">DHT11 &bull; AP Mode &bull; 192.168.4.1</p>
    </div>
    <div class="statusbar" role="status"><span class="dot" id="netdot"></span><span id="nettxt">ออนไลน์</span></div>
  </header>

  <!-- เกจวัด -->
  <div class="grid" aria-live="polite">
    <div class="card">
      <div class="card-head">
        <div class="chip temp" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M12 2a3 3 0 0 0-3 3v7.75a5 5 0 1 0 6 0V5a3 3 0 0 0-3-3zm1 14.18V17a1 1 0 0 1-2 0v-.82a3 3 0 1 1 2 0z"/></svg></div>
        <div><b>อุณหภูมิ</b><small>Temperature &bull; DHT11</small></div>
        <span class="badge" id="temp-badge">--</span>
      </div>
      <svg class="gauge" viewBox="0 0 200 118" role="img" aria-label="เกจวัดอุณหภูมิ">
        <defs>
          <linearGradient id="gTemp" x1="0" y1="0" x2="1" y2="0">
            <stop offset="0" stop-color="#fbbf24"/><stop offset="1" stop-color="#ef4444"/>
          </linearGradient>
        </defs>
        <path d="M20 100 A80 80 0 0 1 180 100" fill="none" stroke="#26334f" stroke-width="14" stroke-linecap="round"/>
        <path id="temp-arc" class="arc-fg" d="M20 100 A80 80 0 0 1 180 100" fill="none" stroke="url(#gTemp)" stroke-width="14" stroke-linecap="round" stroke-dasharray="251.4" stroke-dashoffset="251.4"/>
        <g stroke="#475569" stroke-width="2">
          <line x1="20" y1="100" x2="30" y2="100"/><line x1="60" y1="40" x2="66" y2="47"/>
          <line x1="100" y1="20" x2="100" y2="30"/><line x1="140" y1="40" x2="134" y2="47"/>
          <line x1="180" y1="100" x2="170" y2="100"/>
        </g>
        <line id="temp-needle" class="needle" x1="100" y1="100" x2="100" y2="38" stroke="#f1f5f9" stroke-width="4" stroke-linecap="round"/>
        <circle cx="100" cy="100" r="9" fill="#e2e8f0"/><circle cx="100" cy="100" r="4" fill="#0f172a"/>
      </svg>
      <div class="g-val"><span class="g-num" id="temp-val">--</span> <span class="g-unit">&deg;C</span><div class="g-sub" id="temp-sub">กำลังอ่านค่า…</div></div>
      <div class="g-scale"><span>0</span><span>25</span><span>50&deg;C</span></div>
    </div>

    <div class="card">
      <div class="card-head">
        <div class="chip hum" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg></div>
        <div><b>ความชื้น</b><small>Humidity &bull; DHT11</small></div>
        <span class="badge" id="hum-badge">--</span>
      </div>
      <svg class="gauge" viewBox="0 0 200 118" role="img" aria-label="เกจวัดความชื้น">
        <defs>
          <linearGradient id="gHum" x1="0" y1="0" x2="1" y2="0">
            <stop offset="0" stop-color="#22d3ee"/><stop offset="1" stop-color="#3b82f6"/>
          </linearGradient>
        </defs>
        <path d="M20 100 A80 80 0 0 1 180 100" fill="none" stroke="#26334f" stroke-width="14" stroke-linecap="round"/>
        <path id="hum-arc" class="arc-fg" d="M20 100 A80 80 0 0 1 180 100" fill="none" stroke="url(#gHum)" stroke-width="14" stroke-linecap="round" stroke-dasharray="251.4" stroke-dashoffset="251.4"/>
        <g stroke="#475569" stroke-width="2">
          <line x1="20" y1="100" x2="30" y2="100"/><line x1="60" y1="40" x2="66" y2="47"/>
          <line x1="100" y1="20" x2="100" y2="30"/><line x1="140" y1="40" x2="134" y2="47"/>
          <line x1="180" y1="100" x2="170" y2="100"/>
        </g>
        <line id="hum-needle" class="needle" x1="100" y1="100" x2="100" y2="38" stroke="#f1f5f9" stroke-width="4" stroke-linecap="round"/>
        <circle cx="100" cy="100" r="9" fill="#e2e8f0"/><circle cx="100" cy="100" r="4" fill="#0f172a"/>
      </svg>
      <div class="g-val"><span class="g-num" id="hum-val">--</span> <span class="g-unit">%</span><div class="g-sub" id="hum-sub">กำลังอ่านค่า…</div></div>
      <div class="g-scale"><span>0</span><span>50</span><span>100%</span></div>
    </div>
  </div>

  <div class="section-title"><h2>ควบคุมไฟ LED</h2><span>แตะปุ่มเพื่อ เปิด / ปิด &bull; รองรับคอม + มือถือ</span></div>

  <!-- LED x2 -->
  <div class="led-grid">
    <div class="led-card" id="card1">
      <div class="led-top">
        <div class="bulb" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M9 21c0 .55.45 1 1 1h4c.55 0 1-.45 1-1v-1H9v1zm3-19C8.14 2 5 5.14 5 9c0 2.38 1.19 4.47 3 5.74V17c0 .55.45 1 1 1h6c.55 0 1-.45 1-1v-2.26c1.81-1.27 3-3.36 3-5.74 0-3.86-3.14-7-7-7z"/></svg></div>
        <div class="led-name">LED 1<small>GPIO 5</small></div>
        <span class="led-state" id="state1">ปิด</span>
      </div>
      <button class="led-btn" id="btn1" aria-pressed="false" aria-label="เปิดปิด LED 1">
        <svg viewBox="0 0 24 24"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42A6.99 6.99 0 0 1 19 12a7 7 0 0 1-14 0c0-1.93.78-3.68 2.05-4.94L5.64 5.64A8.99 8.99 0 0 0 3 12a9 9 0 0 0 18 0c0-2.74-1.23-5.19-3.17-6.83z"/></svg>
        <span id="btntxt1">เปิดไฟ</span>
      </button>
    </div>
    <div class="led-card" id="card2">
      <div class="led-top">
        <div class="bulb" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M9 21c0 .55.45 1 1 1h4c.55 0 1-.45 1-1v-1H9v1zm3-19C8.14 2 5 5.14 5 9c0 2.38 1.19 4.47 3 5.74V17c0 .55.45 1 1 1h6c.55 0 1-.45 1-1v-2.26c1.81-1.27 3-3.36 3-5.74 0-3.86-3.14-7-7-7z"/></svg></div>
        <div class="led-name">LED 2<small>GPIO 18</small></div>
        <span class="led-state" id="state2">ปิด</span>
      </div>
      <button class="led-btn" id="btn2" aria-pressed="false" aria-label="เปิดปิด LED 2">
        <svg viewBox="0 0 24 24"><path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42A6.99 6.99 0 0 1 19 12a7 7 0 0 1-14 0c0-1.93.78-3.68 2.05-4.94L5.64 5.64A8.99 8.99 0 0 0 3 12a9 9 0 0 0 18 0c0-2.74-1.23-5.19-3.17-6.83z"/></svg>
        <span id="btntxt2">เปิดไฟ</span>
      </button>
    </div>
  </div>

  <footer>อัปเดตอัตโนมัติทุก <b>2 วินาที</b> &bull; อัปเดตล่าสุด <b id="upd">--:--:--</b> &bull; ทำงานแบบ Offline (AP Mode)</footer>
</div>
<div id="toast" role="status"></div>
<script>
(function(){
  var ARC=251.4;
  function clamp(v,a,b){return Math.max(a,Math.min(b,v));}
  function setGauge(prefix,v,min,max){
    var f=clamp((v-min)/(max-min),0,1);
    document.getElementById(prefix+'-arc').style.strokeDashoffset=(ARC*(1-f)).toFixed(1);
    document.getElementById(prefix+'-needle').style.transform='rotate('+(-90+f*180).toFixed(1)+'deg)';
  }
  function toast(m){var t=document.getElementById('toast');t.textContent=m;t.classList.add('show');clearTimeout(t._h);t._h=setTimeout(function(){t.classList.remove('show');},2500);}
  function setLed(n,on){
    var card=document.getElementById('card'+n);
    var st=document.getElementById('state'+n);
    var bt=document.getElementById('btn'+n);
    var tx=document.getElementById('btntxt'+n);
    card.classList.toggle('on',!!on);
    st.textContent=on?'เปิด':'ปิด';
    tx.textContent=on?'ปิดไฟ':'เปิดไฟ';
    bt.setAttribute('aria-pressed',on?'true':'false');
  }
  function update(){
    fetch('/data').then(function(r){if(!r.ok)throw 0;return r.json();}).then(function(d){
      document.getElementById('nettxt').textContent='ออนไลน์';
      document.getElementById('netdot').classList.remove('off');
      document.getElementById('temp-val').textContent=(+d.temperature).toFixed(1);
      document.getElementById('hum-val').textContent=(+d.humidity).toFixed(1);
      setGauge('temp',+d.temperature,0,50);
      setGauge('hum',+d.humidity,0,100);
      var tb=document.getElementById('temp-badge'),ts=document.getElementById('temp-sub');
      var t=+d.temperature;
      if(t<20){tb.textContent='เย็น';tb.className='badge ok';ts.textContent='อากาศเย็นสบาย';}
      else if(t<30){tb.textContent='สบาย';tb.className='badge ok';ts.textContent='อุณหภูมิปกติ';}
      else if(t<35){tb.textContent='อุ่น';tb.className='badge warn';ts.textContent='เริ่มอุ่น ระบายอากาศหน่อย';}
      else{tb.textContent='ร้อน';tb.className='badge hot';ts.textContent='อากาศร้อน!';}
      var hb=document.getElementById('hum-badge'),hs=document.getElementById('hum-sub');
      var h=+d.humidity;
      if(h<30){hb.textContent='แห้ง';hb.className='badge warn';hs.textContent='อากาศแห้ง';}
      else if(h<=70){hb.textContent='พอดี';hb.className='badge ok';hs.textContent='ความชื้นเหมาะสม';}
      else{hb.textContent='ชื้น';hb.className='badge warn';hs.textContent='ความชื้นสูง';}
      setLed(1,d.led1===1||d.led===1);
      setLed(2,d.led2===1);
      var now=new Date();
      document.getElementById('upd').textContent=('0'+now.getHours()).slice(-2)+':'+('0'+now.getMinutes()).slice(-2)+':'+('0'+now.getSeconds()).slice(-2);
    }).catch(function(){
      document.getElementById('nettxt').textContent='ขาดการเชื่อมต่อ';
      document.getElementById('netdot').classList.add('off');
    });
  }
  function toggle(n){
    var on=document.getElementById('card'+n).classList.contains('on');
    var btn=document.getElementById('btn'+n);
    btn.disabled=true;
    fetch('/led'+n+(on?'/off':'/on')).then(function(){update();}).catch(function(){toast('สั่งงาน LED '+n+' ไม่สำเร็จ');}).finally(function(){btn.disabled=false;});
  }
  document.getElementById('btn1').addEventListener('click',function(){toggle(1);});
  document.getElementById('btn2').addEventListener('click',function(){toggle(2);});
  update();
  setInterval(update,2000);
})();
</script>
</body>
</html>
)rawliteral";

// API ส่ง JSON: temperature, humidity, led1, led2 (+ led = led1 เพื่อรองรับของเดิม)
void handleData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) { t = 0.0; h = 0.0; }

  String json = "{";
  json += "\"temperature\":" + String(t, 1) + ",";
  json += "\"humidity\":" + String(h, 1) + ",";
  json += "\"led1\":" + String(ledState1) + ",";
  json += "\"led2\":" + String(ledState2) + ",";
  json += "\"led\":" + String(ledState1);
  json += "}";

  server.send(200, "application/json", json);
}

void setLed(int pin, int &state, int v) {
  digitalWrite(pin, v ? HIGH : LOW);
  state = v ? 1 : 0;
}

void handleLed1On()  { setLed(ledPin1, ledState1, 1); server.send(200, "text/plain", "ON"); }
void handleLed1Off() { setLed(ledPin1, ledState1, 0); server.send(200, "text/plain", "OFF"); }
void handleLed2On()  { setLed(ledPin2, ledState2, 1); server.send(200, "text/plain", "ON"); }
void handleLed2Off() { setLed(ledPin2, ledState2, 0); server.send(200, "text/plain", "OFF"); }

void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  Serial.println("\n--- Access Point Started ---");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.on("/data", handleData);
  server.on("/led1/on", handleLed1On);
  server.on("/led1/off", handleLed1Off);
  server.on("/led2/on", handleLed2On);
  server.on("/led2/off", handleLed2Off);
  // backward compatible กับปุ่มเดิม /on /off -> LED1
  server.on("/on", handleLed1On);
  server.on("/off", handleLed1Off);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
