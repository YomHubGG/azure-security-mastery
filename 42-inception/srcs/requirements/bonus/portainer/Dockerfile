FROM debian:bookworm

RUN apt-get update && apt-get install -y     ca-certificates     wget     curl     && rm -rf /var/lib/apt/lists/*

# Install docker-compose (required by Portainer v2.19.4)
RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose &&     chmod +x /usr/local/bin/docker-compose

# Download and extract Portainer (includes binary + web UI files)
RUN wget https://github.com/portainer/portainer/releases/download/2.19.4/portainer-2.19.4-linux-amd64.tar.gz &&     tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/ &&     rm portainer-2.19.4-linux-amd64.tar.gz

WORKDIR /usr/local/bin/portainer

VOLUME /data

EXPOSE 9000

CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
