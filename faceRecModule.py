# faceRecModule.py
import cv2
import face_recognition
import time
from simple_facerec import SimpleFacerec

def initialize_video_capture():
    video_capture = cv2.VideoCapture(0)
    video_capture.set(cv2.CAP_PROP_FRAME_WIDTH, 640)  # Higher resolution
    video_capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    return video_capture

def draw_face_boxes(frame, face_location, name, box_color):
    top, right, bottom, left = [coordinate * 4 for coordinate in face_location]
    cv2.rectangle(frame, (left, top), (right, bottom), box_color, 2)
    return left, right, bottom, top

def display_name(frame, left, right, bottom, top, name, box_color, timeout, last_detected_time):
    if time.time() - last_detected_time <= timeout:
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), box_color, cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

def run_face_recognition(access_callback=None, image_folder="/home/macliszt/Face/Images", process_every_n_frames=3, recognition_tolerance=0.5):
    sfr = SimpleFacerec()
    sfr.load_encoding_images(image_folder)

    video_capture = initialize_video_capture()
    frame_counter = 0
    last_detected_name = "Unknown"
    last_detected_time = 0
    display_timeout = 1

    left, right, bottom, top = 0, 0, 0, 0  
    box_color = (0, 0, 255)  # Default to red if no face is detected

    while True:
        ret, frame = video_capture.read()
        if not ret:
            print("Failed to capture frame from webcam.")
            break

        # Resize frame for faster processing
        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
        rgb_small_frame = cv2.cvtColor(small_frame, cv2.COLOR_BGR2RGB)

        if frame_counter % process_every_n_frames == 0:
            face_locations = face_recognition.face_locations(rgb_small_frame)
            face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

            if face_locations:  # Process only if faces are detected
                for face_encoding, face_location in zip(face_encodings, face_locations):
                    results = face_recognition.compare_faces(sfr.known_face_encodings, face_encoding, tolerance=recognition_tolerance)
                    name = "Unknown"
                    access_granted = False
                    
                    if True in results:
                        first_match_index = results.index(True)
                        name = sfr.known_face_names[first_match_index]
                        access_granted = 'mac' in name.lower()  # Adjust to your needs
                        print("Access granted" if access_granted else "Access denied")
                        
                    if access_callback:
                        access_callback(access_granted)

                    if name != last_detected_name:
                        last_detected_name = name
                        last_detected_time = time.time()

                    box_color = (0, 255, 0) if name != "Unknown" else (0, 0, 255)
                    left, right, bottom, top = draw_face_boxes(frame, face_location, name, box_color)

            else:
                if access_callback:  # No face detected, trigger the callback for failure
                    access_callback(False)
                print("No faces detected")
                box_color = (0, 0, 255)

        # Update name display
        if last_detected_name != "Unknown":
            display_name(frame, left, right, bottom, top, last_detected_name, box_color, display_timeout, last_detected_time)

        cv2.imshow('Video', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("Exiting face recognition...")
            break

        frame_counter += 1

    video_capture.release()
    cv2.destroyAllWindows()

