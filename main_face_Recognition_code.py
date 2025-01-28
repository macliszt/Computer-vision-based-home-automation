import serial
from faceRecModule import run_face_recognition
import RPi.GPIO as GPIO
import time

# Setup GPIO for LED
LED_PIN = 18  # Adjust this pin number according to your setup
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)

# Setup serial communication with Arduino
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Check your serial port

# Variable to track if the face was recognized at any time
face_recognized = False

# Callback function to handle access
def access_callback(access_granted):
    global face_recognized

    if access_granted:
        face_recognized = True  # Set the flag to True when a face is recognized
        GPIO.output(LED_PIN, GPIO.HIGH)  # Turn LED on if access granted
        print("LED ON")
        ser.write(b'Face Recognized\n')  # Send message to Arduino
    else:
        if face_recognized:  # If a face was previously recognized
            GPIO.output(LED_PIN, GPIO.HIGH)  # Keep the LED on as access was already granted
            print("Access granted, keeping LED ON")
            ser.write(b'Face Recognized\n')  # Keep sending recognized message
        else:
            GPIO.output(LED_PIN, GPIO.LOW)  # Turn LED off if no access
            print("LED OFF")
            ser.write(b'Face Not Recognized\n')  # Send message to Arduino

    time.sleep(0.5)  # Reduced delay for better responsiveness

if __name__ == "__main__":
    try:
        run_face_recognition(access_callback)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        GPIO.cleanup()  # Clean up GPIO settings on exit
