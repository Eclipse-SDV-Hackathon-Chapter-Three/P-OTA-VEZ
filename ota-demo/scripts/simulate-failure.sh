#!/bin/bash
# Corrupt package to fail checksum
echo "Invalid content" > manifests/visual-templates.tar.gz
# Apply update (should fail and rollback)
docker exec ankaios-server ank cli apply /manifests/visual-templates.yaml
# Verify rollback
docker exec ankaios-agent-cluster podman ps --filter "ancestor=docker.io/visual-templates:v1.0"
