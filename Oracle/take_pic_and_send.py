import time
import signal
import cv2
import threading
import argparse
import sys
from Oracle import Oracle
sys.path.append('./camera')
from RpiCamera import Camera
from Focuser import Focuser
from Autofocus import FocusState, doFocus


exit_ = False
def sigint_handler(signum, frame):
    global exit_
    exit_ = True

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

import RPi.GPIO as GPIO
import time

BuzzerPin = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(BuzzerPin, GPIO.OUT) 

if __name__ == "__main__":
	is_connected = False
	oracle = Oracle()

	try:
		oracle.connect()
		is_connected = True
	except:
		while not is_connected:
			if not oracle.retry_connection():
				print("Connection error")
				GPIO.output(BuzzerPin,GPIO.HIGH)
				time.sleep(1)
				GPIO.output(BuzzerPin,GPIO.LOW)
				time.sleep(5)
			else:
				print("Connected")
				is_connected = True
			
	oracle.start_preview(False)
	focuser = Focuser(10)

	focusState = FocusState()
	doFocus(oracle, focuser, focusState)

	start = time.time()
	frame_count = 0

	image_count = 0
	while not exit_:
		frame = oracle.getFrame()
		img = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)

		cv2.imshow("Test", img)
		key = cv2.waitKey(1)
		if key == ord('q') and oracle != None:
			exit_ = True
		if key == ord('f') and oracle != None:
			if focusState.isFinish():
				focusState.reset()
				doFocus(oracle, focuser, focusState)
			else:
				print("Focus is not done yet.")
		if key == ord('c') and oracle != None:
			try:
				print("Capturing photo")
				cv2.imwrite("./images/image_to_translate.jpg", oracle.getFrame())
				image_count += 1
				print('processing...')
				lines = oracle.process_image("./images/image_to_translate.jpg")
				#lines = process_image("./images/image_to_translate.jpg")
				print('sending...')
				if oracle.send_lines(lines):
					print('sent')
			except:
				print("Something went wrong. Retry connecting to bluetooth")
				GPIO.output(BuzzerPin,GPIO.HIGH)
				time.sleep(1)
				GPIO.output(BuzzerPin,GPIO.LOW)
			
		if key == ord('b'):
			print("Attempting to reconnect")
			if oracle.retry_connection():
				print("Success")
			else:
				print("Failed")
				GPIO.output(BuzzerPin,GPIO.HIGH)
				time.sleep(1)
				GPIO.output(BuzzerPin,GPIO.LOW)


		frame_count += 1
		if time.time() - start >= 1:
			print("{}fps".format(frame_count))
			start = time.time()
			frame_count = 0
	oracle.close()
