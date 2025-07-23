/**
 * @file M-ESP8266.ino
 * @author Osman Selçuk SELÇUKLU 
 * @brief IoT-Tea-Tracking-System
 * @version 1.0.0
 * @date 23 July 2025
 * 
 * @copyright OSS EMBEDDED SYSTEM Copyright (C) 2025. See: license/license.txt
 * @link --
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <pgmspace.h>

// ——— Network Settings ———
const char* SSID     = "YOUR_WIFI_SSID";
const char* PASSWORD = "YOUR_WIFI_PASSWORD";

IPAddress local_IP(10,0,0,77);           // Static IP
IPAddress gateway (10,0,0,1);            // IPAddress gateway (192, 168, 1, 1);   ağ geçidi (router)
IPAddress subnet  (255, 255, 255, 0);    // Subnet mask
IPAddress dns     (8, 8, 8, 8);          // DNS server

// ——— Timer Settings ———
const unsigned long PREP_MS    = 20UL * 60UL * 1000UL;         // 20  minutes
const unsigned long EXPIRE_MS  = 120UL * 60UL * 1000UL;        // 120 minutes
const unsigned long HEARTBEAT_TIMEOUT = 180000UL;              // 180 seconds

// ——— Buzzer ve Live LED ———
const int BUZZER_PIN   = 13;  // GPIO13
const int LIVE_LED_PIN = 4;   // GPIO4 (LOW/ON, HIGH/OFF)

// Live LED blink interval
const unsigned long LIVE_BLINK_INTERVAL = 500; // ms
unsigned long lastLiveBlink = 0;
bool liveLedState = false;

// ——— Buzzer Control Variables ———
bool buzzerPlaying = false;
unsigned long buzzerStartTime = 0;
int buzzerStep = 0;
int buzzerType = 0; // 0: silent, 1: start fade, 2: ready melody

// ——— Web Server ———
ESP8266WebServer server(80);

// ——— Floor Structure ———
struct Floor {
  int    id;
  bool   prepping;
  bool   prepared;
  unsigned long startPrep;
  unsigned long startExpire;
  bool   online;
  unsigned long lastSeen;
  unsigned long lastHeartbeat;
  String lastIP;
  int    connectionQuality;
};

Floor floors[] = {
  {1,false,false,0,0,false,0,0,"",0},
  {2,false,false,0,0,false,0,0,"",0},
  {3,false,false,0,0,false,0,0,"",0}
};
const int FLOOR_COUNT = sizeof(floors)/sizeof(floors[0]);

// ——— System Statistics ———
unsigned long systemStartTime = 0;
unsigned long totalRequests   = 0;
unsigned long totalHeartbeats = 0;



// ——— Buzzer Functions ———
void playStartSound() {
  buzzerPlaying  = true;
  buzzerStartTime = millis();
  buzzerType     = 1;  
  Serial.println("🔊 Çay hazırlanıyor – mutlu jingle başlatıldı");
}

void playReadySound() {
  buzzerPlaying  = true;
  buzzerStartTime = millis();
  buzzerType     = 2;  
  Serial.println("🔊 Çay hazır – kutlama jingle’ı başlatıldı");
}

void handleBuzzer() {
  if (!buzzerPlaying) return;
  unsigned long elapsed = millis() - buzzerStartTime;

  if (buzzerType == 1) {
    /* START JINGLE ‑ C (523 Hz) → E (659 Hz) → G (784 Hz)  */
    if      (elapsed < 150)  tone(BUZZER_PIN, 523);
    else if (elapsed < 200)  noTone(BUZZER_PIN);
    else if (elapsed < 350)  tone(BUZZER_PIN, 659);
    else if (elapsed < 400)  noTone(BUZZER_PIN);
    else if (elapsed < 550)  tone(BUZZER_PIN, 784);
    else { noTone(BUZZER_PIN); buzzerPlaying = false; Serial.println("🔊 Hazırlama jingle’ı bitti"); }
  }

  else if (buzzerType == 2) {
    /* READY JINGLE ‑ G (784 Hz) → E (659 Hz) → C (523 Hz) → C8 (1046 Hz) */
    if      (elapsed < 150)  tone(BUZZER_PIN, 784);
    else if (elapsed < 200)  noTone(BUZZER_PIN);
    else if (elapsed < 350)  tone(BUZZER_PIN, 659);
    else if (elapsed < 400)  noTone(BUZZER_PIN);
    else if (elapsed < 550)  tone(BUZZER_PIN, 523);
    else if (elapsed < 600)  noTone(BUZZER_PIN);
    else if (elapsed < 750)  tone(BUZZER_PIN, 1046);
    else { noTone(BUZZER_PIN); buzzerPlaying = false; Serial.println("🔊 Hazır jingle’ı bitti"); }
  }
}




/* =======================================================
 *   WEB INTERFACE (same as the design you sent)
 * ======================================================= */
// Web Server HTML page for Arduino IDE
// Available for ESP32 or ESP8266

const char* html_page = R"rawliteral(

<!doctype html>
<html lang="tr">
<head>
    <meta charset="utf-8">
    <title>Çay Takip Sistemi</title>
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Inter:wght@300;400;500;600;700;800&display=swap');
        
        :root {
            --bg-primary: #0a0e1a;
            --bg-secondary: #1a1f35;
            --bg-card: rgba(26, 31, 53, 0.85);
            --bg-card-hover: rgba(30, 35, 60, 0.95);
            --txt-primary: #ffffff;
            --txt-secondary: #94a3b8;
            --txt-muted: #64748b;
            --accent: #00f5ff;
            --accent-glow: rgba(0, 245, 255, 0.3);
            --success: #00ff88;
            --success-glow: rgba(0, 255, 136, 0.3);
            --warning: #ff9500;
            --warning-glow: rgba(255, 149, 0, 0.3);
            --danger: #ff4757;
            --danger-glow: rgba(255, 71, 87, 0.3);
            --orange: #ff6b35;
            --orange-light: #ff8c69;
            --orange-glow: rgba(255, 107, 53, 0.3);
            --border: rgba(0, 245, 255, 0.2);
            --border-hover: rgba(0, 245, 255, 0.5);
            --shadow-lg: 0 25px 50px -12px rgba(0, 0, 0, 0.4);
            --shadow-glow: 0 0 30px var(--accent-glow);
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        html, body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            background: linear-gradient(135deg, var(--bg-primary) 0%, #0f1419 50%, var(--bg-secondary) 100%);
            color: var(--txt-primary);
            min-height: 100vh;
            overflow-x: hidden;
            position: relative;
        }

        body::after {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-image: 
                linear-gradient(rgba(0, 245, 255, 0.03) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0, 245, 255, 0.03) 1px, transparent 1px);
            background-size: 50px 50px;
            z-index: -1;
            opacity: 0.5;
        }

        .header {
            text-align: center;
            padding: 3rem 2rem;
            position: relative;
            background: rgba(0, 0, 0, 0.2);
            backdrop-filter: blur(10px);
            border-bottom: 1px solid var(--border);
        }

        .header::after {
            content: '';
            position: absolute;
            bottom: -1px;
            left: 50%;
            transform: translateX(-50%);
            width: 200px;
            height: 2px;
            background: linear-gradient(90deg, transparent, var(--orange), var(--accent), transparent);
        }

        h1 {
            font-family: 'Orbitron', monospace;
            font-size: clamp(2.5rem, 6vw, 4.5rem);
            font-weight: 900;
            background: linear-gradient(135deg, var(--accent) 0%, #ffffff 20%, var(--orange) 40%, var(--success) 70%, var(--accent) 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            text-shadow: 0 0 50px var(--accent-glow);
            margin-bottom: 1rem;
            letter-spacing: 3px;
        }

        #dateNow {
            font-size: 1.2rem;
            font-weight: 500;
            color: var(--txt-secondary);
            padding: 1rem 2rem;
            background: linear-gradient(135deg, rgba(0, 245, 255, 0.1), rgba(255, 107, 53, 0.08));
            border: 1px solid var(--border);
            border-radius: 50px;
            display: inline-block;
            backdrop-filter: blur(20px);
            box-shadow: 0 8px 32px rgba(0, 245, 255, 0.15);
        }
        

        #cards {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(380px, 1fr));
            gap: 2.5rem;
            padding: 4rem 2rem;
            max-width: 1600px;
            margin: 0 auto;
            padding-bottom: 100px; /* Footer ve sinyal kutusu için boşluk */
        }

        .card {
            position: relative; 
            background: var(--bg-card);
            border: 2px solid var(--border);
            border-radius: 24px;
            padding: 2.5rem;
            backdrop-filter: blur(25px);
            transition: transform 0.5s cubic-bezier(0.4, 0, 0.2, 1);
            overflow: hidden;
        }

        .card:hover {
            transform: translateY(-8px) scale(1.02);
        }

        .card-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-bottom: 2rem;
            position: relative;
        }

        .floor-title {
            font-family: 'Orbitron', monospace;
            font-size: 1.8rem;
            font-weight: 700;
            color: var(--txt-primary);
            text-shadow: 0 0 10px var(--accent-glow);
        }

        .connection-status {
            display: flex;
            align-items: center;
            gap: 0.75rem;
            padding: 0.75rem 1.5rem;
            border-radius: 50px;
            font-size: 0.9rem;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 1px;
            transition: all 0.3s ease;
            backdrop-filter: blur(10px);
        }

        .status-led {
            width: 14px;
            height: 14px;
            border-radius: 50%;
            position: relative;
        }

        .status-led::after {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            width: 24px;
            height: 24px;
            border-radius: 50%;
            opacity: 0.4;
        }

        .status-grid {
            display: grid;
            grid-template-columns: 1fr; 
            gap: 2rem;
            margin-bottom: 2.5rem;
        }

        .status-item {
            text-align: center;
            padding: 1.5rem;
            background: linear-gradient(135deg, rgba(0, 245, 255, 0.05), rgba(255, 107, 53, 0.03));
            border: 1px solid rgba(0, 245, 255, 0.15);
            border-radius: 16px;
            transition: all 0.3s ease;
        }

        .status-item:hover {
            background: linear-gradient(135deg, rgba(0, 245, 255, 0.1), rgba(255, 107, 53, 0.05));
            transform: translateY(-2px);
        }

        .status-label {
            font-size: 0.9rem;
            color: var(--txt-muted);
            margin-bottom: 0.75rem;
            text-transform: uppercase;
            letter-spacing: 2px;
            font-weight: 600;
        }

        .status-value {
            font-family: 'Orbitron', monospace;
            font-size: 2rem;
            font-weight: 700;
            text-shadow: 0 0 10px currentColor;
        }

        .timer-value {
            color: var(--accent);
        }


        .progress-section {
            position: relative;
        }

        .progress-label {
            font-size: 0.9rem;
            color: var(--txt-muted);
            margin-bottom: 1rem;
            text-transform: uppercase;
            letter-spacing: 2px;
            font-weight: 600;
        }

        .progress-container {
            position: relative;
            width: 100%;
            height: 12px;
            background: rgba(0, 0, 0, 0.4);
            border-radius: 50px;
            overflow: hidden;
            box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.6);
        }

        .progress-bar {
            height: 100%;
            width: 0%;
            border-radius: inherit;
            transition: width 0.8s cubic-bezier(0.4, 0, 0.2, 1);
            position: relative;
            overflow: hidden;
        }

        .bekleniyor {
            --theme-color: var(--accent);
            --theme-glow: var(--accent-glow);
            --theme-bg: rgba(0, 245, 255, 0.1);
        }

        .hazirlaniyor {
            --theme-color: var(--orange);
            --theme-glow: var(--orange-glow);
            --theme-bg: rgba(255, 107, 53, 0.1);
        }

        .hazir {
            --theme-color: var(--success);
            --theme-glow: var(--success-glow);
            --theme-bg: rgba(0, 255, 136, 0.1);
        }

        .surebitti {
            --theme-color: var(--danger);
            --theme-glow: var(--danger-glow);
            --theme-bg: rgba(255, 71, 87, 0.1);
        }

        .baglidegil {
            --theme-color: var(--txt-muted);
            --theme-glow: rgba(100, 116, 139, 0.3);
            --theme-bg: rgba(100, 116, 139, 0.1);
            opacity: 0.6;
            filter: grayscale(50%);
        }

        .connection-status {
            background: var(--theme-bg);
            color: var(--theme-color);
            border: 1px solid var(--theme-color);
            box-shadow: 0 0 20px var(--theme-glow);
        }

        .status-led {
            background: var(--theme-color);
            box-shadow: 0 0 15px var(--theme-color);
        }

        .status-led::after {
            background: var(--theme-color);
        }

        .progress-bar {
            background: linear-gradient(90deg, var(--theme-color), rgba(255,255,255,0.8));
            box-shadow: 0 0 15px var(--theme-glow);
        }

        .footer {
            text-align: center;
            padding: 3rem 2rem;
            border-top: 1px solid var(--border);
            background: rgba(0, 0, 0, 0.3);
            backdrop-filter: blur(15px);
            position: relative; /* All floors signal status için relative */
            z-index: 10; /* Üstte kalmasını sağla */
        }

        .brand-link {
            display: inline-block;
            font-size: 1rem;
            color: var(--txt-muted);
            text-decoration: none;
            transition: all 0.3s ease;
            padding: 0.5rem 1rem;
            border-radius: 8px;
            position: relative;
            overflow: hidden;
            margin-bottom: 20px; /* Sinyal kutusu ile arasına boşluk */
        }

        .brand-link:hover {
            color: var(--orange);
            text-shadow: 0 0 10px var(--orange-glow);
            transform: translateY(-2px);
        }

        .brand-highlight {
            color: var(--orange);
            font-weight: 700;
            text-shadow: 0 0 10px var(--orange-glow);
        }

        .hazirlaniyor .timer-value {
            color: var(--orange);
        }
        
        /* Sayfanın en altında sabit duran ve açılır/kapanır sinyal kutusu */
        #all-floors-signal-status {
            position: fixed; /* Sabit konum */
            bottom: 0; /* Sayfanın en altında */
            left: 50%;
            transform: translateX(-50%);
            width: auto; /* İçeriğine göre genişlik */
            min-width: 250px; /* Minimum genişlik */
            max-width: 90%; /* Maksimum genişlik */
            background: var(--bg-card);
            border: 1px solid var(--border);
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            box-shadow: 0 -8px 20px rgba(0, 0, 0, 0.4);
            backdrop-filter: blur(15px);
            z-index: 1000; /* En önde */
            text-align: left;
            overflow: hidden; /* İçerik taşmasını engelle */
            transition: all 0.3s ease-in-out;
            padding: 0; /* İç paddingi kaldır, içeriğe verilecek */
        }

        #all-floors-signal-status.expanded {
            /* Açıkken ek yükseklik kazanır */
            padding-bottom: 15px; /* Alt boşluk */
        }

        #all-floors-signal-status-header {
            padding: 15px 20px;
            background: rgba(0, 0, 0, 0.4); /* Header için koyu arka plan */
            border-bottom: 1px solid var(--border);
            cursor: pointer;
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-weight: 600;
            color: var(--accent);
            font-size: 1rem;
            letter-spacing: 1px;
            user-select: none; /* Metin seçimi engelle */
        }
        #all-floors-signal-status-header:hover {
            background: rgba(0, 0, 0, 0.6);
        }
        #all-floors-signal-status-header .toggle-icon {
            margin-left: 10px;
            font-size: 1.2rem;
            transition: transform 0.3s ease;
        }
        #all-floors-signal-status.expanded #all-floors-signal-status-header .toggle-icon {
            transform: rotate(180deg); /* Açıldığında oku döndür */
        }

        #all-floors-signal-status-content {
            max-height: 0; /* Varsayılan olarak gizli */
            overflow: hidden;
            transition: max-height 0.4s cubic-bezier(0.4, 0, 0.2, 1), padding-top 0.4s ease;
            padding: 0 20px; /* Yan padding */
            padding-bottom: 0;
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
        }

        #all-floors-signal-status.expanded #all-floors-signal-status-content {
            max-height: 300px; /* Yeterince büyük bir değer, içeriğe göre ayarla */
            padding-top: 15px; /* Üst boşluk */
        }

        #all-floors-signal-status-content .floor-signal-line {
            color: var(--txt-primary);
            font-weight: 500;
            white-space: nowrap;
            font-size: 0.95rem;
        }
        #all-floors-signal-status-content .signal-value {
            color: var(--success);
            font-weight: 700;
        }
        #all-floors-signal-status-content .offline .signal-value {
            color: var(--txt-muted);
        }


        @media (max-width: 768px) {
            #cards {
                grid-template-columns: 1fr;
                padding: 2rem 1rem;
                gap: 2rem;
                padding-bottom: 100px; /* Mobil için de yeterli boşluk */
            }
            
            .card {
                padding: 2rem;
            }
            
            .status-grid {
                grid-template-columns: 1fr;
                gap: 1rem;
            }

            .card-header {
                flex-direction: column;
                gap: 1rem;
                align-items: flex-start;
            }
            
            #all-floors-signal-status {
                width: 95%; /* Mobil genişliği */
                border-radius: 12px 12px 0 0; /* Köşeleri yuvarla */
            }
            #all-floors-signal-status-header {
                padding: 12px 15px;
                font-size: 0.9rem;
            }
            #all-floors-signal-status-content {
                padding: 0 15px;
            }
            #all-floors-signal-status.expanded #all-floors-signal-status-content {
                 padding-top: 12px;
            }
            #all-floors-signal-status-content .floor-signal-line {
                font-size: 0.85rem;
            }
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>ÇAY TAKİP SİSTEMİ</h1>
        <div id="dateNow">--</div>
    </div>
    
    <div id="cards"></div>
    
    <div class="footer">
        <a href="https://www.linkedin.com/in/selcukselcuklu/" target="_blank" rel="noopener noreferrer" class="brand-link">
            <span class="brand-highlight">OSS EMBEDDED SYSTEM</span> • © <span id="brandYear"></span> • Tüm Hakları Saklıdır
        </a>

        <div id="all-floors-signal-status">
            <div id="all-floors-signal-status-header">
                Sinyal Durumu <span class="toggle-icon">▲</span>
            </div>
            <div id="all-floors-signal-status-content">
                </div>
        </div>
    </div>

    <script>
        const PREP_SEC = 20 * 60; // 20 dakika
        const EXPIRE_SEC = 120 * 60; // 120 dakika
        const FLOORS = [3, 2, 1]; // Kat ID'leri, sıralaması önemli değil

        document.getElementById('brandYear').textContent = new Date().getFullYear();

        updateDateTime = function() {
            const now = new Date();
            const date = now.toLocaleDateString('tr-TR', {
                weekday: 'long',
                year: 'numeric',
                month: 'long',
                day: 'numeric'
            });
            const time = now.toLocaleTimeString('tr-TR', {
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            });
            document.getElementById('dateNow').textContent = date + ' • ' + time;
        }

        setInterval(updateDateTime, 1000);
        updateDateTime();

        const cardsDiv = document.getElementById('cards');
        const allFloorsSignalStatus = document.getElementById('all-floors-signal-status');
        const allFloorsSignalStatusHeader = document.getElementById('all-floors-signal-status-header');
        const allFloorsSignalStatusContent = document.getElementById('all-floors-signal-status-content');
        
        const cards = {};

        FLOORS.forEach(function(id) {
            const cardElement = document.createElement('div');
            cardElement.className = 'card baglidegil';
            cardElement.id = 'card-' + id;
            cardElement.innerHTML = 
                '<div class="card-header">' +
                    '<div class="floor-title">KAT ' + id + '</div>' +
                    '<div class="connection-status">' +
                        '<div class="status-led"></div>' +
                        '<span class="status-text">Bağlı Değil</span>' +
                    '</div>' +
                '</div>' +
                '<div class="status-grid">' +
                    '<div class="status-item">' +
                        '<div class="status-label">Kalan Süre</div>' +
                        '<div class="status-value timer-value">--</div>' +
                    '</div>' +
                '</div>' +
                '<div class="progress-section">' +
                    '<div class="progress-label">Durum İlerlemesi</div>' +
                    '<div class="progress-container">' +
                        '<div class="progress-bar"></div>' +
                    '</div>' +
                '</div>';
            
            cardsDiv.appendChild(cardElement);
            
            cards[id] = {
                root: cardElement,
                statusText: cardElement.querySelector('.status-text'),
                timerValue: cardElement.querySelector('.timer-value'),
                progressBar: cardElement.querySelector('.progress-bar')
            };
        });

        fmtSayim = function(sec) {
            if (sec < 0) sec = 0;
            const m = Math.floor(sec / 60);
            const s = sec % 60;
            return m > 0 ? m + 'dk ' + s + 'sn' : s + 'sn';
        }

        dbmFromQual = function(q) {
            q = Number(q) || 0;
            return Math.round(-90 + (q * 60 / 100));
        }

        getStatusClass = function(state, online) {
            if (!online) return 'baglidegil';
            switch (state) {
                case 'preparing': return 'hazirlaniyor';
                case 'ready': return 'hazir';
                case 'expired': return 'surebitti';
                default: return 'bekleniyor';
            }
        }

        getStatusText = function(state, online) {
            if (!online) return 'Bağlı Değil';
            switch (state) {
                case 'preparing': return 'ÇAY DEMLENİYOR';
                case 'ready': return 'ÇAY İÇİLMEYE HAZIR';
                case 'expired': return 'Süre Doldu';
                default: return 'Bekleniyor';
            }
        }

        // Toggle fonksiyonu
        function toggleSignalStatus() {
            allFloorsSignalStatus.classList.toggle('expanded');
        }

        // Header'a tıklama olay dinleyicisi
        allFloorsSignalStatusHeader.addEventListener('click', toggleSignalStatus);


        updateUI = function(data) {
            let signalHtml = ''; 
            
            data.sort((a, b) => a.id - b.id); 

            data.forEach(function(floor) {
                const element = cards[floor.id];
                if (!element) return;

                const online = floor.online;
                const statusClass = getStatusClass(floor.state, online);

                element.root.className = 'card ' + statusClass;
                element.statusText.textContent = getStatusText(floor.state, online);

                if (online && (floor.state === 'preparing' || floor.state === 'ready')) {
                    element.timerValue.textContent = fmtSayim(floor.rem);
                } else {
                    element.timerValue.textContent = '--';
                }
                
                const quality = Number(floor.quality) || 0;
                const dbm = dbmFromQual(quality);
                const signalClass = online ? '' : 'offline'; 
                
                signalHtml += `<div class="floor-signal-line ${signalClass}">`;
                signalHtml += `Kat${floor.id}: Sinyal: <span class="signal-value">${online ? quality + '%' : '--%'}</span> (${online ? dbm + ' dBm' : '--'})`;
                signalHtml += `</div>`;


                let progressPercent = 0;
                if (floor.state === 'preparing') {
                    progressPercent = ((PREP_SEC - floor.rem) / PREP_SEC) * 100;
                } else if (floor.state === 'ready') {
                    progressPercent = ((EXPIRE_SEC - floor.rem) / EXPIRE_SEC) * 100;
                } else if (floor.state === 'expired') {
                    progressPercent = 100;
                }
                
                element.progressBar.style.width = Math.min(Math.max(progressPercent, 0), 100) + '%';
            });
            allFloorsSignalStatusContent.innerHTML = signalHtml; // İçeriği buraya yazdırıyoruz
        }

        poll = function() {
            fetch('/status')
                .then(function(response) { return response.json(); })
                .then(updateUI)
                .catch(function(error) {
                    console.warn('Polling error:', error);
                    let signalHtml = '';
                    FLOORS.sort((a,b) => a - b).forEach(id => {
                        const element = cards[id];
                        if (element) {
                            element.root.className = 'card baglidegil';
                            element.statusText.textContent = 'Bağlı Değil';
                            element.timerValue.textContent = '--';
                            element.progressBar.style.width = '0%';
                        }
                        signalHtml += `<div class="floor-signal-line offline">Kat${id}: Sinyal: <span class="signal-value">--%</span> (--)</div>`;
                    });
                    allFloorsSignalStatusContent.innerHTML = signalHtml;
                });
        }

        setInterval(poll, 1000);
        poll();
    </script>
</body>
</html>
)rawliteral";


// ——— Assistant: Floor status update ———
void tickFloors() {
  unsigned long now = millis();
  for (int i = 0; i < FLOOR_COUNT; i++) {
    Floor &fl = floors[i];

    // Offline algılama
    if (fl.online && fl.lastSeen > 0 && now - fl.lastSeen > HEARTBEAT_TIMEOUT) {
      fl.online = false;
      fl.connectionQuality = 0;
      Serial.println("⚠ Kat " + String(fl.id) + " offline!");
    }

    // Hazırlama → Bayatma
    if (fl.prepping) {
      unsigned long elapsed = now - fl.startPrep;
      if (elapsed >= PREP_MS) {
        unsigned long overshoot = elapsed - PREP_MS;
        fl.prepping = false;
        if (overshoot >= EXPIRE_MS) {
          fl.prepared = false;
          Serial.println("Kat " + String(fl.id) + " süre doldu (hazırlama geç).");
        } else {
          fl.prepared    = true;
          fl.startExpire = now - overshoot;
          Serial.println("Kat " + String(fl.id) + " çay hazır! Overshoot: " + String(overshoot/1000) + "s");
          // Çay hazır melodisi
          playReadySound();
        }
      }
    }
    // Bayatma → Süre Doldu
    else if (fl.prepared) {
      unsigned long elapsed = now - fl.startExpire;
      if (elapsed >= EXPIRE_MS) {
        fl.prepared = false;
        Serial.println("Kat " + String(fl.id) + " çay süresi doldu!");
      }
    }
  }
}



// ——— HTTP Handler'lar ———
void handleRoot() {
  server.send_P(200, "text/html", html_page); // doğru isim
}

void handleRequest() {
  if (!server.hasArg("floor")) {
    server.send(400, "text/plain", "floor param missing");
    return;
  }
  int f    = server.arg("floor").toInt();
  int rssi = server.hasArg("rssi") ? server.arg("rssi").toInt() : 0;
  unsigned long now = millis();
  String ip = server.client().remoteIP().toString();

  for (int i = 0; i < FLOOR_COUNT; i++) {
    if (floors[i].id == f) {
      floors[i].prepping      = true;
      floors[i].prepared      = false;
      floors[i].startPrep     = now;
      floors[i].startExpire   = 0;
      floors[i].online        = true;
      floors[i].lastSeen      = now;
      floors[i].lastHeartbeat = now;
      floors[i].lastIP        = ip;
      floors[i].connectionQuality = map(constrain(rssi, -90, -30), -90, -30, 0, 100);
      totalRequests++;
      Serial.println("Kat " + String(f) + " istekte bulundu (RSSI=" + rssi + ").");
      
      // Çay hazırlama başladı sesi
      playStartSound();
      break;
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleHeartbeat() {
  if (!server.hasArg("floor")) {
    server.send(400, "text/plain", "floor param missing");
    return;
  }
  int f   = server.arg("floor").toInt();
  unsigned long now = millis();
  String ip = server.client().remoteIP().toString();

  for (int i = 0; i < FLOOR_COUNT; i++) {
    if (floors[i].id == f) {
      floors[i].online        = true;
      floors[i].lastSeen      = now;
      floors[i].lastHeartbeat = now;
      floors[i].lastIP        = ip;
      totalHeartbeats++;
      break;
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleConnect() {
  if (!server.hasArg("floor")) {
    server.send(400, "text/plain", "floor param missing");
    return;
  }
  int f   = server.arg("floor").toInt();
  unsigned long now = millis();
  String ip = server.hasArg("ip") ? server.arg("ip") : server.client().remoteIP().toString();
  int rssi = server.hasArg("rssi") ? server.arg("rssi").toInt() : 0;

  for (int i = 0; i < FLOOR_COUNT; i++) {
    if (floors[i].id == f) {
      floors[i].online        = true;
      floors[i].prepping      = false;
      floors[i].prepared      = false;
      floors[i].startPrep     = 0;
      floors[i].startExpire   = 0;
      floors[i].lastSeen      = now;
      floors[i].lastHeartbeat = now;
      floors[i].lastIP        = ip;
      floors[i].connectionQuality = map(constrain(rssi, -90, -30), -90, -30, 0, 100);
      Serial.println("Kat " + String(f) + " yeniden bağlandı (RSSI=" + rssi + ").");
      break;
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleStatus() {
  unsigned long now = millis();
  String json = "[";

  for (int i = 0; i < FLOOR_COUNT; i++) {
    Floor &fl = floors[i];
    unsigned long rem = 0;
    if (fl.prepping)  rem = (PREP_MS - (now - fl.startPrep)) / 1000;
    else if (fl.prepared) rem = (EXPIRE_MS - (now - fl.startExpire)) / 1000;

    const char* state = fl.prepping ? "preparing"
                        : fl.prepared ? "ready"
                        : fl.online   ? "waiting"
                                      : "expired";

    unsigned long lastSeenSec = fl.lastSeen ? (now - fl.lastSeen)/1000 : 0;

    json += String("{\"id\":") + fl.id +
            ",\"state\":\""   + state +
            "\",\"rem\":"     + rem +
            ",\"online\":"    + (fl.online?"true":"false") +
            ",\"ip\":\""      + fl.lastIP +
            "\",\"lastSeen\":" + lastSeenSec +
            ",\"quality\":"   + fl.connectionQuality +
            ",\"totalReq\":"  + totalRequests +
            ",\"totalHB\":"   + totalHeartbeats + "}";
    if (i < FLOOR_COUNT-1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleInfo() {
  unsigned long uptimeSec = (millis() - systemStartTime)/1000;
  int onlineCount = 0;
  for (int i = 0; i < FLOOR_COUNT; i++)
    if (floors[i].online) onlineCount++;

  String json = "{";
  json += "\"uptime\":"       + String(uptimeSec) +
          ",\"onlineFloors\":"+ String(onlineCount) +
          ",\"totalFloors\":" + String(FLOOR_COUNT) +
          ",\"totalReq\":"    + String(totalRequests) +
          ",\"totalHB\":"     + String(totalHeartbeats) +
          ",\"freeHeap\":"    + String(ESP.getFreeHeap()) +
          ",\"wifiRSSI\":"    + String(WiFi.RSSI()) +
          "}";
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Sayfa bulunamadı");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  systemStartTime = millis();
  Serial.println("\n=== Çay Takip Başlatılıyor ===");

  // Buzzer & Live LED
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(LIVE_LED_PIN, OUTPUT);
  digitalWrite(LIVE_LED_PIN, HIGH); // OFF

  // Wi-Fi: FIXED IP CONFIGURATION
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("⚠ Statik IP ayarlanamadı!");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Wi‑Fi bağlanıyor (statik: ");
  Serial.print(local_IP);
  Serial.println(") …");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n✓ Wi‑Fi OK. http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ Bağlantı başarısız, yeniden başlatılıyor");
    delay(1000);
    ESP.restart();
  }

  // mDNS
  MDNS.begin("tea-system");
  MDNS.addService("http","tcp",80);

  // HTTP handler'lar
  server.on("/",         handleRoot);
  server.on("/start",    handleRequest);
  server.on("/request",  handleRequest);
  server.on("/heartbeat",handleHeartbeat);
  server.on("/connect",  handleConnect);
  server.on("/status",   handleStatus);
  server.on("/info",     handleInfo);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("✓ Web sunucu başladı");

  // Initial floor status
  for (int i = 0; i < FLOOR_COUNT; i++) {
    floors[i].online        = false;
    floors[i].lastSeen      = 0;
    floors[i].lastHeartbeat = 0;
    floors[i].connectionQuality = 0;
  }
  Serial.println("=== Sistem Hazır ===");
}

void loop() {
  unsigned long now = millis();
  server.handleClient();
  MDNS.update();

  handleBuzzer();

  tickFloors();

  // Server LED blink
  if (WiFi.status() == WL_CONNECTED) {
    if (now - lastLiveBlink >= LIVE_BLINK_INTERVAL) {
      lastLiveBlink = now;
      liveLedState = !liveLedState;
      digitalWrite(LIVE_LED_PIN, liveLedState ? LOW : HIGH);
    }
  } else {
    digitalWrite(LIVE_LED_PIN, HIGH);
    liveLedState = false;
  }

  yield();
}