#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <linux/uinput.h>
#include <sys/epoll.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>




// stores all of the current button states (1 for pressed, 0 for not pressed)
// each bit is an individual value
char current_button_states[4] = {};


// these are the possible buttons types that the controller can output depending on the bits that are sent to this driver from the UART connection
// for the 32 bit value that the controller sends us, the i'th bit being 1 means that the i'th element of this array is currently being pressed   
unsigned long CONTROLLER_BUTTON_TYPES[32] = {
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    BTN_X,
    BTN_A,
    BTN_B,
    BTN_C,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
};
int NUMBER_OF_CONTROLLER_BUTTON_TYPES = sizeof(CONTROLLER_BUTTON_TYPES) / sizeof(unsigned long);


bool is_file_descriptor_open(int fd) {
    if (fcntl(fd, F_GETFL) == -1) {
        return errno != EBADF; 
    }
    return true;
}


int setup_uart(const char* device_file_path) {
    int fd = open(device_file_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) throw std::runtime_error("Failed to open UART");

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) throw std::runtime_error("tcgetattr failed");

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);

    tty.c_iflag = 0;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) throw std::runtime_error("tcsetattr failed");

    return fd;
}


// Create a virtual gamepad via uinput
int setup_uinput() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) throw std::runtime_error("Failed to open /dev/uinput");

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    
    // add all of the controller buttons as potential uinput buttons
    for (int i = 0; i < NUMBER_OF_CONTROLLER_BUTTON_TYPES; i++) {
        ioctl(fd, UI_SET_KEYBIT, CONTROLLER_BUTTON_TYPES[i]);
    }

    // Device info
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));

    // Setup the F0XX Controller to look like a Microsoft Xbox 360 Controller 
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x045e;   
    usetup.id.product = 0x028e;
    strcpy(usetup.name, "F0XX Controller");

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
    int uart_fd = setup_uart("/dev/F0XX");

    // int uart_fd = setup_uart("/dev/F0XX");
    int uinput_fd = setup_uinput();

    int epfd = epoll_create1(0);

    if (epfd < 0) {
        throw std::runtime_error("epoll_create failed");
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = uart_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, uart_fd, &ev);


    std::cout << "Driver running: press bytes over UART to control gamepad" << std::endl;

    try {
        while (true) {
            struct epoll_event events[1];

            int n = epoll_wait(epfd, events, 1, 1000);
            
            if (!is_file_descriptor_open(uart_fd)) {
                throw std::runtime_error("Device Disconnected");
            }

            if (!(n > 0 && (events[0].events & EPOLLIN))) break;
            
            char uart_buffer[4];
            int len = read(uart_fd, uart_buffer, sizeof(uart_buffer));
            
            if (len == 0) {
                throw std::runtime_error("Device Disconnected");
            }
            
            // std::cout << "num:" << len << std::endl;
        
            for (int i = 0; i < len; i++) {
                std::cout << "byte[" << i << "] = " 
                        << static_cast<int>(static_cast<unsigned char>(uart_buffer[i])) 
                        << std::endl;
            }


            // go through the UART bits and then send a key event if the bit was changed
            for (int byte_index = 0; byte_index < 4; byte_index++) {
                char changed_buttons = current_button_states[byte_index] ^ uart_buffer[byte_index];

                for (int bit_index = 0; bit_index < 8; bit_index++) {
                    char mask = 1 << bit_index;

                    if ((changed_buttons & mask) && (byte_index * 8 + bit_index < 20)) {
                        send_event(uinput_fd, EV_KEY, CONTROLLER_BUTTON_TYPES[byte_index * 8 + bit_index], (uart_buffer[byte_index] & mask) >> bit_index);
                        sync_events(uinput_fd);

                        current_button_states[byte_index] = uart_buffer[byte_index];
                    }
                }
            }
            
        }
    }

    catch (std::runtime_error e) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        close(uart_fd);
    } 


    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
    close(uart_fd);

}
