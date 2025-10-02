#!/bin/bash
# Static test: Verify checksum
FILE="/manifests/visual-templates.tar.gz"
EXPECTED=$(grep expected_sha256_hash /manifests/visual-templates.yaml | awk '{print $2}')
ACTUAL=$(sha256sum $FILE | awk '{print $1}')
if [ "$ACTUAL" = "$EXPECTED" ]; then
  echo "Checksum test passed"
  exit 0
else
  echo "Checksum test failed"
  exit 1
fi
# Static test: Memory check
FREE_MEM=$(free -m | awk '/Mem:/ {print $4}')
if [ "$FREE_MEM" -ge 128 ]; then
  echo "Memory test passed"
  exit 0
else
  echo "Memory test failed"
  exit 1
fi
