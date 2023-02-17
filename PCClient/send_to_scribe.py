import time
import signal
from PCClient import PCClient
from bluetooth import *

exit_ = False
def sigint_handler(signum, frame):
    global exit_
    exit_ = True

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

if __name__ == "__main__":
	is_connected = False
	pcclient = PCClient()

	try:
		pcclient.connect()
		is_connected = True
	except:
		while not is_connected:
			if not pcclient.retry_connection():
				print("Connection error")
			else:
				print("Connected")
				is_connected = True

	image_count = 0
	while not exit_:

		key = input("Press 'q' to quit, 'c' to capture, 'b' to reconnect: ")
		print(key)
		if key == 'q' and pcclient != None:
			exit_ = True
		if key == 'c' and pcclient != None:
			try:
				lines = pcclient.get_text()
				print(lines)
				if pcclient.send_lines(lines):
					print('sent')
			except:
				print("Something went wrong. Retry connecting to bluetooth")
			
		if key == 'b':
			print("Attempting to reconnect")
			if pcclient.retry_connection():
				print("Success")
			else:
				print("Failed")
	pcclient.close()
