#Import the necessary Packages for this software to run
import mediapipe
from picamera2 import Picamera2
import cv2

def finger_is_up(handLandmarks, finger_tip):
    finger_mcm = finger_tip - 3
    return handLandmarks[finger_tip].y < handLandmarks[finger_mcm].y

def finger_is_down(handLandmarks, finger_tip):
    finger_mcm = finger_tip - 3
    return handLandmarks[finger_tip].y > handLandmarks[finger_mcm].y

def recognize_gestures(handLandmarks):
    #Check if user is pointing up
    if finger_is_up(handLandmarks, INDEX_TIP) and finger_is_down(handLandmarks, 12) and finger_is_down(handLandmarks, 16) and finger_is_down(handLandmarks, 20):
        return True, False
    elif finger_is_up(handLandmarks, INDEX_TIP) and finger_is_up(handLandmarks, 12) and finger_is_up(handLandmarks, 16) and finger_is_up(handLandmarks, 20):
        return False, True
    else:
        return False, False

#Use MediaPipe to draw the hand framework over the top of hands it identifies in Real-Time
drawingModule = mediapipe.solutions.drawing_utils
handsModule = mediapipe.solutions.hands
INDEX_TIP = 8
INDEX_MCP = 5
Gesture = None

picam2 = Picamera2()

config = picam2.create_video_configuration(main={"size": (640, 480)}, display=None)
picam2.configure(config)
picam2.start()

with handsModule.Hands(static_image_mode=False, min_detection_confidence=0.7, min_tracking_confidence=0.7, max_num_hands=2) as hands:

    #Create an infinite loop which will produce the live feed to our desktop and that will search for hands
    while True:
        
        frame = cv2.cvtColor(picam2.capture_array(), cv2.COLOR_BGR2RGB)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        #Produces the hand framework overlay ontop of the hand, you can choose the colour here too)
        results = hands.process(frame)

        if results.multi_hand_landmarks != None:
            for handLandmarks in results.multi_hand_landmarks:
                normalizedLandmarks = handLandmarks.landmark
                start, stop = recognize_gestures(normalizedLandmarks)

                if start:
                    print("START")
                elif stop:
                    print("STOP")
                else:
                    print("Nothing")

                drawingModule.draw_landmarks(frame, handLandmarks, handsModule.HAND_CONNECTIONS)

        #Below shows the current frame to the desktop 
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF

        #Below states that if the |q| is press on the keyboard it will stop the system
        if key == ord("q"):
            break
    