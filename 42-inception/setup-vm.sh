#!/bin/bash
# VirtualBox VM Setup Helper for Inception Project
# Run this on your HOST machine (not in a VM)

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Inception VM Setup Helper ===${NC}"
echo ""

# Configuration
VM_NAME="inception-yom"
OS_TYPE="Debian_64"
RAM_MB=4096  # 4GB, increase to 8192 if you have RAM to spare
CPU_COUNT=2
VRAM_MB=16
DISK_SIZE_MB=30720  # 30GB

# Ask for external disk path
echo -e "${YELLOW}Where is your external disk mounted?${NC}"
echo "Examples:"
echo "  Linux: /media/yom/EXTERNAL_DISK"
echo "  macOS: /Volumes/EXTERNAL_DISK"
echo "  Windows (Git Bash): /d/VirtualBox VMs"
read -p "Enter path: " EXTERNAL_DISK

if [ ! -d "$EXTERNAL_DISK" ]; then
    echo -e "${RED}Error: Directory $EXTERNAL_DISK not found!${NC}"
    echo "Please check the path and try again."
    exit 1
fi

VM_FOLDER="$EXTERNAL_DISK/VirtualBox-VMs"
mkdir -p "$VM_FOLDER"

echo ""
echo -e "${GREEN}Configuration:${NC}"
echo "  VM Name: $VM_NAME"
echo "  RAM: ${RAM_MB}MB"
echo "  CPUs: $CPU_COUNT"
echo "  Disk: ${DISK_SIZE_MB}MB (~30GB)"
echo "  Location: $VM_FOLDER"
echo ""

read -p "Continue? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 0
fi

# Check if VBoxManage exists
if ! command -v VBoxManage &> /dev/null; then
    echo -e "${RED}Error: VBoxManage not found. Is VirtualBox installed?${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}Step 1: Creating VM...${NC}"
VBoxManage createvm \
    --name "$VM_NAME" \
    --ostype "$OS_TYPE" \
    --basefolder "$VM_FOLDER" \
    --register

echo -e "${GREEN}Step 2: Configuring VM settings...${NC}"
VBoxManage modifyvm "$VM_NAME" \
    --memory $RAM_MB \
    --cpus $CPU_COUNT \
    --vram $VRAM_MB \
    --nic1 bridged \
    --bridgeadapter1 "$(VBoxManage list bridgedifs | grep '^Name:' | head -1 | awk '{print $2}')" \
    --boot1 disk \
    --boot2 dvd \
    --boot3 none \
    --boot4 none \
    --audio none \
    --usb on \
    --usbehci on \
    --clipboard bidirectional \
    --draganddrop bidirectional

echo -e "${GREEN}Step 3: Creating virtual hard disk...${NC}"
VBoxManage createhd \
    --filename "$VM_FOLDER/$VM_NAME/$VM_NAME.vdi" \
    --size $DISK_SIZE_MB \
    --variant Standard

echo -e "${GREEN}Step 4: Adding storage controllers...${NC}"
VBoxManage storagectl "$VM_NAME" \
    --name "SATA Controller" \
    --add sata \
    --controller IntelAhci \
    --portcount 2 \
    --bootable on

VBoxManage storageattach "$VM_NAME" \
    --storagectl "SATA Controller" \
    --port 0 \
    --device 0 \
    --type hdd \
    --medium "$VM_FOLDER/$VM_NAME/$VM_NAME.vdi"

echo ""
echo -e "${GREEN}✓ VM created successfully!${NC}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo ""
echo "1. Download Debian 12 (Bookworm) ISO:"
echo "   https://www.debian.org/download"
echo ""
echo "2. Attach the ISO to the VM:"
echo "   VBoxManage storageattach \"$VM_NAME\" \\"
echo "     --storagectl \"SATA Controller\" \\"
echo "     --port 1 --device 0 --type dvddrive \\"
echo "     --medium /path/to/debian-12.x.x-amd64-netinst.iso"
echo ""
echo "3. Start the VM:"
echo "   VBoxManage startvm \"$VM_NAME\""
echo ""
echo "4. Install Debian:"
echo "   - Hostname: inception"
echo "   - Domain: (leave empty)"
echo "   - Root password: (set securely)"
echo "   - User: yom"
echo "   - Password: (set securely)"
echo "   - Partitioning: Guided - use entire disk"
echo "   - Software selection:"
echo "     [*] SSH server"
echo "     [*] Standard system utilities"
echo "     [ ] Debian desktop environment (uncheck)"
echo ""
echo "5. After installation, in the VM run:"
echo "   sudo apt update && sudo apt install -y docker.io docker-compose make git vim"
echo "   sudo usermod -aG docker yom"
echo "   logout"
echo ""
echo "6. Then follow DAILY-CHECKLIST.md Day 1 tasks!"
echo ""
echo -e "${GREEN}VM Location: $VM_FOLDER/$VM_NAME${NC}"
echo ""
echo "To remove the VM later (if needed):"
echo "  VBoxManage unregistervm \"$VM_NAME\" --delete"
echo ""
