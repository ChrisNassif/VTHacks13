sudo echo ACTION=="add", ATTRS{idVendor}=="2E8A", ATTRS{idProduct}=="0005", SYMLINK+="F0XX" > /etc/udev/rules.d/99-F0XX-controller-udev.rules
sudo udevadm control --reload
sudo udevadm trigger