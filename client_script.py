import json
import requests
import paho.mqtt.client as mqtt

# MQTT Broker Configuration
BROKER = "192.168.x.x"  # Replace with your broker's IP address
PORT = 1883
TOPIC = "FinalProjectYedu/#"  # Single topic with wildcard

# PHP Endpoint
PHP_ENDPOINT = "http://192.168.x.x/iot/FinalProject/iot_smart_object/db/create_sensor_reading.php"

# MQTT Callback: When a message is received
def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")
    try:
        # Parse the message payload
        data = json.loads(msg.payload.decode())
        
        # Send data to PHP endpoint
        response = requests.post(PHP_ENDPOINT, json=data)
        if response.status_code == 200 or response.status_code == 201:
            print(f"Data sent to PHP endpoint: {response.json()}")
        else:
            print(f"Failed to send data. HTTP Code: {response.status_code}")
    except Exception as e:
        print(f"Error: {e}")

# MQTT Callback: On connect
def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with code {rc}")
    client.subscribe(TOPIC)
    print(f"Subscribed to topic: {TOPIC}")

# Configure MQTT Client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to MQTT broker and start listening
client.connect(BROKER, PORT, 60)
client.loop_forever()
