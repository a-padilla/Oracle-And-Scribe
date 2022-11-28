import time
import signal
import cv2
import threading
import argparse
import sys
sys.path.append('./camera')
from RpiCamera import Camera
from Focuser import Focuser
from Autofocus import FocusState, doFocus
from send_processed_image import process_image, send_image_data

exit_ = False
def sigint_handler(signum, frame):
    global exit_
    exit_ = True

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

if __name__ == "__main__":
	camera = Camera()
	camera.start_preview(False)
	focuser = Focuser(10)

	focusState = FocusState()
	doFocus(camera, focuser, focusState)

	start = time.time()
	frame_count = 0

	image_count = 0
	while not exit_:
		frame = camera.getFrame()
		img = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)

		cv2.imshow("Test", img)
		key = cv2.waitKey(1)
		if key == ord('q'):
			exit_ = True
		if key == ord('f'):
			if focusState.isFinish():
				focusState.reset()
				doFocus(camera, focuser, focusState)
			else:
				print("Focus is not done yet.")
		if key == ord('c'):
			print("Capturing photo")
			cv2.imwrite("./images/image_to_translate.jpg", camera.getFrame())
			image_count += 1
			print('processing...')
			image_data = process_image("./images/image_to_translate.jpg")
			print('sending...')
			send_image_data(image_data)
			print('sent')


		frame_count += 1
		if time.time() - start >= 1:
			print("{}fps".format(frame_count))
			start = time.time()
			frame_count = 0
	camera.close()