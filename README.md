# IoT Tea Tracking System (Çay Takip Sistemi)

Çayın **demleme süresini (20 dk)** ve **tazelik süresini (120 dk)** takip eden; 3 katlı yapıya sahip, OLED ekranlı ve IoT özellikli bir sistem.

## Özellikler
- Demleme başlatma ve geri sayım
- Tazelik süresi takibi, uyarılar
- Wi-Fi/Bluetooth üzerinden durum görüntüleme (varsa ekle)
- Kat bazında durum raporu (3 kat)
- Loglama / veri paylaşımı (varsa)

## Donanım
- MCU: (STM32/ESP32/…)
- Sensörler / Ekran: (OLED I2C, buton pinleri, vb.)
- Bağlantı: (Wi-Fi, BLE, Ethernet…)

## Yazılım
- Dil / Framework: (C, C++, Arduino, FreeRTOS, Python backend vs.)
- Ana modüller:
  - `src/` – kaynak kodlar
  - `include/` – header dosyaları
  - `hardware/` – şema, PCB, BOM
  - `docs/` – dokümantasyon, diyagramlar
  - `examples/` – örnek kullanım

## Kurulum
```bash
git clone https://github.com/SelcukluOsman/iot-tea-tracking-system.git
cd iot-tea-tracking-system
# Gerekli bağımlılıkları kur, derle, vs.
