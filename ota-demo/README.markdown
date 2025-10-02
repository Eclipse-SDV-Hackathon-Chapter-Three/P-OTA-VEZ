# OTA Update Demo for SDV Hackathon

## Overview
This demo implements an OTA update workflow for an SDV, focusing on fragmented updates (visual templates on instrument cluster) with A/B partitioning, rollback on failure, and dependency management. It uses Eclipse Ankaios for in-vehicle orchestration and simulates a cloud service for update delivery.

## Setup
1. Install Docker and Docker Compose.
2. Clone repos:
   ```bash
   git clone https://github.com/eclipse-ankaios/ankaios
   git clone https://github.com/eclipse-sdv-blueprints/service-to-signal
   ```
3. Generate keys:
   ```bash
   openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out private-key.pem
   openssl pkey -in private-key.pem -pubout -out public-key.pem
   ```
4. Start containers:
   ```bash
   docker-compose up -d
   ```

## Run Demo
1. Generate and sign update package:
   ```bash
   chmod +x scripts/generate-package.sh
   ./scripts/generate-package.sh
   ```
2. Apply update:
   ```bash
   chmod +x scripts/apply-update.sh
   ./scripts/apply-update.sh
   ```
3. Simulate failure and rollback:
   ```bash
   chmod +x scripts/simulate-failure.sh
   ./scripts/simulate-failure.sh
   ```
4. Check logs for traceability:
   ```bash
   cat logs/ankaios.log
   ```

## Notes
- Simulates one ECU (instrument cluster) with A/B partitioning.
- Uses `service-to-signal` uService for signal integration (mocked).
- Logs comply with UNECE WP.29 R156 for auditability.