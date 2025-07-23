# IoT Tea Tracking System (Çay Takip Sistemi)

Çayın **demleme süresini (20 dk)** ve **tazelik süresini (120 dk)** takip eden;  
1 adet **Mother (ESP8266)** ve birden fazla **Floor (ESP32-PICO-D4)** modülünden oluşan IoT sistemi.

---

## Mimari

- **Mother (ESP8266)**  
  - Kat modüllerinden gelen verileri toplar.  
  - Zamanlayıcıları ve durum değişimlerini yönetir.  
  - Ağ üzerinden (Wi‑Fi / MQTT / HTTP …) dış dünyaya veri/servis sağlar.  
- **Floor (ESP32-PICO-D4)**  
  - Her katta çay demleme ve tazelik durumunu izler.  
  - Mother’a periyodik olarak veya olay bazlı (demleme başlatıldı/bitti vb.) veri gönderir.

> Sistem 3 katlıdır (F1, F2, F3). Değişkense README’de belirt.

---

## Özellikler

- Demleme süresi: **20 dakika** geri sayım  
- Tazelik süresi: **120 dakika** geri sayım  
- Süre bitiminde uyarı / status değişimi  
- Kat bazında ayrı sayaçlar  
- Ağ üzerinden durum izleme (dashboard / log / API) *(varsa belirt)*  
- Reset / yeniden yapılandırma akışları

---

## Proje Yapısı

