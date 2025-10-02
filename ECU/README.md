# MXChip AZ3166 – Bootloader and A/B Application Layout

***
This repository contains an embedded software baseline for the **MXChip AZ3166 (STM32F412)** targeting a **bootloader with dual application partitions (A/B)**, **real-time OS integration (ThreadX/NetX Duo)**, and a **CMake-based build system** suitable for **CI/CD** and **OTA extensibility**.

As an **automotive software engineer** focusing on autonomous vehicle components, this setup emphasizes **safe firmware updates** to minimize downtime and ensure reliability in simulated vehicle subsystems, as demonstrated in the **SDV Eclipse hackathon**.

The current scope focuses on establishing a reproducible build, validating device configuration, and preparing the memory layout required for a safe update strategy. **Note:** Native dual-boot is not available in this STM32F412 version; partitioning is implemented via software.
***

## Current Status

### Toolchain and Build System

* **Cross-compilation with ARM GCC** verified end-to-end, including C/ASM sources and startup code for **STM32F412**.
* **Top-level CMake project streamlined**; out-of-tree targets are delegated to subprojects for clearer ownership and reduced coupling.
* Successful generation of **application artifacts**: `ELF` and `Intel HEX` for the main application target.

### Device Configuration

* Correct STM32 device headers selected (STM32F412) and startup sequence in place.
* **HAL/ThreadX/NetX Duo** include paths, compile options, and core definitions aligned with **Cortex-M4F** (`hard-FP`, `fpv4-sp-d16`).

### UART Initialization

* `UART init` refactored into a **shared implementation** and optionally compiled per target; warnings treated as errors resolved.

### Partitioning Groundwork

* **Linker scripts** prepared to place an application at a **non-zero FLASH offset** (e.g., App A at `0x08020000`), with section mapping (FLASH for code/read-only data; RAM for `.data`/`.bss`).
* **Memory usage confirmed** within safe margins (RAM/FLASH footprints reported by the linker).

---
ol
## What's Done

### Build Reproducibility

* **Deterministic CMake configuration** with explicit compiler flags (e.g., `-mcpu=cortex-m4`, `-ffunction-sections`, `-Werror policy`).
* Clean separation of shared sources (e.g., UART) and target-specific sources.

### Application Artifact Validation

* Main application builds to `.elf` and `.hex`; link map indicates expected section placement.

### Memory Layout Readiness

* Example **linker script** for an app at `0x08020000` with **384 KiB** window; sections mapped to `FLASH_APP_A`.

### Partition Layout Details

Based on the STM32F412's **1MB FLASH memory**, the following non-overlapping partition layout is proposed to support bootloader, dual applications (A/B), and safe mode. This is crucial for safe OTA updates in automotive contexts.

| Partition | FLASH Range | Size | Content |
| :--- | :--- | :--- | :--- |
| **Bootloader** | `0x08000000`–`0x08020000` | 128 KiB | Contains entry point and logic for app selection/validation. |
| **App A** | `0x08020000`–`0x08080000` | 384 KiB | Primary application slot. |
| **App B** | `0x08080000`–`0x080E0000` | 384 KiB | Secondary slot for updates. |
| **Safe Mode** | `0x080E0000`–`0x08100000` | 128 KiB | Minimal recovery firmware (e.g., basic UART diagnostics) for fallback. |

> **Note:** Reserve **~4KB** at the start of each app/safe mode partition for **metadata** (version, CRC/SHA checksum, size) to enable integrity checks.

### Safe Mode Implementation

Safe mode provides a fallback for critical failures, such as repeated boot failures in A/B slots.

**In the Bootloader:**

1.  Detect failures via **watchdog timeouts**, **invalid CRC**, or a **failure counter** in non-volatile FLASH.
2.  If triggered, jump to safe mode partition (e.g., set `SCB->VTOR` to `0x080E0000` and branch to its `Reset_Handler`).

**Example Failure Detection Snippet (in C):**

```c
// In bootloader main:
uint32_t failure_count = *(volatile uint32_t*)FAILURE_COUNTER_ADDR; // e.g., 0x0801FFFC

if (validate_app(APP_A_ADDR) == OK) {
    jump_to_app(APP_A_ADDR);
} else if (validate_app(APP_B_ADDR) == OK) {
    jump_to_app(APP_B_ADDR);
} else {
    if (failure_count >= MAX_FAILURES) {
        jump_to_app(SAFE_MODE_ADDR);  // Jump to 0x080E0000
    }
    failure_count++;
    // Write counter back to FLASH
}