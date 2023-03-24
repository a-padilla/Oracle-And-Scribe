# Oracle class - Contains all the functionality all in one class
import time
from bluetooth import *
PREFIX = "for_the_alicex"
class Alice():
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
		self.connection.close_connection()
		# empty sample.txt
		with open("sample.txt", "w") as f:
			f.write("")
			
	
	def get_text(self, num_lines = 5):
		"""Get the text from the file"""
		#open sample.txt, read 15 lines, store, and remove from file
		#return the text
		with open("Alice/sample.txt", "r+") as f:
			lines = f.readlines()
			f.seek(0)
			f.truncate()
			f.writelines(lines[num_lines:])
		text = lines[:num_lines]
		# remove all non-ascii characters, and encode to utf-8
		text = ''.join(text)
		clean = [''.join([i if ord(i) <= 126 and ord(i) >= 32 else '' for i in s]).encode('utf-8') for s in text.split('\n')]
		
		# remove all empty strings
		clean = [s for s in clean if s != b'']
		print(*clean, sep = '\n')
		return clean


	def send_lines(self, lines):
		"""Send lines to the Scribe
		
		If not connected, throw an error.
		"""
		num_lines = len(lines)
		if num_lines <= 0:
			return False
		#send number of lines
		self.connection.sock.send(PREFIX + str(len(lines)))
		while True:	
			# wait for response
			response = self.connection.sock.recv(1024)
			print(response)
			if response == PREFIX + b'1':
				break
		while lines:
			self.connection.sock.send(PREFIX + lines[0])
			# wait for response again
			response = self.connection.sock.recv(1024)
			print(response)
			if response == PREFIX + b'2':
				lines.pop(0)
		while True:
			response = self.connection.sock.recv(1024)
			if response == PREFIX + str(num_lines).encode('utf-8'):
				print(response)
				break
		time.sleep(0.1)
		return True