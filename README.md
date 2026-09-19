
---

# `fingerprint_fpc2532`

**ESPHome integration for the FPC2532 fingerprint sensor (ESP32)**
#### **This repository provides a custom ESPHome component for interfacing the FPC2532 fingerprint module, enabling secure and reliable fingerprint authentication for home-automation projects.**
---

## ⭐ Why another fingerprint sensor for ESPHome?

ESPHome already includes an official fingerprint component for the R503 (`fingerprint_grow`).
So why create another one?

### 🔐 **More secure**

The R503 stores fingerprint images *unencrypted* in its memory.
If someone removes the sensor, your fingerprint images are accessible in plain form — a potentially serious privacy/security risk.
The FPC2532 encrypts stored data.

### 🛡️ **No fingerprint image extraction**

With the R503 it's possible to extract and share fingerprint images (dangerous if the DIY solution has any security flaws).
The FPC2532 does **not** allow image extraction.

### 📘 **Proper documentation & reliability tests**

FPC2532 modules come with better documentation and real reliability testing.

### 🔋 **Much lower power consumption**

Ideal for battery-powered devices.**22 µA** in finger-detect mode.

### 🎮 **Gesture navigation**

The hardware allows simple up/down/left/right swipe navigation (future feature).

### ⚠️ Drawbacks

* More expensive
* No built-in LEDs
* Requires some integration work for enclosure mounting

---

> ⚠️ **Note:** This repository is a work in progress. Errors or omissions may exist. Please verify the content and feel free to contribute improvements!

---

## 🧩 Component Overview

The sensor communicates via UART (default `921600 baud`).
* Pin `CONFIG` (pin 1) must be tied HIGH (3.3 V, through a resistor, e.g. 24kOhm) for UART operation.
* Pin 3.3V (pin 10) must be tied to the 3.3V power supply (check datasheet for optional caps and TVS diodes).
* Pin Rx (pin 3) and Tx (pin 2) must be connected to ESP Tx and Rx (swapped: sensor Tx to ESP Rx and vice versa).
* Pin 11 and 12 must be connected to ESP32 GND.
* for optional pins see table below

### Optional pins

| Pin                  | Purpose                                                                                                                                                                 |
| -------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **sensor_power_pin** | Wakes up the sensor before sending a command. Requires `stop_mode_uart: true`. Recommended **not** to use because sensor consumes only **22 µA (0.7mW)**  in finger-detect mode. Connected to sys-WU (pin 8 of fpc sensor). To disable SYS-WU must be connected to 3.3V (through a resistor, e.g. 24kOhm) |
| **reset_pin**        | Hardware reset of the sensor. Pin 9 of the sensor. Connect the pin to an ESP32 GPIO with an external pull-up resistor (e.g. 22 kΩ) to 3.3 V.|
| **irq_pin**          | Wakes up the ESP32 when the sensor detects a finger. Useful for low-power designs (sleep mode not yet implemented, can be added via YAML).  Pin 4 of the sensor, to be connected to ESP32 RTC GPIO through a resistor e.g. 2.2kOhm (to avoid issues if set as output pin  by mistake )                                                |

---

## 📄 Example configuration

```yaml
fingerprint_FPC2532:
  sensor_power_pin:
      number: GPIOXX
  reset_pin:
      number: GPIOXX
  irq_pin:
      number: GPIOXX
  enroll_timeout_s: 5s
  on_finger_scan_matched:
    ...
  on_finger_scan_unmatched:
    ...
  on_enrollment_scan:
    ...
  on_enrollment_failed:
    ...
  on_enrollment_done:
    ...
  on_finger_scan_start:
    ...
  on_finger_scan_invalid:
    ...
```

---

## ⚙️ Configuration Options

The configuration consists of:

* **Main component**
* **Optional binary sensors**
* **Optional sensors**
* **Optional text sensors**

---

### ### 🔧 Base Configuration Variables

| Option                    | Type   | Description                                                         |
| ------------------------- | ------ | ------------------------------------------------------------------- |
| `sensor_power_pin`        | pin    | Optional. Wakes sensor before commands are sent by the nodeMCU.     |
| `reset_pin`               | pin    | Optional. Sensor hardware reset.                                    |
| `irq_pin`                 | pin    | Optional. Interrupt pin for ESP NodeMCU wake-up.                    |
| `lockout_time_s`          | time   | Lockout duration after too many failed attempts. Default: 15 s      |
| `uart_irq_before_tx`      | bool   | Generate IRQ before UART TX. Default: true                          |
| `status_at_boot`          | bool   | Report status after boot. Default: false                             |
| `stop_mode_uart`          | bool   | Allows deep-sleep mode (requires power pin). Default: false         |
| `uart_baudrate`           | int    | Must match UART config. Default: 921600                             |
| `max_consecutive_fails`   | int    | Fail count before lockout. Default: 5                               |
| `time_before_sleep_ms`    | time   | Idle time before sleep. Default: 0 ms                               |
| `delay_before_irq_ms`     | time   | Delay after IRQ before communication. Default: 1 ms                 |
| `finger_scan_interval_ms` | time   | Delay between scan attempts. Default: 34 ms                         |
| `password`                | string | Required. Unique HW ID from first boot. Detects if the physical sensor module has been swapped for a different unit; not a secret and not a defense against UART-bus access. |

---

## 🔥 Triggers

| Trigger                    | Description                       |
| -------------------------- | --------------------------------- |
| `on_finger_scan_start`     | A finger touched the sensor       |
| `on_finger_scan_invalid`   | Finger detected but scan failed   |
| `on_finger_scan_matched`   | Fingerprint matched registered ID |
| `on_finger_scan_unmatched` | Fingerprint not recognized        |
| `on_enrollment_scan`       | Finger scanned during enrollment  |
| `on_enrollment_done`       | Enrollment successful             |
| `on_enrollment_failed`     | Enrollment failed                 |

---

## 🏃 Actions

### **Start enrollment**

```yaml
on_...:
  then:
    - fingerprint_FPC2532.enroll:
        finger_id: 0
```

### **Cancel enrollment**

```yaml
on_...:
  then:
    - fingerprint_FPC2532.cancel_enroll:
```

### **Delete fingerprint**

```yaml
on_...:
  then:
    - fingerprint_FPC2532.delete:
        finger_id: 0
```

---

## 📡 Binary Sensors

| Binary Sensor        | Description                     |
| -------------------- | ------------------------------- |
| `enrolling_binary`   | True when device is enrolling   |
| `uart_irq_before_tx` | Mirror of configuration setting |
| `status_at_boot`     | Mirror of configuration setting |
| `stop_mode_uart`     | Mirror of configuration setting |

---

## 📟 Sensors

| Sensor                                                         | Description                            |
| -------------------------------------------------------------- | -------------------------------------- |
| `fingerprint_count`                                            | Number of stored fingerprints          |
| `status`                                                       | Raw numeric status register            |
| `num_scans`                                                    | Remaining scans to complete enrollment |
| `enrollment_feedback`                                          | Enrollment state code                  |
| `baud_rate`, `scan_interval_ms`, `uart_dly_before_tx_ms`, etc. | Configuration feedback sensors         |

---

## ✏️ Text Sensors

| Text Sensor   | Description                              |
| ------------- | ---------------------------------------- |
| `version`     | Sensor firmware version                  |
| `unique_id`   | Hardware ID (should be used as password) |
| `text_status` | Verbose status description               |
| `last_finger_id` | JSON {"id":<n>,"ts":<millis>} of the last matched finger ID |

All four are declared as nested keys under a single `platform: fingerprint_FPC2532` block, the same way `sensor:` options are — not as separate list entries:
 
```yaml
text_sensor:
  - platform: fingerprint_FPC2532
    text_status:
      name: "Sensor Status"
    unique_id:
      name: "Unique Hardware ID"
    version:
      name: "Version"
    last_finger_id:
      name: "Last Matched Finger"
```
---

## 🔐 Password & Unique ID Handling 

The FPC2532 requires a device password for all authenticated operations.
This password **must match the sensor’s internal unique hardware ID (unique_id)**.

### 🟦 Step 1 — Obtain the Unique ID

During the first installation, temporarily enable the `unique_id` text sensor in your YAML.
After flashing the device **without a password set**, go to Home Assistant:

```
Settings → Devices & Services → Devices → Select your fingerprint sensor → Sensor tab
```

There you will find the `unique_id` value. Write it down.

### 🟦 Step 2 — Set the Password

Add the detected ID to your YAML under your password (check above Base Configuration Variables)
Re-flash the device.
From now on, the sensor will authenticate correctly.

### 🛡️ Why this matters

The unique_id is permanently fused into the hardware.
Using it as the password ensures that:

* Only *this* exact sensor can authenticate with your ESP32
* Replacing the module with another FPC2532 will cause the password check to fail
* Fingerprint access cannot be spoofed by swapping hardware

---
## 🧪 Sample ESPHome YAML Configuration

> **Note:** GPIOXX placeholders must be replaced with actual pins.
Replace secrets accordingly.
This version includes LEDs, text sensors, API services, and all triggers. LEDs require a series resistor e.g. 100Ω or 220Ω for standard LEDs.

--> [Here the example YAML](https://github.com/luigi-pi/fingerprint_fpc2532/blob/main/example_fingerprint_FPC2532.yaml)

---

## 🔗 See Also

* [Official FPC2532 Sensor Documentation](https://www.fpc.com/products/documentation/)

---
