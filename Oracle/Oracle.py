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
import cv2
from bluetooth import *
sys.path.append('./camera')
from RpiCamera import Camera
ORACLE_PREFIX = "for_the_oracle"

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
			self.esp_mac_addr = esp_mac_addr
			self.connect_to_scribe(esp_mac_addr)
		
		def retry(self):
			try:
				self.connect_to_scribe(self.esp_mac_addr)
				return True
			except:
				return False
			
		def connect_to_scribe(self,esp_mac_addr):
			self.esp_mac_addr = esp_mac_addr
			found_service = find_service( address = self.esp_mac_addr)
			
			if len(found_service) == 0:
				raise Exception("Bluetooth connection failed")

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
	addr = "C8:F0:9E:9E:E7:C2"
	
	def __init__(self, width=640, height=360):
		
		Camera.__init__(self)
		#self.connection = self.Connection(addr)
		
	def connect(self):
		self.connection = self.Connection(self.addr)
		
	def retry_connection(self):
		try:
			if self.connection == None:
				self.connection = self.Connection(self.addr)
				return True
		except:
			return False
		return self.connection.retry()
		
	def close(self):
		Camera.close(self)
		self.connection.close_connection()
 
	def pre_process_image(self, image):
		# convert img from pillow to cv2
		image = np.array(image)
		image = cv2.resize(image, None, fx=1.2, fy=1.2, interpolation=cv2.INTER_CUBIC)
		# skip converting to grayscale if image is already grayscale
		if len(image.shape) == 2:
			pass
		else:
			image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
		kernel = np.ones((1, 1), np.uint8)
		image = cv2.dilate(image, kernel, iterations=1)
		image = cv2.erode(image, kernel, iterations=1)
		# convert back to pillow
		return Image.fromarray(image)
        
	def process_image(self, image):
		"""Take an image and process it into usable lines that can be sent to the Scribe"""
		
		text_from_image = pytesseract.image_to_string(image)

		# remove all non-ascii characters, and encode to utf-8
		clean = [''.join([i if ord(i) <= 126 and ord(i) >= 32 else '' for i in s]).encode('utf-8') for s in text_from_image.split('\n')]
		
		# remove all empty strings
		clean = [s for s in clean if s != b'']
		print(*clean, sep = '\n')
		return clean

	
	def send_lines(self, lines):
		"""Send lines to the Scribe
		
		If not connected, throw an error.
		"""
		
		if len(lines) <= 0:
			return False
			
		#send number of lines
		self.connection.sock.send(ORACLE_PREFIX + str(len(lines)))
		
		# wait for response
		response = self.connection.sock.recv(1024)
		print(response)
		if response == ORACLE_PREFIX + b'1':
			for i in range(len(lines)):
				# when acknowledged, send the line
				self.connection.sock.send(ORACLE_PREFIX + lines[i])
				# wait for response again
				response = self.connection.sock.recv(1024)
				print(response)
				if response != ORACLE_PREFIX + b'2':
					break
		time.sleep(0.1)
		print(self.connection.sock.recv(1024))
		
		return True
		

