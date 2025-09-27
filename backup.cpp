#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <linux/uinput.h>
#include <sys/epoll.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

// Configure the UART (ttyUSB0)
int setup_uart(const char* device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) throw std::runtime_error("Failed to open UART");

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) throw std::runtime_error("tcgetattr failed");

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // disable break
    tty.c_lflag = 0;                            // no signaling chars, no echo
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN]  = 1;                        // read blocks for 1 byte
    tty.c_cc[VTIME] = 1;                        // timeout 0.1s

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // no SW flow ctrl
    tty.c_cflag |= (CLOCAL | CREAD);            // enable read
    tty.c_cflag &= ~(PARENB | PARODD);          // no parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) throw std::runtime_error("tcsetattr failed");

    return fd;
}

// Create a virtual gamepad via uinput
int setup_uinput() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) throw std::runtime_error("Failed to open /dev/uinput");

    // Enable events
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_A);
    ioctl(fd, UI_SET_KEYBIT, BTN_B);

    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);

    // Set axis ranges
    struct uinput_abs_setup abs_setup;
    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_X;
    abs_setup.absinfo.minimum = -32768;
    abs_setup.absinfo.maximum = 32767;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    abs_setup.code = ABS_Y;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    // Device info
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "UART Virtual Gamepad");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    return fd;
}

// Send input event
void send_event(int ufd, unsigned short type, unsigned short code, int value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&ev.time, nullptr);
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(ufd, &ev, sizeof(ev));
}

// Sync report
void sync_events(int ufd) {
    send_event(ufd, EV_SYN, SYN_REPORT, 0);
}

int main() {
    try {
        int uart_fd = setup_uart("/dev/ttyUSB0");
        int uinput_fd = setup_uinput();

        int epfd = epoll_create1(0);
        if (epfd < 0) throw std::runtime_error("epoll_create1 failed");

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = uart_fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, uart_fd, &ev);

        std::cout << "Driver running: press bytes over UART to control gamepad" << std::endl;

        while (true) {
            struct epoll_event events[1];
            int n = epoll_wait(epfd, events, 1, -1);
            if (n > 0 && (events[0].events & EPOLLIN)) {
                char buf[64];
                int len = read(uart_fd, buf, sizeof(buf));
                for (int i = 0; i < len; i++) {
                    // Map simple protocol: 'A' = press A, 'a' = release A
                    if (buf[i] == 'A') {
                        send_event(uinput_fd, EV_KEY, BTN_A, 1);
                        sync_events(uinput_fd);
                    } else if (buf[i] == 'a') {
                        send_event(uinput_fd, EV_KEY, BTN_A, 0);
                        sync_events(uinput_fd);
                    } else if (buf[i] == 'B') {
                        send_event(uinput_fd, EV_KEY, BTN_B, 1);
                        sync_events(uinput_fd);
                    } else if (buf[i] == 'b') {
                        send_event(uinput_fd, EV_KEY, BTN_B, 0);
                        sync_events(uinput_fd);
                    }
                }
            }
        }

        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        close(uart_fd);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
