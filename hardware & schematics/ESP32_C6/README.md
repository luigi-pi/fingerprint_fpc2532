---

# ESP32-C6 Fingerprint Hardware (FPC2532)

This folder contains the **hardware design and enclosure** for an ESP32-C6–based fingerprint system using the **FPC2532 sensor**.

---

## 📁 Folder Structure

### 🔌 PCBs & Schematics

This section includes all boards required to build the system.

#### ⚡ AC_DC Board

* Input: **220 VAC**
* Features:

  * Fuse (**250 mA**)
  * Thermal fuse (**86°C**)
  * Varistor protection
  * LC input filter
* Output:

  * Powers an **HLK-PM01 AC/DC module**

> ⚠️ **WARNING:** This board operates at **mains voltage**. Improper handling can result in serious injury or damage.

---

#### 🔋 DC_DC Board

* Input: **9–24 V DC**
* Features:

  * DC fuse (**1.25 A**)
  * LC filter
  * Additional filtering capacitors
* Compatible with:

  * **MP1584EN DC-DC modules** (pin-compatible)

---

#### 🔁 Relay-5V Board

* Can be used with:

  * AC_DC Board (low-voltage side of HLK-PM01)
  * DC_DC Board
* Features:

  * Relay output
  * Transistor-based control circuit

---

#### 🧠 ESP32_C6 Board

* Based on **ESP32-C6 WROOM module**
* Interfaces:

  * **2.54 mm header**
  * Adapter for **FPC2532 fingerprint module**

    * 12-pin, 0.5 mm FPC connector → 2.54 mm header

---

### ⚠️ Safety Notice

> **DANGER:**
> This project includes **high-voltage AC circuitry**.
> Only proceed if you have the **necessary skills to review and validate the design**.
> The author takes no responsibility for damage or injury.

---

## 🧱 3D Printed Case

### Material

* Recommended: **ASA**

  * UV resistant
  * Weather resistant (suitable for outdoor use)

---

### 🧩 Assembly Overview

#### 🔧 Mounting

* Backplate:

  * Fix to wall using screws
* PCBs:

  * Mount using **M2.5 nylon spacers**
  * Required spacing between PCB layers: **25 mm**
  * Suggested stack:

    * 20 mm spacer + 15 mm spacer
    * Add **polyimide (PI) insulation foil** between layers

---

#### 👆 Fingerprint Module Installation

* Mount the **FPC2532 sensor** inside the case using:

  * Double-sided adhesive tape
* For **IP68 sealing**:

  * Fill with **potting material**:

    * Silicone / Polyurethane / Epoxy
  * Fill up to the level of the sensor ring

> 📌 Refer to FPC2532 documentation for sealing guidelines.

---

#### 🔌 Electrical Connection

* Connect sensor via:

  * Flex cable → 2.54 mm adapter board
* Secure adapter board inside the case with screws

---

#### 💡 LED Protection

* Seal LED opening with:

  * Transparent cover layer
  * Double-sided adhesive

---

#### 🛡️ Optional Cover

* Sliding cover included:

  * Adds mechanical protection
  * Helps prevent dirt accumulation

---

## 🖼️ Assembly Reference Images

<div align="center">

<img src="https://github.com/user-attachments/assets/1ad9e541-949e-4905-9b7b-eea2830e3494" width="400"/>
<img src="https://github.com/user-attachments/assets/a954c9a4-0cef-4832-b0a2-0bc3c3693aca" width="400"/>

<img src="https://github.com/user-attachments/assets/56505231-38a2-4669-a47a-34345d2e2b28" width="400"/>
<img src="https://github.com/user-attachments/assets/245796b5-18ea-47cb-b1ce-266e958948d2" width="400"/>

<img src="https://github.com/user-attachments/assets/84e52d4f-0489-4601-a519-0234d6618847" width="400"/>
<img src="https://github.com/user-attachments/assets/5a602015-2cf4-4e34-9908-93485ea315f5" width="400"/>

<img src="https://github.com/user-attachments/assets/a3b71c3f-dea3-4520-b1c3-1c9b414cb549" width="400"/>
<img src="https://github.com/user-attachments/assets/43b091ab-6271-4f48-b349-bdb077bff30d" width="400"/>

<img src="https://github.com/user-attachments/assets/39f9c56b-2f4f-46de-ac9e-e6a9eeace156" width="400"/>
<img src="https://github.com/user-attachments/assets/31ea3971-17b7-4626-86a5-4f3c45b93767" width="400"/>

</div>

---

## ✅ Notes & Tips

* Always validate **clearances and insulation** when working with mixed AC/DC designs
* Prefer **nylon hardware** to reduce short-circuit risks
* Potting is critical to maintain **IP rating**
* Double-check connector orientation before powering

---
