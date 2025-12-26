#!/bin/bash
set -e

# Read secrets
DB_PASS=$(cat /run/secrets/db_password)
WP_ADMIN_PASS=$(cat /run/secrets/wp_admin_password)

# Wait for MariaDB
echo "Waiting for MariaDB..."
for i in {1..30}; do
    if mariadb -h mariadb -u"${MYSQL_USER}" -p"${DB_PASS}" -e "SELECT 1" &>/dev/null; then
        echo "MariaDB is ready!"
        break
    fi
    echo "Attempt $i/30..."
    sleep 2
done

# Wait for Redis
echo "Waiting for Redis..."
for i in {1..30}; do
    if redis-cli -h redis ping &>/dev/null; then
        echo "Redis is ready!"
        break
    fi
    echo "Attempt $i/30..."
    sleep 2
done

# Download WordPress if not exists
if [ ! -f /var/www/html/wp-config.php ]; then
    echo "Installing WordPress..."
    
    cd /var/www/html
    
    # Download WordPress
    wp core download --allow-root
    
    # Create wp-config.php
    wp config create \
        --dbname="${MYSQL_DATABASE}" \
        --dbuser="${MYSQL_USER}" \
        --dbpass="${DB_PASS}" \
        --dbhost=mariadb:3306 \
        --allow-root
    
    # Add Redis configuration to wp-config.php
    echo "Configuring Redis cache..."
    wp config set WP_REDIS_HOST redis --allow-root
    wp config set WP_REDIS_PORT 6379 --raw --allow-root
    wp config set WP_REDIS_TIMEOUT 1 --raw --allow-root
    wp config set WP_REDIS_READ_TIMEOUT 1 --raw --allow-root
    wp config set WP_CACHE true --raw --allow-root
    
    # Install WordPress
    wp core install \
        --url="${WP_URL}" \
        --title="${WP_TITLE}" \
        --admin_user="${WP_ADMIN_USER}" \
        --admin_password="${WP_ADMIN_PASS}" \
        --admin_email="${WP_ADMIN_EMAIL}" \
        --allow-root
    
    # Create second user (non-admin)
    wp user create "${WP_USER}" "${WP_USER_EMAIL}" \
        --role=author \
        --user_pass="${WP_USER_PASSWORD}" \
        --allow-root
    
    # Install and activate Redis Object Cache plugin
    echo "Installing Redis Object Cache plugin..."
    wp plugin install redis-cache --activate --allow-root
    
    # Enable Redis cache
    wp redis enable --allow-root
    
    # Set permissions
    chown -R www-data:www-data /var/www/html
    
    echo "WordPress installation complete with Redis caching!"
else
    echo "WordPress already installed, checking configuration..."
    cd /var/www/html
    
    # ✅ PERMANENT FIX: Always update URLs on startup
    echo "Updating WordPress URLs to ${WP_URL}..."
    wp option update siteurl "${WP_URL}" --allow-root
    wp option update home "${WP_URL}" --allow-root
    
    # Install Redis plugin if not present
    if ! wp plugin is-installed redis-cache --allow-root; then
        echo "Installing Redis Object Cache plugin..."
        wp plugin install redis-cache --activate --allow-root
        wp redis enable --allow-root
    fi
    
    # Ensure Redis is enabled
    wp redis status --allow-root || wp redis enable --allow-root
fi

echo "Starting PHP-FPM..."
exec "$@"
