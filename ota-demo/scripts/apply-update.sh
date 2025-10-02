#!/bin/bash
# Apply bootloader update first (dependency)
docker exec ankaios-server ank cli apply /manifests/bootloader.yaml
# Wait for bootloader validation
sleep 5
# Apply visual templates update
docker exec ankaios-server ank cli apply /manifests/visual-templates.yaml
# Check status
docker exec ankaios-server ank cli get workloads
