import machine
import time

# Button pins 0-19
button_pins = list(range(20))
buttons = []

for pin_num in button_pins:
    pin = machine.Pin(pin_num, machine.Pin.IN, machine.Pin.PULL_UP)
    buttons.append(pin)

def read_buttons():
    states = []
    for button in buttons:
        states.append(0 if button.value() == 0 else 1)  # Inverted due to pull-up
    for i in range(4):
        states.append(0)
    return states

# Main loop
while True:
    button_states = read_buttons()
    
    # Send over USB serial (appears on your computer as COM port)
    data = ",".join(map(str, button_states))
    print(data)  
    
    time.sleep_ms(10)  # 100Hz update rate