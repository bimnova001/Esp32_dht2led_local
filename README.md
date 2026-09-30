# 🌡️ ESP32 DHT + 2×LED — Offline Dashboard

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-000?logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?logo=arduino&logoColor=white)
![Sensor](https://img.shields.io/badge/Sensor-DHT11-ef4444)
![Offline](https://img.shields.io/badge/Offline-AP%20Mode-22c55e)
![AI](https://img.shields.io/badge/UI%2FCode-Assisted%20by%20AI-8b5cf6)

> 🤖 **Disclosure / หมายเหตุ:** โปรเจกต์นี้เขียนโดย **AI (Muse Spark)** ร่วมกับเจ้าของ repo —
> ทั้งโค้ด `w8.ino` และไฟล์ README นี้ AI เป็นคนร่าง/ออกแบบให้ โปรดตรวจสอบก่อนใช้งานจริง

แดชบอร์ดควบคุม ESP32 แบบ **Offline 100%** — ESP32 ปล่อย Wi-Fi AP ของตัวเอง
แล้ว serve หน้าเว็บที่มี **เกจหน้าปัดอุณหภูมิ/ความชื้น** + **สวิตช์ไฟ LED 2 ดวง**
เปิดจากมือถือหรือคอมได้เลย ไม่ต้องมีเน็ต ไม่ต้องมี server เพิ่ม

---

## ✨ Features

| ฟีเจอร์ | รายละเอียด |
|---|---|
| 🎛️ เกจหน้าปัด SVG | อุณหภูมิ (0–50 °C) + ความชื้น (0–100 %) พร้อมเข็ม, gradient, badge สถานะ (เย็น/สบาย/อุ่น/ร้อน) |
| 💡 LED 2 ดวง | การ์ดแยกกัน มี glow ตอนเปิด + ป้าย เปิด/ปิด + ปุ่มใหญ่ 52px |
| 📡 AP Mode | SSID `ESP32_Config_AP` / รหัส `123456789` → เปิด `192.168.4.1` |
| 🔌 Offline ล้วน | ไม่มี CDN / Google Fonts / library ภายนอก — HTML+CSS+JS inline ทั้งหมด (~17KB) |
| 📱 Responsive | คอม + มือถือ (grid ยุบเป็นคอลัมน์เดียวบนจอเล็ก, touch target ≥ 44px) |
| ♿ Accessible | `aria-pressed`/`aria-live`, focus ring, รองรับ `prefers-reduced-motion` |

---

## 🧰 Hardware

- ESP32 (WROOM-32 หรือเทียบเท่า)
- DHT11 (data → GPIO 4)
- LED × 2 + R 220Ω × 2

### 🔌 Wiring

| อุปกรณ์ | ขา | ต่อเข้า ESP32 |
|---|---|---|
| DHT11 VCC | + | 3V3 |
| DHT11 GND | − | GND |
| DHT11 DATA | S | **GPIO 4** |
| LED 1 (+) | + | **GPIO 5** ผ่าน R 220Ω |
| LED 1 (−) | − | GND |
| LED 2 (+) | + | **GPIO 18** ผ่าน R 220Ω |
| LED 2 (−) | − | GND |

---

## 🚀 วิธีใช้งาน

### 1. Flash firmware

1. เปิด `w8/w8.ino` ด้วย **Arduino IDE**
2. ติดตั้ง Library: `DHT sensor library` (Adafruit) + `Adafruit Unified Sensor`
3. เลือกบอร์ด `ESP32 Dev Module` →เลือก COM Port → **Upload**
4. เปิด Serial Monitor (115200) จะเห็น `AP IP Address: 192.168.4.1`

### 2. เปิดแดชบอร์ด

1. เอามือถือ/คอมต่อ Wi-Fi ชื่อ **`ESP32_Config_AP`** (รหัส `123456789`)
2. เปิดเบราว์เซอร์ไปที่ **`http://192.168.4.1`**
3. ดูค่าเกจ (รีเฟรชอัตโนมัติทุก 2 วินาที) + กดปุ่มเปิด/ปิด LED ได้เลย

---

## 🔗 API Endpoints

| Method | Path | ผลลัพธ์ |
|---|---|---|
| `GET` | `/` | หน้า Dashboard (HTML) |
| `GET` | `/data` | JSON `{temperature, humidity, led1, led2}` |
| `GET` | `/led1/on`, `/led1/off` | เปิด/ปิด LED 1 (GPIO 5) |
| `GET` | `/led2/on`, `/led2/off` | เปิด/ปิด LED 2 (GPIO 18) |
| `GET` | `/on`, `/off` | alias เดิม → LED 1 (backward compatible) |

ตัวอย่าง:

```json
GET /data
{"temperature":29.5,"humidity":62.0,"led1":1,"led2":0,"led":1}
```

---

## 📁 Project Structure

```text
w8/
├── w8.ino      # โค้ดทั้งหมด (firmware + หน้าเว็บ inline ใน PROGMEM)
└── README.md   # ไฟล์นี้
```

---

## 🎨 Design Notes

- ธีม **dark dashboard** (เหมาะกับงาน IoT): การ์ด gradient, ข้อความ contrast สูง
- ฟอนต์ใช้ **system stack** เพื่อให้ offline ได้ (`-apple-system, Segoe UI, Roboto, Noto Sans Thai…`)
- อนิเมชันจำกัด `transform/opacity` 150–300ms (ตามแนวทาง Apple HIG / Material)
- หน้าเว็บเก็บใน `PROGMEM` + ส่งด้วย `send_P` เพื่อประหยัด RAM

---

## 📝 Roadmap / TODO

- [ ] เพิ่มกราฟประวัติอุณหภูมิ (เก็บใน SPIFFS/LittleFS)
- [ ] โหมด STA (ต่อ Wi-Fi บ้าน) สลับกับ AP
- [ ] OTA update

---

## 📄 License

MIT — ใช้ได้อิสระ ปรับแก้ได้ตามสบาย

## 🙏 Credits

- เจ้าของโปรเจกต์: [@bimnova001](https://github.com/bimnova001)
- โค้ด + README ร่างโดย **AI (Muse Spark)** — *reviewed by human before use* ✅
