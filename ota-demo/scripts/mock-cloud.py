import http.client
import os

def push_update(manifest_path):
    conn = http.client.HTTPConnection("ankaios-server", 25551)
    with open(manifest_path, 'rb') as f:
        conn.request("POST", "/api/v1/workloads", f.read())
    response = conn.getresponse()
    print(f"Push status: {response.status} {response.reason}")

if __name__ == "__main__":
    push_update("/manifests/bootloader.yaml")
    push_update("/manifests/visual-templates.yaml")
