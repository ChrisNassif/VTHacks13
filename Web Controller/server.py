import asyncio
import websockets
import json
import random
from datetime import datetime

# Store connected clients
connected_clients = set()

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


async def broadcast_button_press(button, pressed=True):
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


async def simulate_button_presses():
    buttons = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
               'n', 'o', 'p', 'q', 'r', 's', 't']
    
    print("Waiting 3 seconds before starting simulation...")
    await asyncio.sleep(3)
    
    print("Starting button press simulation...")
    while True:
        if connected_clients:
            button = random.choice(buttons)
            
            await broadcast_button_press(button, True)
            
            await asyncio.sleep(random.uniform(0.1, 0.5))
            
            await broadcast_button_press(button, False)
            
            await asyncio.sleep(random.uniform(0.3, 1.5))
        else:
            await asyncio.sleep(1)


async def main():
    """Start the WebSocket server"""
    print("=" * 50)
    print("Game Controller WebSocket Server")
    print("=" * 50)
    print("Waiting for connections...")
    print("\nPress Ctrl+C to stop the server")
    print("=" * 50)
    
    # Start the WebSocket server
    async with websockets.serve(handle_client, "localhost", 8080):
        # Start simulation task
        simulation_task = asyncio.create_task(simulate_button_presses())
        
        try:
            await asyncio.Future()  # Run forever
        except KeyboardInterrupt:
            print("\n\nShutting down server...")
            simulation_task.cancel()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped.")