import evdev

devices = [evdev.InputDevice(path) for path in evdev.list_devices()]

f0xx_controller_device = None

for device in devices:
   if device.name == "F0XX Controller":
       f0xx_controller_device = device

if f0xx_controller_device == None:
    print("No F0XX Controller Found")
    exit()

for event in f0xx_controller_device.read_loop():
    if event.type == evdev.ecodes.EV_KEY:
        print(evdev.categorize(event))
