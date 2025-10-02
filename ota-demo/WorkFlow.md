### How Ankaios Detects Corruption and Triggers Rollback

Your OTA demo for the SDV hackathon uses **Eclipse Ankaios** to manage in-vehicle software updates, with a focus on **fragmented installation**, **A/B partitioning**, **dependency management**, **validation**, and **rollback** in case of failures, such as a corrupted package. The `simulate-failure.sh` script in your setup (`~/eclipse_sdv_hackaton/P-OTA-VEZ/ota-demo`) intentionally corrupts the update package (e.g., `visual-templates.tar.gz`) to fail its **checksum verification**, prompting Ankaios to detect the failure and trigger a **rollback** to the stable partition A (v1.0). Below, I’ll explain how Ankaios handles this process, based on its architecture and your demo setup, and clarify the mechanism for corruption detection and rollback.

---

### Ankaios’s Corruption Detection Mechanism

Ankaios is a workload orchestration framework designed for automotive systems, managing containerized workloads (like your instrument cluster’s visual templates) with robust validation and error handling. In your OTA demo, it uses a **server-agent model** running in Docker containers on a Raspberry Pi 5 (Ubuntu, ARM64). The corruption detection and rollback process involves several steps, leveraging Ankaios’s state management, validation checks, and logging capabilities.

#### 1. **Update Workflow Overview**
- **Components**:
  - **Ankaios Server** (`ankaios-server` container): Orchestrates updates, manages state, and coordinates with agents.
  - **Ankaios Agent** (`ankaios-agent-cluster` container): Executes updates on the target ECU (instrument cluster), runs tests, and reports status.
  - **Mock Cloud** (`mock-cloud` container): Pushes update manifests (e.g., `visual-templates.yaml`) to the server.
  - **Manifests**: YAML files (`manifests/visual-templates.yaml`, `bootloader.yaml`) define workloads, dependencies, and checksums.
- **A/B Partitioning**: The instrument cluster uses two partitions:
  - **Partition A**: Stable, running `visual-templates:v1.0`.
  - **Partition B**: Target for updates (e.g., `visual-templates:v2.0`).
- **Validation**: Updates undergo **static** (checksum) and **dynamic** (functional) tests before switching partitions.

#### 2. **Corruption Simulation (via `simulate-failure.sh`)**
The `simulate-failure.sh` script intentionally corrupts the update package (`visual-templates.tar.gz`) to fail the **checksum verification**. Based on the demo setup (Eclipse SDV’s P-OTA-VEZ), this likely involves:
- Modifying the `visual-templates.tar.gz` file (e.g., appending random data or truncating it).
- Example script snippet (hypothetical, based on typical demo structure):
  ```bash:disable-run
  # scripts/simulate-failure.sh
  echo "Corrupting visual-templates.tar.gz..."
  echo "random-data" >> manifests/visual-templates.tar.gz
  echo "Triggering update with corrupted package..."
  curl -X POST -d @manifests/visual-templates.yaml http://ankaios-server:25551/api/v1/workloads
  ```
- **Checksum Mismatch**: The package’s SHA-256 checksum (stored in `visual-templates.yaml`, e.g., `64add1361da865ce5aa49b25bf8305a7c5fa3045331946c5ad24c5c7cfdf0a2d`) no longer matches the corrupted file.

#### 3. **Ankaios’s Detection Process**
Ankaios detects corruption through its **validation pipeline**, which includes:
- **Checksum Verification**:
  - The Ankaios agent downloads the package (`visual-templates.tar.gz`) and its signature (`visual-templates.tar.gz.sig`) from the server or a specified source (via `mock-cloud`).
  - The agent computes the SHA-256 checksum of the downloaded package and compares it to the expected checksum in `visual-templates.yaml`.
  - **Detection**: If the checksums don’t match (due to corruption from `simulate-failure.sh`), the agent marks the package as invalid and halts the update process.
  - Example (internal Ankaios logic, simplified):
    ```pseudo
    computed_checksum = sha256(visual-templates.tar.gz)  # e.g., "abc123..."
    expected_checksum = manifest["visual-templates"]["checksum"]  # "64add136..."
    if computed_checksum != expected_checksum:
        log("Checksum mismatch, package corrupted!")
        set_workload_state("FAILED")
    ```
- **Signature Verification**:
  - The agent verifies the package’s ECDSA signature using `public-key.pem` (generated earlier in `generate-package.sh`).
  - If the signature is invalid (e.g., due to corruption), this provides an additional layer of security, but the checksum failure is sufficient to trigger a failure.
- **Static Tests**:
  - The agent runs static tests (via `tests/static-tests.sh`), which may include additional integrity checks (e.g., file structure, size).
  - Corruption typically fails here, as the tarball may not extract correctly.

#### 4. **Rollback Trigger**
Once corruption is detected:
- **Agent Reports Failure**:
  - The agent updates the workload state to `FAILED` and notifies the server via gRPC (Ankaios’s communication protocol).
  - Example log (in `logs/ankaios.log`):
    ```
    [2025-10-02T09:30:00Z] ERROR: Workload visual-templates:v2.0 FAILED - Checksum mismatch
    ```
- **Server Initiates Rollback**:
  - Ankaios’s server maintains the system state (desired vs. current) using its state machine.
  - On failure, the server reverts the instrument cluster to **partition A** (running `visual-templates:v1.0`), ensuring the vehicle remains operational.
  - This is achieved by:
    - Not switching the active partition (keeping partition A active).
    - Discarding the corrupted update on partition B.
    - Updating the workload state to reflect `visual-templates:v1.0` as active.
  - Example log:
    ```
    [2025-10-02T09:30:01Z] INFO: Rolling back to visual-templates:v1.0 on partition A
    ```
- **Podman Integration**:
  - Ankaios uses Podman to manage containers. If the update fails, the agent ensures the Podman container for `visual-templates:v2.0` is not started, and the `v1.0` container remains active.

#### 5. **Traceability**
- All steps (checksum failure, test failure, rollback) are logged in `logs/ankaios.log` with timestamps, meeting **UNECE WP.29 R156** auditability requirements.
- Example log snippet:
  ```
  [2025-10-02T09:30:00Z] INFO: Applying visual-templates:v2.0
  [2025-10-02T09:30:00Z] ERROR: Checksum mismatch: expected 64add136..., got abc123...
  [2025-10-02T09:30:01Z] INFO: Static tests failed for visual-templates:v2.0
  [2025-10-02T09:30:01Z] INFO: Rollback initiated to visual-templates:v1.0
  [2025-10-02T09:30:02Z] INFO: Vehicle available, partition A active
  ```

---

### Detailed Workflow in the Demo
Here’s how the corruption and rollback play out in your setup:
1. **Initial State**:
   - Instrument cluster runs `visual-templates:v1.0` on partition A.
   - `docker-compose.yml` runs `ankaios-server`, `ankaios-agent-cluster`, and `mock-cloud`.
2. **Update Attempt** (`apply-update.sh`):
   - Pushes `bootloader.yaml` and `visual-templates.yaml` to the server.
   - Server resolves dependencies (bootloader first), instructs agent to update partition B.
3. **Corruption via `simulate-failure.sh`**:
   - Corrupts `visual-templates.tar.gz` (e.g., appends data).
   - Agent detects checksum mismatch during validation.
4. **Validation Failure**:
   - Static tests (`tests/static-tests.sh`) fail due to checksum error.
   - Agent reports `FAILED` state to server.
5. **Rollback**:
   - Server keeps partition A (`v1.0`) active, discards corrupted `v2.0` on partition B.
   - Logs capture all events for traceability.
6. **Verification**:
   - `docker exec -it ankaios-server ank cli get workloads` shows `visual-templates:v1.0` running.
   - `tail -20 logs/ankaios.log` confirms rollback.

---

### Technical Details
- **Checksum Mechanism**:
  - Generated in `generate-package.sh`:
    ```bash
    sha256sum visual-templates.tar.gz > checksum.txt
    ```
  - Stored in `visual-templates.yaml` (e.g., `checksum: 64add136...`).
  - Agent recomputes using `sha256sum` or equivalent.
- **A/B Partitioning**:
  - Simulated via Podman containers (one per partition).
  - Ankaios’s state machine ensures only one partition is active (`desired_state` vs. `current_state`).
- **Rollback Logic**:
  - Handled by Ankaios’s server, which updates the `desired_state` to revert to `v1.0` if tests fail.
  - No manual intervention needed—fully automated.
- **Security**:
  - ECDSA signatures (`private-key.pem`, `public-key.pem`) ensure authenticity.
  - Corruption also fails signature check, reinforcing detection.

---

### Hackathon Demo Context
For your slides (from the previous response):
- **Slide 4 (Technical Implementation)**:
  - Highlight: “Ankaios detects corruption via SHA-256 checksum verification and ECDSA signatures, triggering automatic rollback to partition A.”
  - Show log snippet: Checksum failure → rollback.
- **Slide 6 (Demo)**:
  - Run `simulate-failure.sh` live.
  - Display `logs/ankaios.log` to show: “Checksum mismatch” → “Rollback to v1.0.”
  - Emphasize: “Zero-downtime updates with traceability for WP.29 compliance.”

---

### Action Item
Since you’ve built the binaries (`cargo build --release --target aarch64-unknown-linux-gnu`), proceed with:
1. **Confirm Binary**:
   ```bash
   ls -la ~/eclipse_sdv_hackaton/ankaios/target/aarch64-unknown-linux-gnu/release/
   ```
   - Share output to confirm `ank` or alternative (e.g., `ank-cli`).
2. **Build Docker Image**
3. **Run Demo**
4. **Test Corruption**:
   ```bash
   chmod +x scripts/simulate-failure.sh
   ./scripts/simulate-failure.sh
   tail -20 logs/ankaios.log
   ```

```
