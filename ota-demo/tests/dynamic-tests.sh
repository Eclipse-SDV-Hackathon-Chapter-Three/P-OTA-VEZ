#!/bin/bash
# Simulate UI rendering test
podman run --rm docker.io/visual-templates:v2.0 /app/render-test
if [ $? -eq 0 ]; then
  echo "UI rendering test passed"
  exit 0
else
  echo "UI rendering test failed"
  exit 1
fi
