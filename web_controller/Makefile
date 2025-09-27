PYTHON := python3
SERVER_SCRIPT := server.py
HTML_FILE := index.html
SERVICE_NAME := game-controller
SERVICE_FILE := $(SERVICE_NAME).service
SYSTEMD_USER_DIR := $(HOME)/.config/systemd/user
CURRENT_DIR := $(shell pwd)


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
	@echo "ExecStart=$(PYTHON) $(CURRENT_DIR)/$(SERVER_SCRIPT)" >> $(SERVICE_FILE)
	@echo "Restart=on-failure" >> $(SERVICE_FILE)
	@echo "RestartSec=5" >> $(SERVICE_FILE)
	@echo "" >> $(SERVICE_FILE)
	@echo "[Install]" >> $(SERVICE_FILE)
	@echo "WantedBy=default.target" >> $(SERVICE_FILE)
	@echo "Service file created: $(SERVICE_FILE)"

# Install systemd service and enable it
install: $(SERVICE_FILE)
	@echo "Installing systemd user service..."
	@mkdir -p $(SYSTEMD_USER_DIR)
	@cp $(SERVICE_FILE) $(SYSTEMD_USER_DIR)/
	@systemctl --user daemon-reload
	@echo "Enabling service to start at boot..."
	@systemctl --user enable $(SERVICE_NAME).service
	@systemctl --user start $(SERVICE_NAME).service
	@echo "Service installed, enabled, and started!"
	@echo "Run 'make status' to check service status"

# Check service status
status:
	@systemctl --user status $(SERVICE_NAME).service

# Clean up - remove service and generated files
clean:
	@echo "Cleaning up..."
	@systemctl --user stop $(SERVICE_NAME).service 2>/dev/null || true
	@systemctl --user disable $(SERVICE_NAME).service 2>/dev/null || true
	@rm -f $(SYSTEMD_USER_DIR)/$(SERVICE_FILE)
	@rm -f $(SERVICE_FILE)
	@systemctl --user daemon-reload 2>/dev/null || true
	@echo "Clean complete!"