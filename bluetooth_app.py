import asyncio
from bleak import BleakClient, BleakScanner

# Funkcia pre spracovanie prijatých notifikácií
def notification_handler(sender, data):
    print(f"Prijaté dáta od {sender}: {data}")

async def run():
    # Skenuj BLE zariadenia
    devices = await BleakScanner.discover()
    for i, device in enumerate(devices):
        print(f"[{i}] {device.name} ({device.address})")

    # Vyber zariadenie na pripojenie
    device_index = int(input("Zadaj index zariadenia na pripojenie: "))
    selected_device = devices[device_index]

    # Pripojenie k zariadeniu
    async with BleakClient(selected_device.address) as client:
        print(f"Pripojený k {selected_device.name} ({selected_device.address})")

        # Výpis všetkých charakteristík zariadenia
        services = await client.get_services()
        for service in services:
            print(f"Služba: {service.uuid}")
            for char in service.characteristics:
                print(f"  - Charakteristika: {char.uuid} | Properties: {char.properties}")

        # UUID charakteristiky, z ktorej chceme čítať dáta (nahraď UUID správnou hodnotou)
        CHAR_UUID = input("Zadaj UUID charakteristiky na čítanie notifikácií: ")

        # Povolenie notifikácií pre vybranú charakteristiku
        await client.start_notify(CHAR_UUID, notification_handler)

        print("Prijímanie dát... Stlač Ctrl+C na ukončenie.")
        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("Ukončenie spojenia...")

        # Zastavenie notifikácií a odpojenie
        await client.stop_notify(CHAR_UUID)

# Spustenie asyncio event loop
loop = asyncio.get_event_loop()
loop.run_until_complete(run())
