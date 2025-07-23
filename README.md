# 🫖 IoT Tea Tracking System (Çay Takip Sistemi)

<div align="center">

![Tea Tracking System Logo](images/mobile-demo-1.jpg)

**Çayın demleme ve tazelik süresini akıllı takip eden IoT sistemi**

[![ESP8266](https://img.shields.io/badge/ESP8266-Mother-blue)](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System)
[![ESP32](https://img.shields.io/badge/ESP32--PICO--D4-Floor-green)](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System)
[![Web Dashboard](https://img.shields.io/badge/Web-Dashboard-orange)](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System)

</div>

## 📱 Canlı Demo Görüntüleri

### 📱 Mobil Arayüz
<div align="center">
<table>
<tr>
<td align="center">
<img src="images/Phone-NoConnect.png" width="200" alt="Bekleme Durumu"/>
<br/><b>Bekleme Durumu</b>
</td>
<td align="center">
<img src="images/Phone-Brewing.png" width="200" alt="Demleme Süreci"/>
<br/><b>Demleme Süreci</b>
</td>
<td align="center">
<img src="images/Phone-ReadyforDrink.png" width="200" alt="Tazelik Takibi"/>
<br/><b>Tazelik Takibi</b>
</td>
</tr>
</table>
</div>

### 💻 Web Dashboard
<div align="center">
<table>
<tr>
<td align="center">
<img src="images/web1.png" width="600" alt="Bekleme Durumu"/>
<br/><b>Bekleme Durumu</b>
</td>
<td align="center">
<img src="images/web2.png" width="600" alt="Demleme Süreci"/>
<br/><b>Demleme Süreci</b>
</td>
<td align="center">
<img src="images/web3.png" width="600" alt="Tazelik Takibi"/>
<br/><b>Tazelik Takibi</b>
</td>
</tr>
</table>
</div>

---

## 📁 Dosya Yapısı

Bu proje aşağıdaki dosya yapısına sahiptir:

```
IoT-Tea-Tracking-System/
├── 📁 images/                     # Demo görüntüleri ve UI screenshots
│   ├── mobile-demo-1.jpg
│   ├── Phone-NoConnect.png
│   ├── Phone-Brewing.png
│   ├── Phone-ReadyforDrink.png
│   ├── web1.png
│   ├── web2.png
│   └── web3.png
│
├── 📁 docs/                       # Dokümantasyon ve kod dosyaları
│   ├── M-ESP8266.ino             # Mother modül kodu (Ana sunucu)
│   ├── F-ESP32-PICO-D4.ino       # Floor modül kodu (Buton modülleri)
│   └── system-architecture.md     # Sistem mimarisi detayları
│
├── 📁 video/                      # Demo videoları
│   └── video/                     # Sistem çalışma videoları
│       └── system-demo.mp4        # Tam sistem demo videosu
│
├── 📄 README.md                   # Bu dosya
├── 📄 LICENSE                     # Lisans bilgileri
└── 📄 CHANGELOG.md               # Versiyon geçmişi
```

---

## 🏗️ Sistem Mimarisi

Bu proje, **1 adet Mother (ESP8266)** ve **birden fazla Floor (ESP32-PICO-D4)** modülünden oluşan dağıtık IoT sistemidir.

### 🧠 Mother Modül (ESP8266)
- **Sabit IP Server**: `http://10.0.0.77` üzerinde web dashboard servisi
- **Heartbeat Interval**: 180 saniye (Floor → Mother)
- **Smart Timer Management**: 20dk demleme → 120dk tazelik otomatik geçişi
- **Advanced Buzzer System**: Başlangıç ve hazır melodileri (GPIO13)
- **Live Status LED**: Server durumu için GPIO4 LED yanıp sönme
- **Overshoot Protection**: Gecikmiş sorgularda süre kaybı önleme
- **Connection Quality**: RSSI tabanlı sinyal kalitesi hesaplama

#### 📂 Mother Modül Kod Dosyası: `docs/M-ESP8266.ino`
```cpp
// ESP8266 Mother Modül - Ana Sunucu
// Network: Static IP 10.0.0.77
// Ports: HTTP:80, mDNS:tea-system.local
// Features: Web Dashboard, Timer Management, Buzzer Control
```

### 📍 Floor Modülleri (ESP32-PICO-D4)
- **Ultra Low Power Design**: 400mAh batarya ile 2-4 hafta kullanım
- **Deep Sleep Optimization**: 55 saniye uyku, 5 saniye aktif döngü  
- **Smart Button Detection**: GPIO25 ile hassas buton algılama
- **NeoPixel Feedback**: Tek LED ile renkli durum bildirimi
- **Minimal Network Usage**: Sadece buton basımı ve heartbeat gönderimi
- **Battery Management**: Ultra düşük güç tüketimi (80MHz CPU)
- **Quick Response**: 2 saniye WiFi timeout ile hızlı bağlantı

#### 📂 Floor Modül Kod Dosyası: `docs/F-ESP32-PICO-D4.ino`
```cpp
// ESP32-PICO-D4 Floor Modül - Buton Kontrol
// Power: 400mAh Battery, Deep Sleep Mode
// Network: WiFi Client → Mother 10.0.0.77
// Features: Button Detection, NeoPixel Status, Ultra Low Power
```

---

## 🎬 Video Demonstrasyon

### 📹 Sistem Çalışma Videosu
Projenin tam çalışma videosunu `video/video/` klasöründe bulabilirsiniz. Video şunları göstermektedir:

- **System Architecture**: Mother-Floor modül iletişimi
- **Web Dashboard**: Gerçek zamanlı durum takibi
- **Mobile Interface**: Responsive tasarım özellikleri
- **Button Interaction**: Floor modüllerinden çay başlatma
- **Timer Management**: Demleme ve tazelik süreçleri
- **Status Indicators**: LED ve buzzer geri bildirimleri
- **Power Management**: Deep sleep ve batarya optimizasyonu

> 💡 **Not**: Video dosyası sistem mimarisini ve akış şemasını net bir şekilde göstermektedir. Flow chart'lara gerek kalmadan tüm sistem davranışını anlayabilirsiniz.

---

## ⚡ Temel Özellikler

### ⏱️ Zamanlama Sistemi
- **Demleme Süresi**: 20 dakika hassas geri sayım
- **Tazelik Süresi**: 120 dakika taze kalma takibi
- **Visual Progress**: Gerçek zamanlı ilerleme çubukları
- **Multi-Status**: Her kat için ayrı durum yönetimi

### 🔊 Gelişmiş Buzzer Sistemi
- **Başlangıç Jingle**: C → E → G (523→659→784 Hz) yükselen mutlu melodi
- **Hazır Melodisi**: G → E → C → C8 (784→659→523→1046 Hz) kutlama jingle'ı
- **Smart Timing**: Ton aralarında sessizlik ile net müzik efekti
- **Non-blocking**: Buzzer çalarken sistem diğer işlevleri sürdürür

### 📡 Network API Endpoints
- **`/start`** veya **`/request`**: Çay demleme başlatma
- **`/heartbeat`**: Kat modüllerinden yaşam sinyali
- **`/connect`**: Yeni kat modülü bağlantısı
- **`/status`**: JSON formatında tüm katların durumu
- **`/info`**: Sistem bilgileri ve istatistikler

### 🌐 Web Arayüzü
- **Responsive Design**: Mobil ve desktop uyumlu
- **Modern UI/UX**: Gradient renkler ve smooth animasyonlar
- **Multi-Floor Support**: 3 kata kadar eş zamanlı takip
- **Status Indicators**: Renk kodlu durum göstergeleri

---

## 🚀 Çalışma Prensibi

### 🔄 Sistem Akışı
1. **Floor modülleri** deep sleep modunda bekler (55s uyku)
2. **Buton basımı** ile ESP32 uyanır ve Mother'a istek gönderir
3. **Mother modül** timer başlatır ve buzzer çalar
4. **Web dashboard** gerçek zamanlı süreyi gösterir
5. **20 dakika** sonra demleme tamamlanır (hazır melodisi)
6. **120 dakika** tazelik takibi başlar
7. **Heartbeat sinyalleri** ile bağlantı kontrolü yapılır

### 📊 Durum Matrisi
| Durum | Renk | Süre | Açıklama |
|-------|------|------|----------|
| **Standby** | ⚫ Gri | - | Bekleme durumu |
| **Brewing** | 🟠 Turuncu | 20dk | Aktif demleme |
| **Ready** | 🟢 Yeşil | - | Tüketim zamanı |
| **Fresh** | 🔵 Mavi | 120dk | Tazelik takibi |
| **Expired** | 🔴 Kırmızı | - | Tazelik süresi bitti |

---

## 🎯 Kullanım Senaryoları

### 🏢 Ofis Ortamı
- Çoklu katlarda çay demleme takibi
- Ekip üyeleri için ortak çay zamanı koordinasyonu
- Çay israfının önlenmesi

### 🏠 Ev Kullanımı  
- Aile üyeleri için kişisel çay takibi
- Misafir ağırlama durumlarında çay hazırlığı
- Çay kalitesinin korunması

### 🏭 Endüstriyel Kullanım
- Fabrika vardiya molalarında toplu çay servisi
- Büyük ofis binalarında kat bazlı çay yönetimi
- Enerji tasarrufu ve verimlilik artışı

---

## 🔧 Teknik Detaylar

### Hardware Gereksinimleri
- **1x ESP8266** (Mother modül için)
- **Nx ESP32-PICO-D4** (Floor modülleri için)
- **Butonlar** (GPIO25 - Her kat için başlatma butonu)
- **NeoPixel LED** (GPIO4 - Görsel geri bildirim için)
- **Buzzer** (Mother modülde ses çıkışı için)  
- **400mAh Battery** (Floor modülleri için taşınabilir güç)
- **Power Supply** (Mother modül için sabit güç kaynağı)

### Software Stack
- **Arduino IDE** ile geliştirme
- **ESP8266WiFi** kütüphanesi (Mother)
- **ESP8266WebServer** kütüphanesi (HTTP server)
- **ESP8266mDNS** kütüphanesi (domain çözümleme)
- **WiFi.h** kütüphanesi (Floor ESP32)
- **HTTPClient** kütüphanesi (HTTP iletişimi)
- **Adafruit_NeoPixel** (LED kontrol)
- **esp_sleep.h** (Ultra low power deep sleep)
- **tone()** fonksiyonu (Buzzer melodileri)
- **Custom Timer & State Machine** implementasyonu

### Network Konfigürasyonu
- **WiFi Bağlantısı**: Tüm modüller aynı network
- **Static IP**: Mother modül için sabit IP (örn: 10.0.0.77)
- **Port Management**: HTTP trafiği için özel port
- **Local Access**: Harici internet gereksiz

---

## ⚙️ Kurulum ve Konfigürasyon

### 1. Hardware Bağlantıları
```
Mother ESP8266:
├── Buzzer → GPIO13 (Start/Ready Jingle'lar)
├── Live LED → GPIO4 (Server Status - LOW=ON)
├── WiFi Anteni
└── Power Input (5V/3.3V)

Floor ESP32-PICO-D4:
├── Başlatma Butonu → GPIO25 (Pull-down)
├── NeoPixel LED → GPIO4 (Data)
├── NeoPixel Enable → GPIO5 (Power Control)
├── WiFi Anteni  
└── 400mAh Battery Input
```

### 2. Software Yükleme
```bash
# Arduino IDE'de gerekli kütüphaneleri yükleyin
# docs/M-ESP8266.ino dosyasını Mother modüle yükleyin
# docs/F-ESP32-PICO-D4.ino dosyasını Floor modüllerine yükleyin
# WiFi credentials ve IP ayarlarını yapılandırın
```

### 3. Network Ayarları
```cpp
// Mother modül için (docs/M-ESP8266.ino)
const char* SSID = "YOUR_WIFI_SSID";
const char* PASSWORD = "YOUR_WIFI_PASSWORD";

// Sabit IP Konfigürasyonu
IPAddress local_IP(10,0,0,77);       // Ana server IP
IPAddress gateway (10,0,0,1);        // Router IP
IPAddress subnet  (255,255,255,0);   // Subnet mask
IPAddress dns     (8,8,8,8);         // DNS server

// Zamanlayıcı Ayarları
const unsigned long PREP_MS = 20UL * 60UL * 1000UL;    // 20 dakika
const unsigned long EXPIRE_MS = 120UL * 60UL * 1000UL; // 120 dakika
const unsigned long HEARTBEAT_TIMEOUT = 180000UL;      // 180 saniye

// Floor modülleri için (docs/F-ESP32-PICO-D4.ino)
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* MAIN_HOST = "10.0.0.77";
const uint16_t MAIN_PORT = 80;
#define FLOOR_ID 3  // Her kat için farklı ID

// Ultra Low Power Ayarları
const unsigned long SLEEP_DURATION_US = 55000000UL; // 55s uyku
const unsigned long HB_INTERVAL_MS = 60000UL;       // 60s heartbeat
```

---

## 🎨 Arayüz Özellikleri

### Renk Kodları
- **🟠 Turuncu**: Aktif demleme süreci
- **🟢 Yeşil**: Hazır durum / Optimal tüketim zamanı
- **🔵 Mavi**: Bekleme / Tazelik takibi
- **⚫ Gri**: İnaktif / Bağlantı yok

### Responsive Tasarım
- **Desktop**: Geniş ekran için 3 kolonlu layout
- **Tablet**: 2 kolonlu adaptif görünüm
- **Mobile**: Tek kolon dikey scroll
- **Cross-browser**: Modern tarayıcı desteği

---

## 📊 Performans ve Verimlilik

### Sistem Metrikleri
- **Response Time**: < 180ms (local network)
- **Heartbeat Interval**: 60 saniye (Floor → Mother)
- **Offline Detection**: 180 saniye timeout
- **Battery Life**: ESP32 için 2-4 hafta (400mAh batarya)
- **Deep Sleep Current**: < 10µA (ultra low power mode)
- **Active Current**: ~80mA (WiFi aktif, 2 saniye)
- **Concurrent Users**: 10+ eş zamanlı kullanıcı
- **System Uptime**: 99.9+ güvenilirlik
- **mDNS Support**: `tea-system.local` domain çözümleme

### 🔋 Ultra Low Power Özellikleri
- **Deep Sleep Mode**: 55 saniye uyku, 5 saniye aktif
- **Batarya Ömrü**: 400mAh ile 2-4 hafta kesintisiz çalışma
- **Smart Wake-up**: Buton basımı veya timer ile uyanma
- **Power Management**: Bluetooth kapalı, 80MHz CPU frekansı
- **Minimal Network**: 8 saniye WiFi timeout, maksimum güç tasarrufu

### 💡 NeoPixel LED Geri Bildirimleri
- **🟢 Yeşil**: Başarılı buton basımı ve server iletişimi
- **🔴 Kırmızı**: Bağlantı hatası veya server yanıtı yok
- **🔵 Turkuaz**: Hızlı buton onayı (network öncesi)
- **⚫ Kapalı**: Enerji tasarrufu için varsayılan durum

---

## 🔮 Gelecek Geliştirmeler

### v2.0 Planlanan Özellikler
- [ ] **Mobile App**: Native iOS/Android uygulaması
- [ ] **Cloud Integration**: Remote monitoring desteği  
- [ ] **Temperature Sensing**: Çay sıcaklığı takibi
- [ ] **User Profiles**: Kişiselleştirilmiş çay tercihleri
- [ ] **Analytics Dashboard**: Kullanım istatistikleri
- [ ] **Voice Control**: Alexa/Google Assistant entegrasyonu

### Gelişmiş Özellikler
- **Machine Learning**: Kullanım kalıpları analizi
- **Push Notifications**: Mobil bildirimler
- **Multi-Language**: Çoklu dil desteği
- **API Integration**: Third-party servis desteği

---

## 👥 Katkıda Bulunma

Bu proje açık kaynak ruhuyla geliştirilmektedir. Katkılarınızı bekliyoruz!

### Nasıl Katkıda Bulunabilirsiniz?
1. **Fork** edin
2. **Feature branch** oluşturun (`git checkout -b feature/AmazingFeature`)
3. **Commit** edin (`git commit -m 'Add some AmazingFeature'`)
4. **Push** edin (`git push origin feature/AmazingFeature`)
5. **Pull Request** açın

### 📂 Kod Yapısı
- **docs/M-ESP8266.ino**: Mother modül ana kodu
- **docs/F-ESP32-PICO-D4.ino**: Floor modül kodu
- **video/video/**: Sistem demo videoları
- **images/**: UI screenshots ve demo görselleri

---

## 📄 Lisans

Bu proje **OSS EMBEDDED SYSTEM** tarafından geliştirilmiştir.
© 2025 • Tüm Hakları Saklıdır

---

## 📞 İletişim

**Geliştirici**: Osman Selçuklu  
**GitHub**: [@SelcukluOsman](https://github.com/SelcukluOsman)  
**Proje**: [IoT-Tea-Tracking-System](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System)

---

<div align="center">

**☕ Çayınızı hiç bu kadar akıllıca takip etmediniz! ☕**

[⭐ Star verin](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System) | [🍴 Fork edin](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System/fork) | [📝 Issue açın](https://github.com/SelcukluOsman/IoT-Tea-Tracking-System/issues)

</div>
