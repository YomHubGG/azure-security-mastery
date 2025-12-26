#!/bin/bash
set -e

# Read secrets and strip newlines
DB_ROOT_PASS=$(cat /run/secrets/db_root_password | tr -d '\n')
DB_PASS=$(cat /run/secrets/db_password | tr -d '\n')

# Initialize database if not exists
if [ ! -d "/var/lib/mysql/mysql" ]; then
    echo "Initializing MariaDB database..."
    mysql_install_db --user=mysql --datadir=/var/lib/mysql
    
    # Start temporary server
    mysqld --user=mysql --datadir=/var/lib/mysql --skip-networking &
    pid="$!"
    
    # Wait for server to start
    for i in {1..30}; do
        if mysqladmin ping &>/dev/null; then
            break
        fi
        sleep 1
    done
    
    # Create database and user BEFORE setting root password
    echo "Creating database and user..."
    mysql -e "CREATE DATABASE IF NOT EXISTS ${MYSQL_DATABASE};"
    mysql -e "CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASS}';"
    mysql -e "GRANT ALL PRIVILEGES ON ${MYSQL_DATABASE}.* TO '${MYSQL_USER}'@'%';"
    mysql -e "FLUSH PRIVILEGES;"
    
    # NOW set root password and secure
    mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASS}';"
    mysql -uroot -p"${DB_ROOT_PASS}" -e "DELETE FROM mysql.user WHERE User='';"
    
    # Stop temporary server
    mysqladmin -uroot -p"${DB_ROOT_PASS}" shutdown
    wait "$pid"
    
    echo "MariaDB initialization complete!"
fi

echo "Starting MariaDB..."
exec "$@"
