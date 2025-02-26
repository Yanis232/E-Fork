import asyncio
import writeFiles
import time
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "ESP32_Sensor"
CHARACTERISTIC_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  # UUID de la caractéristique
id = writeFiles.readSaves("sauvegarde.txt")


async def find_device_by_name():
    devices = await BleakScanner.discover()
    for device in devices:
        if device.name == DEVICE_NAME:
            return device.address
    return None

async def read_characteristic():
    address = await find_device_by_name()
    if not address:
        print(f"Périphérique {DEVICE_NAME} non trouvé.")
        return
    
    async with BleakClient(address) as client:
        while True:
            if await client.is_connected():
                value = await client.read_gatt_char(CHARACTERISTIC_UUID)
                decoded_data = value.decode("utf-8")
                writeFiles.writInFiles(decoded_data, id)
                client.disconnect()
                
            await asyncio.sleep(0.1)

if __name__ == "__main__":
    start = int(round(time.time() * 1000))
    asyncio.run(read_characteristic())
    stop = int(round(time.time() * 1000))
    print(stop-start)