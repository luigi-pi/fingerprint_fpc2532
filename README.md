
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
| `status_at_boot`          | bool   | Report status after boot. Default: true                             |
| `stop_mode_uart`          | bool   | Allows deep-sleep mode (requires power pin). Default: false         |
| `uart_baudrate`           | int    | Must match UART config. Default: 921600                             |
| `max_consecutive_fails`   | int    | Fail count before lockout. Default: 5                               |
| `time_before_sleep_ms`    | time   | Idle time before sleep. Default: 0 ms                               |
| `delay_before_irq_ms`     | time   | Delay after IRQ before communication. Default: 1 ms                 |
| `finger_scan_interval_ms` | time   | Delay between scan attempts. Default: 34 ms                         |
| `password`                | string | Required. Unique HW ID from first boot. Needed to authorize access. |

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
    - fingerprint_grow.enroll:
        finger_id: 0
```

### **Cancel enrollment**

```yaml
on_...:
  then:
    - fingerprint_grow.cancel_enroll:
```

### **Delete fingerprint**

```yaml
on_...:
  then:
    - fingerprint_grow.delete:
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
| `last_finger_id`                                               | Last matched fingerprint ID            |
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
## 🧪 Sample ESPHome Configuration

> **Note:** GPIOXX placeholders must be replaced with actual pins.
Replace secrets accordingly.
This version includes LEDs, text sensors, API services, and all triggers. LEDs require a series resistor e.g. 220Ω for standard LEDs.

```yaml
external_components:
  source:
    type: git
    url: https://github.com/luigi-pi/fingerprint_fpc2532.git
    ref: main
  components: [fingerprint_FPC2532]

esphome:
  name: fingerprintfpc2532

esp32:
  board: nodemcu-32s

wifi:
  ssid: !secret my_ssid
  password: !secret my_pw
  fast_connect: true
  ap:
    ssid: "FPC Fallback"
    password: !secret AP_fallback_pw

captive_portal:

logger:

ota:
  platform: esphome
  password: !secret my_OTA_Pw

uart:
  rx_pin: GPIOXX
  tx_pin: GPIOXX
  baud_rate: 921600
  rx_buffer_size: 2048
  rx_timeout: 1

fingerprint_FPC2532:
  id: fpc2532
  # IMPORTANT: replace this with the unique_id seen in logs during first boot
  password: !secret unique_id_pw

  enroll_timeout_s: 5s

  on_finger_scan_matched:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Fingerprint Authorized"
    - light.turn_on:
        id: green_led
    - delay: 1s
    - light.turn_off:
        id: green_led

  on_finger_scan_unmatched:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Fingerprint NOT Authorized"
    - light.turn_on:
        id: red_led
        effect: Flash
    - delay: 1s
    - light.turn_off:
        id: red_led

  on_enrollment_scan:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "On enrollment"
    - light.turn_on:
        id: green_led
    - light.turn_on:
        id: blue_led
        effect: Breathing
    - delay: 200ms
    - light.turn_off:
        id: green_led

  on_enrollment_failed:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Enrollment failed"
    - light.turn_on:
        id: red_led
        effect: Flash
    - delay: 3s
    - light.turn_off:
        id: red_led
    - light.turn_off:
        id: blue_led
    - light.turn_off:
        id: green_led

  on_enrollment_done:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Enrollment successful"
    - light.turn_on:
        id: green_led
    - delay: 3s
    - light.turn_off:
        id: green_led
    - light.turn_off:
        id: blue_led

  on_finger_scan_start:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Verify fingerprint"
    - light.turn_on:
        id: blue_led
        effect: Flash
    - delay: 0.5s
    - if:
        condition:
          binary_sensor.is_on: enrolling_binary
        then:
          - light.turn_on:
              id: blue_led
              effect: Breathing
        else:
          - light.turn_off:
              id: blue_led

  on_finger_scan_invalid:
    - text_sensor.template.publish:
        id: fingerprint_state
        state: "Fingerprint verification error"
    - light.turn_on:
        id: red_led
        effect: Flash
    - delay: 3s
    - light.turn_off:
        id: red_led

api:
  encryption:
    key: !secret my_api_password
  custom_services: true

  services:
    - service: enroll
      variables:
        finger_id: int
      then:
        - fingerprint_FPC2532.enroll:
            finger_id: !lambda 'return finger_id;'

    - service: cancel_enroll
      then:
        - fingerprint_FPC2532.cancel_enroll:

    - service: delete
      variables:
        finger_id: int
      then:
        - fingerprint_FPC2532.delete:
            finger_id: !lambda 'return finger_id;'

switch:
  - platform: restart
    name: "Fingerprint Sensor Restart"

binary_sensor:
  - platform: fingerprint_FPC2532
    enrolling_binary:
      id: enrolling_binary
      name: "Enrollment Active"

  - platform: fingerprint_FPC2532
    uart_irq_before_tx:
      name: "UART IRQ Before TX"

  - platform: fingerprint_FPC2532
    status_at_boot:
      name: "Status At Boot"

  - platform: fingerprint_FPC2532
    stop_mode_uart:
      name: "Stop Mode UART"

text_sensor:
  - platform: fingerprint_FPC2532
    id: text_status
    name: "Sensor Status"

  - platform: fingerprint_FPC2532
    id: unique_id
    name: "Unique Hardware ID"

  - platform: fingerprint_FPC2532
    id: version
    name: "Version"

  - platform: template
    id: fingerprint_state
    name: "Scan Result"

  - platform: template
    id: text_enrollment_feedback
    name: "Enrollment Feedback"

sensor:
  - platform: fingerprint_FPC2532
    fingerprint_count:
      name: "Number of Fingerprints"
      accuracy_decimals: 0

    last_finger_id:
      name: "Last Matched Finger"
      accuracy_decimals: 0

    num_scans:
      name: "Enrollment Scans Remaining"

    status:
      name: "Status Code"
      id: status
      internal: true

    baud_rate:
      name: "UART Baud Rate"
      accuracy_decimals: 0

    scan_interval_ms:
      name: "Scan Interval (ms)"
      accuracy_decimals: 0

    uart_dly_before_tx_ms:
      name: "UART Delay Before TX (ms)"
      accuracy_decimals: 0

    idle_time_before_sleep_ms:
      name: "Idle Time Before Sleep (ms)"
      accuracy_decimals: 0

    lockout_after_nr_of_fails:
      name: "Lockout After Failures"
      accuracy_decimals: 0

    lockout_time_s:
      name: "Lockout Time (s)"
      accuracy_decimals: 0

    enroll_timeout_s:
      name: "Enrollment Timeout (s)"
      accuracy_decimals: 0

    enrollment_feedback:
      id: num_enrollment_feedback
      internal: true
      name: "Enrollment Feedback (Code)"
      on_value:
        then:
          - lambda: |-
              switch ((int)id(num_enrollment_feedback).state) {
                case 8: id(text_enrollment_feedback).publish_state("Touch the sensor to enroll"); break;
                case 1: id(text_enrollment_feedback).publish_state("Enrollment completed"); break;
                case 2: id(text_enrollment_feedback).publish_state("Record fingerprint"); break;
                case 3: id(text_enrollment_feedback).publish_state("Discarded: bad quality"); break;
                case 4: id(text_enrollment_feedback).publish_state("Discarded: low coverage"); break;
                case 5: id(text_enrollment_feedback).publish_state("Discarded: low mobility"); break;
                case 6: id(text_enrollment_feedback).publish_state("Discarded: unspecified"); break;
                case 7: id(text_enrollment_feedback).publish_state("Recorded: immobile"); break;
                default: id(text_enrollment_feedback).publish_state("Unknown status"); break;
              }

light:
  - platform: monochromatic
    id: red_led
    output: output_red
    default_transition_length: 25ms
    restore_mode: ALWAYS_OFF
    effects: &led_effects
      - pulse:
          name: Breathing
          transition_length: 0.5s
          update_interval: 0.5s
      - strobe:
          name: Flash
          colors:
            - state: true
              brightness: 100%
              duration: 125ms
            - state: false
              duration: 125ms

  - platform: monochromatic
    id: green_led
    output: output_green
    default_transition_length: 25ms
    restore_mode: ALWAYS_OFF
    effects: *led_effects

  - platform: monochromatic
    id: blue_led
    output: output_blue
    default_transition_length: 25ms
    restore_mode: ALWAYS_OFF
    effects: *led_effects

output:
  - platform: ledc
    id: output_red
    pin: GPIOXX

  - platform: ledc
    id: output_green
    pin: GPIOXX

  - platform: ledc
    id: output_blue
    pin: GPIOXX

```
---

## 🔗 See Also

* [Official FPC2532 Sensor Documentation](https://www.fpc.com/products/documentation/)

---
