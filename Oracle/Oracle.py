# Oracle class - Contains all the functionality all in one class
import time
import  threading
from picamera2 import Picamera2, Preview
try:
    from PIL import Image
except ImportError:
    import Image
import pytesseract
import numpy as np
from bluetooth import *
sys.path.append('./camera')
from RpiCamera import Camera


class Oracle(Camera):
	"""Encapsulates all functionality to take an image, process it, and send it to the Scribe

	Inherits from the Arducam team's Camera class
	"""

	class Connection:
		"""Handles connecting to the Scribe through bluetooth and any retry/error-logic for failed connections"""
		service = None
		port = None
		name = None
		host = None
		sock = None
		esp_mac_addr = None
		
		def __init__(self, esp_mac_addr):
			self.connect_to_scribe(esp_mac_addr)
			
		def connect_to_scribe(self,esp_mac_addr):
			self.esp_mac_addr = esp_mac_addr
			found_service = find_service( address = self.esp_mac_addr)

			self.scribe = found_service[0]

			if len(self.scribe) == 0:
				print("couldn't find the Scribe")
				sys.exit(0)

			self.port = self.scribe["port"]
			self.name = self.scribe["name"]
			self.host = self.scribe["host"]

			print(f"connecting to ${self.name} on ${self.host} with port ${self.port}")

			# Create the client socket
			self.sock=BluetoothSocket(RFCOMM)
			self.sock.connect((self.host, self.port))

			print("connected")
			
		def close_connection(self):
			self.sock.close()
			self.sock = None
			self.service = None
		

	cam = None
	_value_lock = None
	connection = None
	
	def __init__(self, width=640, height=360):
		addr = "C8:F0:9E:9E:E7:C2"
		Camera.__init__(self)
		self.connection = self.Connection(addr)
		
	def close(self):
		Camera.close()
		self.connection.close_connection()
 
	
        
	def process_image(self, filename):
		"""Take an image and process it into usable lines that can be sent to the Scribe"""
		text_from_image = pytesseract.image_to_string(Image.open(filename))
		
		def strippy(s):
			"""Takes a string and separates it into lines to send to the Scribe"""
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
	
	def send_lines(self, lines):
		"""Send lines to the Scribe
		
		If not connected, throw an error.
		"""
		
		#send number of lines
		self.connection.sock.send(str(len(lines)))
		
		# wait for response
		response = self.connection.sock.recv(1024)
		print(response)
		if response == b'1':
			for i in range(len(lines)):
				# when acknowledged, send the line
				self.connection.sock.send(lines[i])
				# wait for response again
				response = self.connection.sock.recv(1024)
				print(response)
				if response != b'2':
					break
		print(self.connection.sock.recv(1024))
		

