# Motor & Door Control via Telegram Bot (ESP8266)

Control a motor and door actuator using an ESP8266 and Telegram with a user-friendly inline keyboard.

---

## 🔧 Features

* **/start**: Displays an inline keyboard featuring:

  * OFF, timed motor options (3, 5, 10, 20 mins, or *Fill the empty Tank* = 35 mins)
  * *Open The Door*
* Sends confirmation messages (“Motor is OFF ❌”, “Motor set to XX mins ✅”, etc.)
* Auto-turnoff the motor after the specified duration
* Maintains Wi‑Fi and uses a status LED
* Secure HTTPS interaction via `WiFiClientSecure`

---

## 🧩 Hardware Requirements

* **ESP8266** board
* Motor connected to `MOTOR_PIN` (no power circuitry included)
* Door actuator on `DOOR_PIN`
* Wi‑Fi status LED on `WIFI_LED_PIN`

All mapped in `auth.h`:

```
#define WIFI_LED_PIN  D3
#define MOTOR_PIN     D2
#define DOOR_PIN      D1
```

---

## 📁 Software Setup

### 1. Required Libraries

* `ESP8266WiFi` & `WiFiClientSecure`
* [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot) ([stackoverflow.com][1], [github.com][2], [github.com][3])
* `ArduinoJson` (v6)
* `auth.h` (below)

### 2. auth.h

```
#ifndef AUTH_H
#define AUTH_H
#define motor_bot_token ""  // Telegram token from BotFather
#define ssid             ""  // Wi‑Fi SSID
#define password         ""  // Wi‑Fi password
#define WIFI_LED_PIN D3
#define MOTOR_PIN     D2
#define DOOR_PIN      D1
#endif
```

Insert your credentials and token before compiling.

### 3. Wiring

* `WIFI_LED_PIN` → indicator LED
* `MOTOR_PIN` → motor control (e.g., via relay)
* `DOOR_PIN` → door actuator  (e.g., via relay)
  

---

## ⚙️ Usage

1. Upload the sketch to ESP8266, open Serial Monitor (9600 baud).
2. The board connects to Wi‑Fi, displays its IP, and polls Telegram (every second).
3. In Telegram, send **/start** to begin.
4. Choose a command:

   * **OFF**: stops motor
   * **3/5/10/20 Mins**, **Fill the empty Tank**: starts motor, shuts off after timeout
   * **Open The Door**: pulses door actuator
5. The board auto-turns off the motor and sends a notification when the timer ends.

---

## 🧪 Flow Summary

```text
setup():
  - starts Wi‑Fi
  - configures pins (motor off, door off)
  - configures secure Telegram client

loop():
  - reconnects Wi‑Fi if dropped
  - polls Telegram for new messages
  - handles commands via handleNewMessages()
  - turns off motor based on timerExpired
```
