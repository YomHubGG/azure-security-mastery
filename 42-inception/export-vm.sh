#!/bin/bash

set -e

VM_NAME="inception"
EXPORT_DIR="$HOME/inception-export"
EXPORT_FILE="${EXPORT_DIR}/inception-$(date +%Y%m%d).ova"

echo "=========================================="
echo "VM Export Script for 42 School Machines"
echo "=========================================="
echo ""

mkdir -p "$EXPORT_DIR"

echo "[1/4] Checking VM state..."
VM_STATE=$(VBoxManage showvminfo "$VM_NAME" | grep "State:" | awk '{print $2}')
if [ "$VM_STATE" == "running" ]; then
    echo "⚠ VM is running. Powering off..."
    VBoxManage controlvm "$VM_NAME" acpipowerbutton
    sleep 10
    VBoxManage controlvm "$VM_NAME" poweroff 2>/dev/null || true
    sleep 5
else
    echo "✓ VM is powered off (ready for export)"
fi

echo ""
echo "[2/4] Compacting VM disk (reduces size)..."
VBoxManage modifymedium disk /mnt/vmlab/inception/inception.vdi --compact

echo ""
echo "[3/4] Exporting VM to OVA format..."
echo "This may take 5-10 minutes..."
VBoxManage export "$VM_NAME" \
    --output "$EXPORT_FILE" \
    --options manifest,nomacs \
    --vsys 0 \
    --product "Inception - Docker Infrastructure Project" \
    --producturl "https://github.com/42-inception" \
    --vendor "42 School" \
    --vendorurl "https://42.fr" \
    --version "1.0"

echo ""
echo "[4/4] Verifying export..."
if [ -f "$EXPORT_FILE" ]; then
    FILE_SIZE=$(du -h "$EXPORT_FILE" | awk '{print $1}')
    echo "✓ Export successful!"
    echo ""
    echo "=========================================="
    echo "Export Complete"
    echo "=========================================="
    echo "Location: $EXPORT_FILE"
    echo "Size: $FILE_SIZE"
    echo ""
    echo "To use on school machines:"
    echo "1. Copy this file to a USB drive or upload to cloud"
    echo "2. On school machine: VirtualBox > File > Import Appliance"
    echo "3. Select the .ova file and import"
    echo ""
    echo "MD5 checksum (for integrity verification):"
    md5sum "$EXPORT_FILE"
else
    echo "✗ Export failed!"
    exit 1
fi
