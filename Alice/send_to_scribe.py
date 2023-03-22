import time
import signal
from Alice import Alice
from bluetooth import *

exit_ = False
def sigint_handler(signum, frame):
    global exit_
    exit_ = True

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

if __name__ == "__main__":
	is_connected = False
	alice = Alice()

	try:
		alice.connect()
		is_connected = True
	except:
		while not is_connected:
			if not alice.retry_connection():
				print("Connection error")
			else:
				print("Connected")
				is_connected = True

	image_count = 0
	while not exit_:

		key = input("Press 'q' to quit, 'c' to capture, 'b' to reconnect: ")
		print(key)
		if key == 'q' and alice != None:
			exit_ = True
		if key == 'c' and alice != None:
			try:
				lines = alice.get_text()
				print(lines)
				if alice.send_lines(lines):
					print('sent')
			except:
				print("Something went wrong. Retry connecting to bluetooth")
			
		if key == 'b':
			print("Attempting to reconnect")
			if alice.retry_connection():
				print("Success")
			else:
				print("Failed")
	alice.close()
