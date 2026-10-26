
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

After a successful identification the sensor can accept one swipe (up/down/left/right), short press or long press,
usable as a command in ESPHome or Home Assistant. See [Gesture navigation](#-gesture-navigation).

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
  # optional gesture navigation, see "Gesture navigation" below
  navigation_mode: true
  navigation_time: 5s
  gesture_event: esphome.fingerprint_gesture
  on_swipe_left:
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
| `navigation_mode`         | bool   | After a successful identification, accept one gesture. See [Gesture navigation](#-gesture-navigation). Default: false |
| `navigation_time`         | time   | How long to wait for the finger to be lifted, and then for a gesture, before returning to identify. Min 500 ms. Default: 5 s |
| `navigation_orientation`  | int    | Sensor mounting rotation for gesture directions: 0, 90, 180 or 270 (degrees). Default: 0 |
| `gesture_event`           | string | Optional. Name of a Home Assistant event fired for every gesture (must start with `esphome.`). Requires `api:` |
| `password`                | string | Required. Unique HW ID from first boot. Detects if the physical sensor module has been swapped for a different unit; not a secret and not a defense against UART-bus access. |

---

## 🔥 Triggers

| Trigger                    | Description                       |
| -------------------------- | --------------------------------- |
| `on_finger_scan_start`     | A finger touched the sensor (identify/enroll; not in navigation mode) |
| `on_finger_scan_invalid`   | Finger detected but scan failed   |
| `on_finger_scan_matched`   | Fingerprint matched registered ID |
| `on_finger_scan_unmatched` | Fingerprint not recognized        |
| `on_enrollment_scan`       | Finger scanned during enrollment  |
| `on_enrollment_done`       | Enrollment successful             |
| `on_enrollment_failed`     | Enrollment failed                 |
| `on_navigation`            | Any gesture. Variables: `gesture` (1=Up, 2=Down, 3=Right, 4=Left, 5=Press, 6=LongPress), `finger_id` |
| `on_swipe_up`              | Swipe up. Variable: `finger_id`   |
| `on_swipe_down`            | Swipe down. Variable: `finger_id` |
| `on_swipe_left`            | Swipe left. Variable: `finger_id` |
| `on_swipe_right`           | Swipe right. Variable: `finger_id` |
| `on_short_press`           | Short press. Variable: `finger_id` |
| `on_long_press`            | Long press. Variable: `finger_id` |

The navigation triggers (`on_navigation`, `on_swipe_*`, `on_short_press`, `on_long_press`) fire only with
`navigation_mode: true`, see [Gesture navigation](#-gesture-navigation).

---

## 🎮 Gesture navigation

With `navigation_mode: true` a successful identification unlocks **one** gesture:

1. The sensor identifies a finger (`on_finger_scan_matched`).
2. The component waits until the finger is lifted, so the identifying touch is not taken as a gesture.
3. Navigation mode starts: the sensor waits up to `navigation_time` for one gesture
   (swipe up/down/left/right, short press, long press).
4. The first gesture fires `on_navigation` and the matching `on_swipe_*` / `on_short_press` / `on_long_press`
   trigger, with `finger_id` = the finger identified in step 1. Further gestures are ignored.
5. The sensor goes back to identify mode after the gesture, or when `navigation_time` expires without one.

If the finger stays on the sensor longer than `navigation_time` after the identification, navigation is skipped
and identify resumes. A failed identification never opens navigation. `on_finger_scan_start` does not fire for
touches during navigation.

```yaml
fingerprint_FPC2532:
  navigation_mode: true
  navigation_time: 5s
  navigation_orientation: 0      # 0, 90, 180, 270
  on_swipe_left:
    - logger.log:
        format: "Swipe left by finger %u"
        args: [finger_id]
  on_long_press:
    - light.toggle: green_led
```

**Orientation:** gesture directions are relative to the sensor. If it is mounted rotated, set
`navigation_orientation` (degrees): swipe towards what should be "right" and change it in 90° steps until
the logs show `Gesture.Right`.

### Using gestures in Home Assistant

Set `gesture_event` and the component fires a Home Assistant event for each gesture, no extra YAML needed:

```yaml
api:

fingerprint_FPC2532:
  navigation_mode: true
  gesture_event: esphome.fingerprint_gesture
```

Event data: `gesture` (`swipe_up`, `swipe_down`, `swipe_left`, `swipe_right`, `short_press`, `long_press`)
and `finger_id` (as a string). Home Assistant automation:

```yaml
triggers:
  - trigger: event
    event_type: esphome.fingerprint_gesture
    event_data:
      gesture: swipe_left
      finger_id: "3"        # optional: only this finger
actions:
  - action: light.toggle
    target:
      entity_id: light.garage
```

You can watch the events in Home Assistant under *Developer tools → Events* (listen to `esphome.fingerprint_gesture`).
If none arrive, enable *Allow the device to perform Home Assistant actions* in the ESPHome device options.

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
This version includes LEDs, text sensors, API services, all triggers and gesture navigation. LEDs require a series resistor e.g. 100Ω or 220Ω for standard LEDs.

--> [Here the example YAML](https://github.com/luigi-pi/fingerprint_fpc2532/blob/main/example_fingerprint_FPC2532.yaml)

---

## 🔗 See Also

* [Official FPC2532 Sensor Documentation](https://www.fpc.com/products/documentation/)

---
