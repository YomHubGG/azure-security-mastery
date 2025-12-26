COMPOSE_FILE = srcs/docker-compose.yml
ENV_FILE = srcs/.env
DATA_DIR = /home/ggrisole/data

all: setup build up

setup:
	@echo "Creating data directories..."
	@mkdir -p $(DATA_DIR)/mariadb
	@mkdir -p $(DATA_DIR)/wordpress
	@grep -q "ggrisole.42.fr" /etc/hosts || echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts

build:
	@echo "Building Docker images..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) build

up:
	@echo "Starting services..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) up -d

down:
	@echo "Stopping services..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) down

clean: down
	@echo "Cleaning containers and images..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) down -v --rmi all 2>/dev/null || true

fclean: clean
	@echo "Removing data directories..."
	@sudo rm -rf $(DATA_DIR)

re: fclean all

logs:
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) logs -f

ps:
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) ps

.PHONY: all setup build up down clean fclean re logs ps
