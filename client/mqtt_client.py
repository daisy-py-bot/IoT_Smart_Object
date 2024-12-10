import paho.mqtt.client as mqtt
import requests
import json
import time

# MQTT Configuration
MQTT_SERVER = "172.20.10.2"
MQTT_PATH = "FinalProjectYedu/#"  # Subscribe to all topics under FinalProjectYedu

# PHP endpoint configuration
PHP_ENDPOINT = "http://172.20.10.2/iot_smart_object/db/create_sensor_reading.php"  # The PHP endpoint to send the data to

# Callback when connected to the MQTT broker
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(MQTT_PATH)  # Subscribe to the major topic FinalProjectYedu/#

# Callback when a message is received on the subscribed topic
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    
    # Decode the message and parse it as JSON
    message = msg.payload.decode('utf-8')
    print("Message received:", message)

    try:
        # Parse the JSON payload
        payload_data = json.loads(message)
        
        # Prepare data to send to PHP endpoint
        data = {
            "node_id": payload_data.get("node_id"),
            "sensor_id": payload_data.get("sensor_id"),
            "sensor_value": payload_data.get("sensor_value")
        }

        # Post the data to the PHP endpoint using requests
        response = requests.post(PHP_ENDPOINT, data=data)
        
        # Check if the request was successful
        if response.status_code == 200:
            print("Data successfully sent to PHP endpoint.")
        else:
            print(f"Failed to send data to PHP. Status code: {response.status_code}")
    
    except json.JSONDecodeError:
        print("Error: Failed to decode JSON message.")
    except Exception as e:
        print(f"An error occurred: {e}")

# MQTT Client Setup
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT server
client.connect(MQTT_SERVER, 1883, 60)

# Start receiving messages
print("Waiting for messages...")
client.loop_forever()
