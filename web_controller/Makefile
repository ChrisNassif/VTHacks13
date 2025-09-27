PYTHON := python3
SERVER_SCRIPT := server.py
HTML_FILE := index.html
SERVICE_NAME := game-controller
SERVICE_FILE := $(SERVICE_NAME).service
SYSTEMD_DIR := /etc/systemd/system
CURRENT_DIR := $(shell pwd)
PYTHON_PATH := $(shell which python3)


.PHONY: open server install status clean

# Open HTML file in browser
OS := $(shell uname 2>/dev/null || echo Windows)

ifeq ($(OS),Darwin) # macOS
    OPEN_CMD = open
else ifeq ($(OS),Linux) # Linux
    OPEN_CMD = xdg-open
else # Windows
    OPEN_CMD = cmd.exe /c start
endif

open:
	@echo "Opening $(HTML_FILE) in default browser..."
	@$(OPEN_CMD) $(HTML_FILE)

# Start the Python server
server:
	@echo "Starting WebSocket server..."
	@$(PYTHON) $(SERVER_SCRIPT)

# Generate systemd service file
$(SERVICE_FILE):
	@echo "Generating systemd service file..."
	@echo "[Unit]" > $(SERVICE_FILE)
	@echo "Description=Game Controller WebSocket Server" >> $(SERVICE_FILE)
	@echo "After=network.target" >> $(SERVICE_FILE)
	@echo "" >> $(SERVICE_FILE)
	@echo "[Service]" >> $(SERVICE_FILE)
	@echo "Type=simple" >> $(SERVICE_FILE)
	@echo "WorkingDirectory=$(CURRENT_DIR)" >> $(SERVICE_FILE)
	@echo "ExecStart=$(PYTHON_PATH) $(CURRENT_DIR)/$(SERVER_SCRIPT)" >> $(SERVICE_FILE)
	@echo "Restart=always" >> $(SERVICE_FILE)
	@echo "RestartSec=5" >> $(SERVICE_FILE)
	@echo "StandardOutput=journal" >> $(SERVICE_FILE)
	@echo "StandardError=journal" >> $(SERVICE_FILE)
	@echo "" >> $(SERVICE_FILE)
	@echo "[Install]" >> $(SERVICE_FILE)
	@echo "WantedBy=default.target" >> $(SERVICE_FILE)
	@echo "Service file created: $(SERVICE_FILE)"

# Install systemd service and enable it
install: $(SERVICE_FILE)
	@echo "===== Installing Python dependencies ====="
	@if $(PYTHON) -m pip --version >/dev/null 2>&1; then \
		echo "Using pip to install packages..."; \
		$(PYTHON) -m pip install --user websockets evdev; \
	else \
		echo "pip not found, using apt to install packages..."; \
		sudo apt update && sudo apt install -y python3-websockets python3-evdev; \
	fi
	@echo "Dependencies installed!"
	@echo ""
	@echo "===== Installing systemd system service ====="
	@sudo mkdir -p $(SYSTEMD_DIR)
	@sudo cp $(SERVICE_FILE) $(SYSTEMD_DIR)/
	@sudo systemctl daemon-reload
	@echo "Enabling service to start at boot..."
	@sudo systemctl enable $(SERVICE_NAME).service
	@sudo systemctl start $(SERVICE_NAME).service
	@echo ""
	@echo "Service installed, enabled, and started!"
	@echo "Run 'make status' to check service status"

# Check service status
status:
	@sudo systemctl status $(SERVICE_NAME).service

# Clean up - remove service and generated files
clean:
	@echo "Cleaning up..."
	@sudo systemctl stop $(SERVICE_NAME).service 2>/dev/null || true
	@sudo systemctl disable $(SERVICE_NAME).service 2>/dev/null || true
	@sudo rm -f $(SYSTEMD_DIR)/$(SERVICE_FILE)
	@rm -f $(SERVICE_FILE)
	@sudo systemctl daemon-reload 2>/dev/null || true
	@echo "Clean complete!"