FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    wget \
    php-fpm \
    php-mysql \
    php-mysqli \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /var/www/html

RUN wget "https://github.com/vrana/adminer/releases/download/v4.8.1/adminer-4.8.1.php" -O adminer.php && \
    chmod 644 adminer.php

RUN sed -i 's/listen = \/run\/php\/php.*-fpm.sock/listen = 9001/' /etc/php/*/fpm/pool.d/www.conf && \
    mkdir -p /run/php

EXPOSE 9001

CMD ["php-fpm8.2", "-F"]
