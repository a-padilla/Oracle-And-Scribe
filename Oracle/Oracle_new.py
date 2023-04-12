from picamera2 import Picamera2
import cv2
import mediapipe as mp
import pytesseract
import bluetooth
import time


# Initialize mediapipe and Tesseract OCR
handsModule = mp.solutions.hands.Hands(static_image_mode=False, max_num_hands=2,
                                       min_detection_confidence=0.7, min_tracking_confidence=0.7)

connectToScribe = False

def _finger_is_up(handLandmarks, finger_tip):
    finger_mcm = finger_tip - 3
    return handLandmarks[finger_tip].y < handLandmarks[finger_mcm].y

def _finger_is_down(handLandmarks, finger_tip):
    finger_mcm = finger_tip - 3
    return handLandmarks[finger_tip].y > handLandmarks[finger_mcm].y

def _recognize_gestures(handLandmarks):
    #Check if user is pointing up
    if _finger_is_up(handLandmarks, 8) and _finger_is_down(handLandmarks, 12) and _finger_is_down(handLandmarks, 16) and _finger_is_down(handLandmarks, 20):
        return "start"
    elif _finger_is_up(handLandmarks, 8) and _finger_is_up(handLandmarks, 12) and _finger_is_up(handLandmarks, 16) and _finger_is_up(handLandmarks, 20):
        return "stop"
    else:
        return None


# Define a function to process the hand gesture
def process_gesture(frame):
    # Convert the frame to RGB and process it
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    print("conversion")
    #Produces the hand framework overlay ontop of the hand, you can choose the colour here too)
    results = handsModule.process(frame)
    print("Processed")

    gesture = None
    if results.multi_hand_landmarks != None:
        for handLandmarks in results.multi_hand_landmarks:
            normalizedLandmarks = handLandmarks.landmark
            gesture = _recognize_gestures(normalizedLandmarks)

    if gesture != None:
        print("gesture: " + gesture)
    
    return gesture


# Define a function to perform OCR on the hand gesture
def perform_ocr(gesture, frame):
    # gesture = gesture_future.result()
    if gesture != None:
        print("OCR got gesture: " + gesture)
    text = None
    if gesture != None:
        # Convert the hand gesture to a grayscale image
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Apply binary thresholding
        # thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]

        # # Apply median filtering to remove noise
        # filtered = cv2.medianBlur(thresh, 3)

        # # Deskew the image
        # coords = np.column_stack(np.where(filtered > 0))
        # angle = cv2.minAreaRect(coords)[-1]
        # if angle < -45:
        #     angle = -(90 + angle)
        # else:
        #     angle = -angle
        # (h, w) = frame.shape[:2]
        # center = (w // 2, h // 2)
        # M = cv2.getRotationMatrix2D(center, angle, 1.0)
        # rotated = cv2.warpAffine(filtered, M, (w, h), flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)

        # Perform text recognition on the preprocessed image
        text = pytesseract.image_to_string(gray)
        print(text)

    return text


# Define a function to send the text over Bluetooth
def send_text(text, sock):
    # Send the text over Bluetooth
    # text = ocr_future.result()
    if sock != None and text != None:
        sock.send(text.encode())
    elif text != None:
        print(text)


# Define a function to process frames from the queue and perform gesture recognition and OCR
# def process_frames(queue, sock):
#     with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
#         while True:
#             # wait for a frame from the queue
#             frame = queue.get()
#             print("Got frame")

#             # Process the hand gesture and perform OCR in separate threads
#             gesture_future = executor.submit(process_gesture, frame)
#             ocr_future = executor.submit(perform_ocr, gesture_future, frame)

#             # Send the text over Bluetooth
#             communication_future = executor.submit(send_text, ocr_future, sock)


# Define the main function to start the event loop and threads
def main():
    print("Main")
    # Initialize the Bluetooth connection
    sock = None
    if connectToScribe:
        addr = 'C8:F0:9E:9E:E7:C2' 
        port = 1
        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        sock.connect((addr, port))

    # Initialize the queue to store frames
    print("INIT QUEUE")

    # threading.Thread(target=process_frames, args=(queue, sock)).start()

    picam2 = Picamera2()
    config = picam2.create_video_configuration(main={"size": (640, 480)}, display=None)
    picam2.configure(config)
    picam2.start()


    print("Starting Main loop")
    # Loop to capture frames and put them in the queue
    while True:
        # Capture a frame from the camera
        frame = picam2.capture_array()
        print("New frame")

        # Put the frame in the queue
        gesture = process_gesture(frame)
        print("done with gesture")
        if gesture == "start":
            text = perform_ocr(gesture, frame)
            send_text(text, sock)

        
        key = cv2.waitKey(1) & 0xFF

        #Below states that if the |q| is press on the keyboard it will stop the system
        if key == ord("q"):
            break
        time.sleep(3)
        
    
    picam2.release()


if __name__ == '__main__':
    main()