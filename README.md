# IoT Tea Tracking System (Çay Takip Sistemi)

Çayın **demleme süresini (20 dk)** ve **tazelik süresini (120 dk)** takip eden;  
1 adet **Mother (ESP8266)** ve birden fazla **Floor (ESP32-PICO-D4)** modülünden oluşan IoT sistemi.

> **MQTT kullanılmaz.** Tüm cihazlar aynı Wi‑Fi ağı içinde yerel IP üzerinden HTTP istekleriyle haberleşir. Web arayüzü (Mother üzerinde host edilir) sayaçları tarayıcı tarafında çalıştırır; Floor modülleri batarya tasarrufu için çoğunlukla deep-sleep’tedir.

---

## Mimari

- **Mother (ESP8266)**
  - Kat modüllerinden gelen HTTP isteklerini alır ve durumları saklar.
  - Yerel ağda web sunucusu açar (örn. `http://10.0.0.77`).
  - Zamanlayıcılar/durum değişimleri web tarafında JS ile yönetilir.

- **Floor (ESP32-PICO-D4)**
  - Buton tetiklendiğinde uyanır, Mother’a olay bilgisini gönderir (örn. `/api/brew_start?floor=3`).
  - İşini bitirince tekrar deep-sleep’e geçer.

> Sistem 3 katlıdır (F1, F2, F3). Değiştirilebilir.

---

## Özellikler

- Demleme süresi: **20 dakika** geri sayım  
- Tazelik süresi: **120 dakika** geri sayım  
- Süre bitiminde durum/renk değişimi  
- Kat bazında ayrı sayaçlar ve durumlar  
- Yerel web arayüzü (mobil/desktop responsive)  
- Reset / yeniden yapılandırma akışları

---

## Proje Yapısı

iot-tea-tracking-system/
├─ src/
│ ├─ mother/
│ │ └─ M-ESP8266.ino
│ └─ floor/
│ └─ F-ESP32-PICO-D4.ino
├─ images/
│ ├─ Phone-NoConnect.png
│ ├─ Phone-Brewing.png
│ ├─ Phone-ReadyforDrink.png
│ ├─ web1.png
│ ├─ web2.png
│ └─ web3.png
├─ LICENSE
├─ README.md
└─ .gitignore

yaml
Kopyala
Düzenle

---

## İletişim (HTTP REST Örnekleri)

GET /api/status -> Tüm katların durumlarını JSON döner
GET /api/brew_start?floor=2 -> Kat 2 için demleme başlat
GET /api/reset?floor=2 -> Kat 2 sayaçlarını sıfırla

css
Kopyala
Düzenle

**JSON Örneği:**
```json
{
  "floors": [
    { "id": 1, "state": "brewing", "brewRemaining": 980,  "freshRemaining": 0    },
    { "id": 2, "state": "ready",   "brewRemaining": 0,    "freshRemaining": 7100 },
    { "id": 3, "state": "offline", "brewRemaining": 0,    "freshRemaining": 0    }
  ]
}
Kurulum
bash
Kopyala
Düzenle
git clone https://github.com/SelcukluOsman/iot-tea-tracking-system.git
cd iot-tea-tracking-system
Arduino IDE
ESP8266 & ESP32 kart paketlerini kur.

Kütüphaneler: ESP8266WiFi / WiFi.h, ESP8266WebServer / WebServer.h, ArduinoJson vb.

src/mother/M-ESP8266.ino dosyasını ESP8266’ya,
src/floor/F-ESP32-PICO-D4.ino dosyasını ESP32’ye yükle.

PlatformIO (opsiyonel)
platformio.ini içinde board ve lib’leri tanımlayarak pio run -t upload.

Kullanım
Mother açılır → IP alır (sabit IP önerilir).

Floor butonuna bas → /api/brew_start?floor=X isteği gönderilir → sleep.

Tarayıcıdan http://<mother-ip> adresine gir → sayaçlar web’de işler, durumları gör.

Ekran Görüntüleri
Mobil ve web arayüzünde üç ana durum: Bağlı Değil, Çay Demleniyor, Çay İçilmeye Hazır.

Mobil Arayüz
<p float="left"> <img src="images/Phone-NoConnect.png" width="240" alt="Mobil - Bağlı Değil"> <img src="images/Phone-Brewing.png" width="240" alt="Mobil - Çay Demleniyor"> <img src="images/Phone-ReadyforDrink.png" width="240" alt="Mobil - Çay İçilmeye Hazır"> </p>
Web Arayüzü
<p float="left"> <img src="images/web1.png" width="380" alt="Web - Bağlı Değil"> <img src="images/web2.png" width="380" alt="Web - Çay Demleniyor"> <img src="images/web3.png" width="380" alt="Web - Çay İçilmeye Hazır"> </p>
Yol Haritası
 OTA güncelleme

 Basit loglama (SPIFFS/SD)

 UI geliştirmeleri / grafikler

Katkı
PR ve issue’lara açığım. Lütfen önce mevcut issue’ları kontrol edin.
