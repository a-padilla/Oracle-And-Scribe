# Process an image(pre-scanned for now) and send the output to the Scribe
try:
    from PIL import Image
except ImportError:
    import Image
import pytesseract
import numpy as np
from bluetooth import *


def process_image(filename):
	# get text from an image as a string
	text_from_image = pytesseract.image_to_string(Image.open(filename))

	# separate string into lines
	def strippy(s):
		return s.strip().encode('UTF-8')

	lines = text_from_image.split('\n')
	clean = np.array(lines)
	stripper = np.vectorize(strippy)
	clean = stripper(clean)
	clean = clean[clean!=''.encode('UTF-8')]
	
	print(len(clean))
	for i in range(len(clean)):
		print(clean[i])

	return clean
	
def send_image_data(image_data):
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
	
	#send number of lines
	sock.send(str(len(image_data)))
	
	# wait for response
	response = sock.recv(1024)
	print(response)
	if response == b'1':
		for i in range(len(image_data)):
			# when acknowledged, send the line
			sock.send(image_data[i])
			# wait for response again
			response = sock.recv(1024)
			print(response)
			if response != b'2':
				break
	print(sock.recv(1024))
	
	sock.close()
	
	
if __name__ == "__main__":
	image_data = process_image("./images/test1.jpg")
	send_image_data(image_data)
