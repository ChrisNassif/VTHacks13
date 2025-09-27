BIN_DIR = /usr/bin
SERVICE_DIR = /etc/systemd/system

all:
	sudo g++ -Ofast foxx_driver.cpp -o $(BIN_DIR)/foxx_driver.o

test:
	sudo g++ -Ofast foxx_test_driver.cpp -o $(BIN_DIR)/foxx_test_driver.o

install: all
	echo "[*] Installing systemd service..."
	echo "[Unit]"                                    | sudo tee $(SERVICE_DIR)/foxx-driver.service
	echo "Description=F0XX Game Controller Driver"  | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "After=dev-F0XX.device"                    | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "Wants=dev-F0XX.device"                    | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo ""                                         | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "[Service]"                                | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "Type=simple"                              | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "ExecStart=$(BIN_DIR)/foxx_driver.o"       | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "Restart=always"                           | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "RestartSec=1"                             | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo ""                                         | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "[Install]"                                | sudo tee -a $(SERVICE_DIR)/foxx-driver.service
	echo "WantedBy=multi-user.target"               | sudo tee -a $(SERVICE_DIR)/foxx-driver.service

	sudo systemctl daemon-reload
	sudo systemctl enable foxx-driver.service
	sudo systemctl start foxx-driver.service

clean:
	sudo rm -f $(BIN_DIR)/foxx_driver.o
	sudo rm -f $(BIN_DIR)/foxx_test_driver.o
	sudo rm -f $(SERVICE_DIR)/foxx-driver.service
	sudo systemctl disable --now foxx-driver.service || true