#!/bin/bash
# Create dummy update package
echo "Visual Templates v2.0" > visual-templates.tar.gz
# Generate SHA256 checksum
SHA256=$(sha256sum visual-templates.tar.gz | awk '{print $1}')
echo "Checksum: $SHA256"
# Sign package
openssl dgst -sha256 -sign private-key.pem -out visual-templates.tar.gz.sig visual-templates.tar.gz
# Copy to manifests
cp visual-templates.tar.gz visual-templates.tar.gz.sig manifests/
# Update manifest with checksum
sed -i "s/expected_sha256_hash/$SHA256/" manifests/visual-templates.yaml
