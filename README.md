[![Project Banner](https://github.com/SelcukluOsman/SelcukluOsman/blob/main/assets/masterhead.jpg?raw=true)](https://www.linkedin.com/in/selcukselcuklu)

<h1 align="center">🍵 IoT Tea Tracking System</h1>
<h3 align="center">3 katlı, düşük güç tüketimli, lokal ağ üzerinden çalışan gerçek zamanlı çay takip sistemi.</h3>

<p align="center">
  <img src="https://readme-typing-svg.herokuapp.com?font=Fira+Code&size=24&pause=1000&center=true&vCenter=true&width=1000&lines=Demleme+Süresi:+20+dakika;Tazelik+Süresi:+120+dakika;ESP8266+%2B+ESP32-PICO-D4;Yerel+Web+Arayüzü+%2F+API;Batarya+Dostu+Deep+Sleep+Yapısı" alt="Typing SVG" />
</p>

---

## 🔧 Sistem Özeti

Bu sistem; ESP8266 tabanlı **Mother** modül ile 3 farklı **katta (Floor)** yer alan ESP32-PICO-D4 cihazlarının haberleşerek  
**çayın ne zaman demlendiğini** ve **tazeliğinin ne zaman biteceğini** takip etmesini sağlar.

> MQTT veya harici sunucu kullanılmaz. Tüm işlemler yerel ağda, doğrudan IP erişimi ile gerçekleşir.  
> Sayaç işlemleri tarayıcı tarafında yapılır, bu sayede batarya ile çalışan kat modülleri enerji tasarruflu şekilde çalışır.

---

## 🧠 Mimari

[Floor-ESP32] ---
[Floor-ESP32] ----> [Mother ESP8266] → Web Arayüz (JS sayaç)
[Floor-ESP32] ---/


- **Mother (ESP8266):**
  - HTTP sunucu açar (örn. `http://10.0.0.77`)
  - ESP32 modüllerinden gelen istekleri alır
  - Katlara ait durumları tutar ve tarayıcıya servis eder
- **Floor (ESP32-PICO-D4):**
  - Butonla uyanır, demleme başlat sinyali gönderir
  - Ardından deep sleep'e geçer

---

## 🧪 Özellikler

- ⏱️ Demleme süresi: **20 dakika**  
- 🧊 Tazelik süresi: **120 dakika**  
- 🔋 **Batarya dostu** ESP32 Floor modülleri (deep sleep)  
- 📱 Mobil ve masaüstü web arayüzü  
- 🌐 Yerel IP üzerinden arayüz ve API erişimi  
- 🔁 HTTP REST API üzerinden kontrol  
- 🔧 Genişletilebilir ve açık kaynak

---

## 🔌 API Örnekleri

| Endpoint                    | Açıklama                       |
|----------------------------|--------------------------------|
| `/api/status`              | Tüm katların son durumunu döner |
| `/api/brew_start?floor=2`  | Kat 2 için demlemeyi başlatır  |
| `/api/reset?floor=1`       | Kat 1 durumunu sıfırlar         |

JSON örneği:
```json
{
  "floors": [
    { "id": 1, "state": "brewing", "brewRemaining": 900, "freshRemaining": 0 },
    { "id": 2, "state": "ready", "brewRemaining": 0, "freshRemaining": 6400 },
    { "id": 3, "state": "offline", "brewRemaining": 0, "freshRemaining": 0 }
  ]
}

Mobil Arayüz
<p align="center"> <img src="images/Phone-NoConnect.png" width="220"> <img src="images/Phone-Brewing.png" width="220"> <img src="images/Phone-ReadyforDrink.png" width="220"> </p>
Web Arayüzü
<p align="center"> <img src="images/web1.png" width="300"> <img src="images/web2.png" width="300"> <img src="images/web3.png" width="300"> </p>


🚀 Kurulum
Arduino IDE veya PlatformIO aç

Aşağıdaki dosyaları ilgili kartlara yükle:

src/mother/M-ESP8266.ino

src/floor/F-ESP32-PICO-D4.ino

Wi-Fi ayarlarını .ino içinde düzenle

ESP8266 IP'sine bağlan: http://<ip-adresi>




