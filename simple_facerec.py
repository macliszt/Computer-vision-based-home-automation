import face_recognition
import os
import cv2
import numpy as np

class SimpleFacerec:
    def __init__(self):
        self.known_face_encodings = []
        self.known_face_names = []

    def load_encoding_images(self, images_path):
        """
        Load images from a directory and encode the faces in them.

        Parameters:
        images_path (str): Path to the directory containing images.
        """
        # Ensure absolute path
        images_path = os.path.abspath(images_path)
        
        # Process each image file in the directory
        for img_path in os.listdir(images_path):
            if img_path.lower().endswith(('png', 'jpg', 'jpeg')):
                # Load the image
                img = cv2.imread(os.path.join(images_path, img_path))
                if img is None:
                    print(f"Image {img_path} could not be loaded.")
                    continue

                print(f"Loaded image {img_path} with shape: {img.shape} and dtype: {img.dtype}")

                # Ensure the image is in RGB format
                if img.shape[2] == 3 and img.dtype == np.uint8:
                    rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                else:
                    print(f"Image {img_path} is not in expected format.")
                    continue
                
                # Encode face(s) in the image
                try:
                    encodings = face_recognition.face_encodings(rgb_img)
                except Exception as e:
                    print(f"Error encoding image {img_path}: {e}")
                    continue

                if encodings:
                    self.known_face_encodings.append(encodings[0])
                    # Use filename without extension as the name
                    name = os.path.splitext(img_path)[0]
                    self.known_face_names.append(name)
                    print(f"Encoded face for {name}")
                else:
                    print(f"No faces found in {img_path}")

    def encode_image(self, image_path):
        """
        Encode the face(s) in a given image file.

        Parameters:
        image_path (str): Path to the image file.

        Returns:
        numpy.ndarray: Encoding of the first face found in the image or None if no face is found.
        """
        img = cv2.imread(image_path)
        if img is None:
            print(f"Image {image_path} could not be loaded.")
            return None

        print(f"Loaded image {image_path} with shape: {img.shape} and dtype: {img.dtype}")

        # Ensure the image is in RGB format
        if img.shape[2] == 3 and img.dtype == np.uint8:
            rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        else:
            print(f"Image {image_path} is not in expected format.")
            return None

        try:
            encodings = face_recognition.face_encodings(rgb_img)
        except Exception as e:
            print(f"Error encoding image {image_path}: {e}")
            return None

        return encodings[0] if encodings else None
