<p align="center">
  <img src="logo.svg" width="140" alt="ESP32 DHT + 2xLED logo">
</p>

# Esp32_dht2led_local

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-000?logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?logo=arduino&logoColor=white)
![Sensor](https://img.shields.io/badge/Sensor-DHT11-ef4444)
![Offline](https://img.shields.io/badge/Offline-AP%20Mode-22c55e)

ESP32 ปล่อย Wi-Fi AP ของตัวเอง แล้ว serve หน้าเว็บ offline 100% สำหรับดูค่าอุณหภูมิ/ความชื้น (เกจหน้าปัด SVG) และเปิด–ปิด LED 2 ดวง จากมือถือหรือคอม ไม่ต้องมีเน็ต

## Hardware

- ESP32 + DHT11 (DATA → **GPIO 4**) + LED × 2 (→ **GPIO 5**, **GPIO 18** ผ่าน R 220Ω ลง GND)

## Usage

1. เปิด `w8/w8.ino` ใน Arduino IDE (ลง lib `DHT sensor library` + `Adafruit Unified Sensor`), เลือกบอร์ด `ESP32 Dev Module` แล้ว Upload
2. ต่อ Wi-Fi `ESP32_Config_AP` (รหัส `123456789`) → เปิด `http://192.168.4.1`

## API

| Path | ผลลัพธ์ |
|---|---|
| `GET /` | หน้า Dashboard |
| `GET /data` | JSON `{temperature, humidity, led1, led2}` |
| `GET /led1/on`, `/led1/off` | LED 1 (GPIO 5) |
| `GET /led2/on`, `/led2/off` | LED 2 (GPIO 18) |

## License

MIT — by [@bimnova001](https://github.com/bimnova001)
