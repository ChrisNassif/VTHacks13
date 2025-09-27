sudo chmod 777 /etc/udev/rules.d/99-F0XX-controller-udev.rules
sudo echo 'ACTION=="add", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000a", SYMLINK+="F0XX", MODE="0666"' > /etc/udev/rules.d/99-F0XX-controller-udev.rules
sudo udevadm control --reload
sudo udevadm trigger

# sudo apt install linux-headers-$(uname -r)
