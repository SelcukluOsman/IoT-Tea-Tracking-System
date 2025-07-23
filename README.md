T Tea Tracking System (Çay Takip Sistemi)

Çayın **demleme süresini (20 dakika)** ve **tazelik süresini (120 dakika)** takip eden bir IoT sistemidir. Sistem; 1 adet **Mother (ESP8266)** ve birden fazla **Floor (ESP32-PICO-D4)** modülünden oluşur.

> Sistem MQTT kullanmaz. Tüm haberleşme yerel ağ üzerinden HTTP protokolü ile yapılır.  
> Web arayüzü tek bir IP üzerinden çalışır ve kullanıcı, aynı ağa bağlı olduğu sürece bu arayüzden tüm çay durumlarını canlı olarak görebilir.  
> Sayaçlar web tarafında JavaScript ile işler. Floor modülleri batarya ile çalıştığı için çoğu zaman deep sleep modundadır, sadece olay olduğunda uyanır.

---

## Sistem Mimarisi

- **Mother (ESP8266)**
  - ESP32 modüllerinden gelen HTTP isteklerini alır
  - Durumları kaydeder
  - Yerel web sunucusu sağlar (`http://10.0.0.77` gibi)
  - Sayaçlar web arayüzü üzerinden yönetilir (JS)

- **Floor (ESP32-PICO-D4)**
  - Katlara yerleştirilen butonlu, bataryalı modüllerdir
  - Butona basıldığında kısa süreliğine uyanır, Mother’a HTTP isteği gönderir (örneğin: `GET /api/brew_start?floor=2`)
  - İşini bitirince deep sleep moduna geçerek enerji tasarrufu sağlar

---

## Özellikler

- 3 kat desteği (isteğe göre artırılabilir)
- Kat bazlı bağımsız durumlar:
  - Bağlı Değil
  - Çay Demleniyor (20 dakika geri sayım)
  - Çay İçilmeye Hazır (120 dakika geri sayım)
  - Süresi Doldu
- Geri sayım işlemleri web arayüzünde tarayıcı tarafında yapılır
- Düşük güç tüketimi sayesinde uzun batarya ömrü
- Kullanıcı dostu ve mobil uyumlu arayüz

---

## HTTP API Örnekleri

GET /api/status → Tüm katların anlık durumu (JSON)
GET /api/brew_start?floor=1 → 1. kat için demleme başlat
GET /api/reset?floor=3 → 3. kat sayaçlarını sıfırla

JSON örneği:
```json
{
  "floors": [
    { "id": 1, "state": "brewing", "brewRemaining": 984, "freshRemaining": 0 },
    { "id": 2, "state": "ready",   "brewRemaining": 0,   "freshRemaining": 6500 },
    { "id": 3, "state": "offline", "brewRemaining": 0,   "freshRemaining": 0 }
  ]
}
Ekran Görüntüleri
Mobil Arayüz
<p float="left"> <img src="images/Phone-NoConnect.png" width="240" alt="Mobil - Bağlı Değil"> <img src="images/Phone-Brewing.png" width="240" alt="Mobil - Çay Demleniyor"> <img src="images/Phone-ReadyforDrink.png" width="240" alt="Mobil - Çay İçilmeye Hazır"> </p>
Web Arayüzü
<p float="left"> <img src="images/web1.png" width="380" alt="Web - Bağlı Değil"> <img src="images/web2.png" width="380" alt="Web - Çay Demleniyor"> <img src="images/web3.png" width="380" alt="Web - Çay İçilmeye Hazır"> </p> ```
