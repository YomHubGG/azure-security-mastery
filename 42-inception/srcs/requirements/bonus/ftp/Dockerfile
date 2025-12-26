FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    vsftpd \
    && rm -rf /var/lib/apt/lists/*

# Create FTP user that will access WordPress files
RUN useradd -m ftpuser && \
    echo "ftpuser:ftppass" | chpasswd

COPY conf/vsftpd.conf /etc/vsftpd.conf

# Create necessary directories
RUN mkdir -p /var/run/vsftpd/empty && \
    chmod 755 /var/run/vsftpd/empty

EXPOSE 21 21000-21010

CMD ["vsftpd", "/etc/vsftpd.conf"]
