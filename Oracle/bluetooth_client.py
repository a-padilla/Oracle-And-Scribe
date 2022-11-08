# Setup Oracle(RaspberryPi) as Bluetooth Client to connect to Scribe (ESP32)

from bluetooth import *
            
#MAC address of ESP32
addr = "C8:F0:9E:9E:50:42"
#Name of ESP32
esp_name = "Scribe"

# Try to find the scribe based off the MAC Addr and name
found_service = find_service( address = addr)

scribe = found_service[0]

if len(scribe) == 0:
    print("couldn't find the Scribe")
    sys.exit(0)

port = scribe["port"]
name = scribe["name"]
host = scribe["host"]

print(f"connecting to ${name} on ${host} with port ${port}")

# Create the client socket
sock=BluetoothSocket(RFCOMM)
sock.connect((host, port))

print("connected")

while True:
	data = input()
	if len(data) == 0:
		break
	sock.send(data)
	sock.send("\n")

sock.close()
