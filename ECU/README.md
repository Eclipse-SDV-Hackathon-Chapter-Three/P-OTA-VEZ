# MXChip AZ3166 – Bootloader and A/B Application Layout

Overview

This repository contains an embedded software baseline for the MXChip AZ3166 (STM32F412) targeting a bootloader with dual application partitions (A/B), real‑time OS integration (ThreadX/NetX Duo), and a CMake‑based build system suitable for CI/CD and OTA extensibility. The current scope focuses on establishing a reproducible build, validating device configuration, and preparing the memory layout required for a safe update strategy.
Current Status

    Toolchain and build system

        Cross-compilation with ARM GCC verified end‑to‑end, including C/ASM sources and startup code for STM32F412.

        Top-level CMake project streamlined; out-of-tree targets are delegated to subprojects for clearer ownership and reduced coupling.

        Successful generation of application artifacts: ELF and Intel HEX for the main application target.

    Device configuration

        Correct STM32 device headers selected (STM32F412) and startup sequence in place.

        HAL/ThreadX/NetX Duo include paths, compile options, and core definitions aligned with Cortex‑M4F (hard‑FP, fpv4-sp-d16).

    UART initialization

        UART init refactored into a shared implementation and optionally compiled per target; warnings treated as errors resolved by including headers or removing nonessential calls for rapid iteration.

    Partitioning groundwork

        Linker scripts prepared to place an application at a non‑zero FLASH offset (e.g., App A at 0x08020000), with section mapping (.isr_vector, .text, .rodata in FLASH; .data/.bss in RAM).

        Memory usage confirmed within safe margins on the reference application build (RAM/FLASH footprints reported by the linker).

## What’s Done (Technical Detail)

    Build reproducibility

        Deterministic CMake configuration with explicit compiler flags (-mcpu=cortex-m4, -mfpu=fpv4-sp-d16, -mfloat-abi=hard, -fdata/ffunction‑sections, -Werror policy, nano specs optional).

        Clean separation of shared sources (e.g., UART) and target‑specific sources; includes resolved across CMSIS/HAL/RTOS stacks.

    Application artifact validation

        Main application builds to .elf and .hex; link map indicates expected placement of sections; size report confirms headroom for future features.

    Memory layout readiness

        Example linker script for an app at 0x08020000 with 384 KiB window; sections mapped to FLASH_APP_A and RAM with proper AT> placement for .data initialization.

## What’s Pending (Actionable Next Steps)

    Finalize partition layout and linker scripts

        Provide three linker scripts: bootloader (FLASH origin 0x08000000), App A (e.g., 0x08020000), App B (e.g., 0x08080000 or another non‑overlapping region).

        Confirm sizes per device datasheet; reserve metadata/header area (version, CRC) at the start of each app region for integrity checks.

    Bootloader control flow

        Implement robust jump sequence: disable IRQs, set SCB->VTOR to app base, load MSP from [app_base+0x0], load Reset_Handler from [app_base+0x4], branch to Reset_Handler.

        Add app selection policy (A/B): flag in FLASH, rollback criteria, and simple safe‑mode fallback.

    Flashing and verification

        Add per‑target flash tasks (OpenOCD/ST‑Link) that write each image to its correct offset; generate .bin aligned to partition origin.

        Smoke tests: LED/UART banner on entry to BL/App A/App B; confirm VTOR and stack pointer are correct at runtime.

    Integrity and rollback

        Embed manifest (version, size, CRC/SHA) in each app slot; bootloader validates before jump.

        Define failure counters and automatic fallback to the alternate slot when boot fails or watchdog trips early.

    OTA/update pipeline (optional next milestone)

        Define package format (header + payload) and transport abstraction; add staging/commit protocol to write into inactive slot, then atomically switch.

        CI job to produce signed/hashed artifacts and publish release bundles.

    Hardening and diagnostics

        Watchdog enable in BL and apps; brown‑out and error logging for recovery analysis.

        Minimal crash log region in FLASH/retained RAM for post‑mortem across resets.

## How to Build (Local)

    Prerequisites: ARM GCC toolchain, CMake ≥ 3.5, STM32 utilities (OpenOCD or ST‑Link).

    Typical flow:

        Configure and build: run project’s build script or invoke CMake presets.

        Artifacts: <target>.elf, <target>.hex (and optional .bin).

    Notes:

        Warnings treated as errors: ensure target includes the appropriate headers for any hardware init (e.g., UART) or comment noncritical init while bringing up the build.

## How to Flash and Test (Partitioned Layout)

    Bootloader

        Flash bootloader image at 0x08000000.

    Application A

        Flash App A image at 0x08020000 (or partition origin defined in its linker script).

    Application B (when available)

        Flash App B image at its partition origin (e.g., 0x08080000).

    Expected behavior

        On reset, bootloader selects slot per policy, sets VTOR to the chosen app base, and jumps to the app’s Reset_Handler.

        Verify with UART banner or LED pattern unique to each image.

## Project Conventions

    Targets: one CMake target per image (bootloader, app_a, app_b, safe_mode), each with its own linker script.

    Toolchain flags: unified Cortex‑M4F options; -ffunction/-fdata‑sections with linker GC to minimize footprint.

    Code quality: -Wall/Werror policy; keep hardware access in isolated modules to ease unit testing on host where feasible.

## Risks and Open Items

    Partition sizes must be validated against worst‑case footprint and future features (ML/diagnostics).

    Dual‑bank features vary by MCU family; if not available, A/B requires fixed offsets and careful OTA sequencing.

    Interrupt vector relocation must be consistent across system init paths to avoid hard faults when apps run from non‑zero FLASH origins.

## Roadmap (Suggested)

    M1: Finalize BL/App A/B linker scripts and boot jump; add per‑slot flash tasks; end‑to‑end A/B demo.

    M2: Add manifest, CRC, rollback logic; watchdog and safe‑mode.

    M3: OTA staging/commit pipeline and artifact signing; CI integration.

    M4: Telemetry and persistent crash logs; documentation and acceptance test suite.
