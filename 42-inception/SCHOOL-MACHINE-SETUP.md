# Inception VM - School Machine Setup Guide

## Export Information
- **File**: inception-20251229.ova
- **Size**: 4.2 GB
- **MD5**: df71d72ccfd7faa7f3833bcb52337160
- **Format**: OVA (Open Virtualization Appliance)

## Transferring to School

### Option 1: USB Drive
```bash
# Copy to USB
cp ~/inception-export/inception-20251229.ova /media/usb/

# Verify copy
md5sum /media/usb/inception-20251229.ova
# Should match: df71d72ccfd7faa7f3833bcb52337160
```

### Option 2: Cloud Upload (if permitted)
```bash
# Using scp to school storage (if available)
scp ~/inception-export/inception-20251229.ova login@school-server:~/

# Or use cloud service
# Note: Check 42 school policies on cloud usage
```

## Importing on School Machine

### Step 1: Copy File Locally (if from USB)
```bash
cp /media/usb/inception-20251229.ova ~/
```

### Step 2: Import in VirtualBox
```bash
# Command line method
VBoxManage import ~/inception-20251229.ova

# Or GUI method:
# 1. Open VirtualBox
# 2. File > Import Appliance
# 3. Select inception-20251229.ova
# 4. Review settings (RAM: 4GB, Disk: ~30GB)
# 5. Click Import (takes 2-5 minutes)
```

### Step 3: Start VM
```bash
VBoxManage startvm inception

# Or from VirtualBox GUI
```

## Inside the VM

### Default Credentials
- **Username**: Check your setup (likely `ggrisole` or your login)
- **Password**: Your VM password

### Project Location
```bash
cd ~/inception
# or
cd /home/ggrisole/inception
```

### Running the Project
```bash
cd inception
make

# Wait 30-60 seconds for all services to start
docker ps  # Should show 8 containers

# Add to /etc/hosts if not done
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts

# Access the site
firefox https://ggrisole.42.fr
```

## Quick Verification Commands

```bash
# Check all containers running
docker ps

# Check volumes exist
docker volume ls | grep inception

# Check network
docker network ls | grep inception

# Test WordPress
curl -k https://ggrisole.42.fr

# Check database
docker exec mariadb mysqladmin ping

# View logs
docker logs nginx
docker logs wordpress
docker logs mariadb
```

## Troubleshooting

### VM Won't Start
- Check VirtualBox is installed: `VBoxManage --version`
- Check virtualization enabled in BIOS
- Try starting from GUI instead of command line

### Import Fails
- Verify MD5 checksum matches
- Check available disk space (need ~35GB free)
- Try importing from VirtualBox GUI

### Docker Not Working
```bash
# Check Docker status
sudo systemctl status docker

# Start Docker if stopped
sudo systemctl start docker

# Add user to docker group (if needed)
sudo usermod -aG docker $USER
newgrp docker
```

### Network Issues
```bash
# Check /etc/hosts
cat /etc/hosts | grep ggrisole.42.fr

# Add if missing
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts

# Restart network
sudo systemctl restart NetworkManager
```

## VM Configuration

The exported VM includes:
- ✓ Debian Bookworm OS
- ✓ Docker and Docker Compose installed
- ✓ Project files in ~/inception
- ✓ Secrets configured
- ✓ Data directories set up
- ✓ All dependencies installed

## Performance Notes

**Recommended Host Requirements**:
- 8GB RAM (VM uses 4GB)
- 50GB free disk space
- CPU with virtualization support (VT-x/AMD-V)
- VirtualBox 6.1+ or 7.0+

**School Machine Considerations**:
- May need to adjust RAM if school machines have limited memory
- Disk I/O may be slower on shared storage
- Network access may vary by campus

## Evaluation Day Checklist

Before the evaluator arrives:
```bash
# 1. Start VM
VBoxManage startvm inception

# 2. Login and start project
cd inception
make fclean  # Clean start
make         # Build and start

# 3. Wait for all services
sleep 60

# 4. Verify everything works
docker ps
firefox https://ggrisole.42.fr

# 5. Be ready to explain
# - Docker vs VM
# - Each service's purpose
# - Network configuration
# - Volume persistence
```

## Quick Commands Reference

```bash
# VM Control
VBoxManage startvm inception           # Start
VBoxManage controlvm inception pause   # Pause
VBoxManage controlvm inception resume  # Resume
VBoxManage controlvm inception poweroff # Stop

# Project Management
make                # Start everything
make down           # Stop services
make fclean         # Complete cleanup
make re             # Rebuild everything
make logs           # View all logs

# Docker
docker ps           # List containers
docker logs <name>  # View container logs
docker exec -it <name> bash  # Enter container
docker volume ls    # List volumes
docker network ls   # List networks
```

## Support

If issues arise during evaluation:
1. Check logs: `make logs` or `docker logs <service>`
2. Restart services: `make restart`
3. Full rebuild: `make fclean && make`
4. Check documentation: README.md, USER_DOC.md, DEV_DOC.md

---

**Created**: December 29, 2025
**VM Export**: inception-20251229.ova
**Status**: ✅ Ready for evaluation

Good luck with your defense! 🚀
