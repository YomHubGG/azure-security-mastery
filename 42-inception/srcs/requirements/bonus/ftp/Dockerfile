FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    vsftpd \
    && rm -rf /var/lib/apt/lists/*

COPY tools/init-ftp.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/init-ftp.sh

COPY conf/vsftpd.conf /etc/vsftpd.conf

RUN mkdir -p /var/run/vsftpd/empty && \
    chmod 755 /var/run/vsftpd/empty

EXPOSE 21 21000-21010

ENTRYPOINT ["/usr/local/bin/init-ftp.sh"]
CMD ["vsftpd", "/etc/vsftpd.conf"]
