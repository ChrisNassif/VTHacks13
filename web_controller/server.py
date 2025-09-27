import asyncio
import websockets
import json
import evdev
from datetime import datetime

# Store connected clients
connected_clients = set()

# Map evdev key codes to button names
KEY_MAP = {
    evdev.ecodes.KEY_A: 'a',
    evdev.ecodes.KEY_B: 'b',
    evdev.ecodes.KEY_C: 'c',
    evdev.ecodes.KEY_D: 'd',
    evdev.ecodes.KEY_E: 'e',
    evdev.ecodes.KEY_F: 'f',
    evdev.ecodes.KEY_G: 'g',
    evdev.ecodes.KEY_H: 'h',
    evdev.ecodes.KEY_I: 'i',
    evdev.ecodes.KEY_J: 'j',
    evdev.ecodes.KEY_K: 'k',
    evdev.ecodes.KEY_L: 'l',
    evdev.ecodes.KEY_M: 'm',
    evdev.ecodes.KEY_N: 'n',
    evdev.ecodes.KEY_O: 'o',
    evdev.ecodes.KEY_P: 'p',
    evdev.ecodes.KEY_Q: 'q',
    evdev.ecodes.KEY_R: 'r',
    evdev.ecodes.KEY_S: 's',
    evdev.ecodes.KEY_T: 't',
    evdev.ecodes.BTN_A: 'a',
    evdev.ecodes.BTN_B: 'b',
    evdev.ecodes.BTN_C: 'c',
    evdev.ecodes.BTN_X: 'x',
    evdev.ecodes.KEY_0: '0',
    evdev.ecodes.KEY_1: '1',
    evdev.ecodes.KEY_2: '2',
    evdev.ecodes.KEY_3: '3',
    evdev.ecodes.KEY_4: '4',
    evdev.ecodes.KEY_5: '5',
    evdev.ecodes.KEY_6: '6',
    evdev.ecodes.KEY_7: '7',
}

async def handle_client(websocket):
    connected_clients.add(websocket)
    print(f"[{datetime.now().strftime('%H:%M:%S')}] Client connected from {websocket.remote_address}. Total clients: {len(connected_clients)}")
    
    try:
        await websocket.send(json.dumps({'status': 'connected'}))
        
        async for message in websocket:
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Received: {message}")
            
            try:
                data = json.loads(message)
                if data.get('command') == 'ping':
                    await websocket.send(json.dumps({'response': 'pong'}))
            except json.JSONDecodeError:
                pass
                
    except websockets.exceptions.ConnectionClosedError as e:
        print(f"[{datetime.now().strftime('%H:%M:%S')}] Connection closed with error: {e}")
    except websockets.exceptions.ConnectionClosedOK:
        print(f"[{datetime.now().strftime('%H:%M:%S')}] Client disconnected normally")
    except Exception as e:
        print(f"[{datetime.now().strftime('%H:%M:%S')}] Error: {e}")
    finally:
        connected_clients.discard(websocket)
        print(f"[{datetime.now().strftime('%H:%M:%S')}] Total clients: {len(connected_clients)}")


async def broadcast_button_event(button, pressed):
    if connected_clients:
        message = json.dumps({
            'button': button,
            'pressed': pressed
        })
        
        disconnected = set()
        for client in connected_clients:
            try:
                await client.send(message)
            except:
                disconnected.add(client)
        
        connected_clients.difference_update(disconnected)
        
        state = "PRESSED" if pressed else "RELEASED"
        print(f"[{datetime.now().strftime('%H:%M:%S')}] Broadcast: Button {button.upper()} {state}")


def find_foxx_controller():
    """Find the F0XX Controller device"""
    devices = [evdev.InputDevice(path) for path in evdev.list_devices()]
    
    for device in devices:
        if device.name == "F0XX Controller":
            return device
    
    return None


async def read_evdev_events():
    """Read events from evdev and broadcast to WebSocket clients"""
    while True:  # Continuous reconnection loop
        print("Looking for F0XX Controller...")
        
        device = find_foxx_controller()
        
        if device is None:
            print("ERROR: F0XX Controller not found!")
            print("Available devices:")
            for dev in [evdev.InputDevice(path) for path in evdev.list_devices()]:
                print(f"  - {dev.name}")
            print("Retrying in 5 seconds...")
            await asyncio.sleep(5)
            continue
        
        print(f"Found F0XX Controller: {device.name}")
        print(f"Device path: {device.path}")
        print("Listening for button events...\n")
        
        try:
            async for event in device.async_read_loop():
                if event.type == evdev.ecodes.EV_KEY:
                    if event.code in KEY_MAP:
                        button = KEY_MAP[event.code]
                        pressed = event.value == 1
                        
                        await broadcast_button_event(button, pressed)
        except OSError as e:
            if e.errno == 19:  # Device disconnected
                print(f"[{datetime.now().strftime('%H:%M:%S')}] Controller disconnected! Attempting to reconnect...")
                await asyncio.sleep(2)  # Wait before retrying
            else:
                print(f"Error reading from device: {e}")
                await asyncio.sleep(2)
        except Exception as e:
            print(f"Error reading from device: {e}")
            await asyncio.sleep(2)


async def main():
    """Start the WebSocket server and evdev reader"""
    print("=" * 50)
    print("Game Controller WebSocket Server (evdev)")
    print("=" * 50)
    print("Starting server on localhost:8080")
    print("\nPress Ctrl+C to stop the server")
    print("=" * 50 + "\n")
    
    # Start the WebSocket server
    async with websockets.serve(handle_client, "localhost", 8080):
        # Start evdev reading task
        evdev_task = asyncio.create_task(read_evdev_events())
        
        try:
            await asyncio.Future()  # Run forever
        except KeyboardInterrupt:
            print("\n\nShutting down server...")
            evdev_task.cancel()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped.")