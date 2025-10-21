Here’s a concise **README.md** draft based on your *MidTermProject.docx* instructions:

---

# Midterm Project – Real-Time Embedded System (ESP32-S3 PlatformIO)

## Overview

This project is part of the Real-Time Systems course and focuses on developing a complete RTOS-based embedded application using the **ESP32-S3** with **PlatformIO**.
The goal is to extend and modify the existing *YoloUNO_PlatformIO RTOS project* by at least **30%** with new functionalities, demonstrating strong understanding of task management, synchronization, and embedded system design.

---

## Implemented Tasks

### 🟢 Task 1: LED Blink with Temperature Conditions

* LED behavior changes dynamically with 3+ temperature ranges.
* Uses **semaphores** for task synchronization.

### 🔵 Task 2: NeoPixel LED Control by Humidity

* NeoPixel color patterns represent humidity levels (≥3 color zones).
* Implements **semaphore-based** color update logic.

### 🟡 Task 3: LCD Temperature & Humidity Display

* LCD shows real-time measurements with **three display states**: normal, warning, critical.
* Semaphore logic controls data updates.

### 🔴 Task 4: Web Server (Access Point Mode)

* Redesigned **AP Web UI** for user control and monitoring.
* Two controllable devices (LEDs or actuators) with labeled buttons.

### ⚙️ Task 5: TinyML Deployment & Accuracy Evaluation

* TinyML model trained and quantized for ESP32-S3.
* Includes dataset description, inference logic, and **on-device accuracy testing**.

---

## Project Structure

```
├── include/          # Header files
├── src/              # Main RTOS source files
├── lib/              # Libraries
├── data/             # Web server HTML/CSS/JS
├── test/             # Unit and hardware tests
├── platformio.ini    # Build configuration
└── README.md         # Project documentation
```

---

## Deliverables

* **PDF Report** – detailing design, implementation, results, and member contributions.
* **GitHub Repository** – fully commented source code (this repository).

## Team

* **[Vo Phuc Thien]** – Head Engineer
* **[Tran Hoai Nhan]** – Project Manager, Assistant Engineer


---

