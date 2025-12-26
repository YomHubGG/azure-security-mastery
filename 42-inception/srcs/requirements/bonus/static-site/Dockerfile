FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    nginx \
    && rm -rf /var/lib/apt/lists/*

# Create static website
COPY tools/index.html /var/www/html/index.html
COPY tools/style.css /var/www/html/style.css

# Configure NGINX for static site
RUN echo 'server { \n\
    listen 8080; \n\
    server_name _; \n\
    root /var/www/html; \n\
    index index.html; \n\
    location / { \n\
        try_files $uri $uri/ =404; \n\
    } \n\
}' > /etc/nginx/sites-available/default

EXPOSE 8080

CMD ["nginx", "-g", "daemon off;"]
