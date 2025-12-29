#!/bin/bash
set -e

FTP_USER="${FTP_USER:-ftpuser}"
FTP_PASS="${FTP_PASS:-ftppass123}"

if ! id "$FTP_USER" &>/dev/null; then
    echo "Creating FTP user: $FTP_USER"
    useradd -m "$FTP_USER"
    echo "$FTP_USER:$FTP_PASS" | chpasswd
    
    chown -R "$FTP_USER:$FTP_USER" /var/www/html
fi

echo "Starting vsftpd..."
exec "$@"
