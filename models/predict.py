import sys
import pickle
import numpy as np
import json  # Import the json module to format the output correctly

# Load the pre-trained model (make sure it's in the same folder or adjust the path)
model = pickle.load(open('random_forest_model.pkl', 'rb'))

# Get the inputs from the command-line arguments
hour = float(sys.argv[1])  # Hour (timestamp)
humidity = float(sys.argv[2])  # Humidity
light_intensity = float(sys.argv[3])  # Light Intensity
location_indoor = int(sys.argv[4])  # Location (Indoor=1, Outdoor=0)

# Prepare the input data in the expected format (order: hour, humidity, light_intensity, location_Indoors)
X = np.array([[hour, humidity, light_intensity, location_indoor]])

# Make prediction using the model
prediction = model.predict(X)

# Prepare the response in JSON format
response = {'prediction': prediction[0]}

# Print the response as a JSON string
print(json.dumps(response))
