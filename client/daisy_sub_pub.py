import time
import paho.mqtt.client as paho

# MQTT Broker configuration
server = '172.20.10.2'  # IP of the MQTT server (e.g., your Mac)
port = 1883  # Default MQTT port

# Callback for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe to the topic (you can modify this as needed)
    client.subscribe("daisy/#", qos=1)

# Callback for when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    print(f"Received message '{msg.payload.decode()}' on topic '{msg.topic}'")

# Callback for when a publish is successful
def on_publish(client, userdata, mid):
    print(f"Message published with mid: {mid}")

# Initialize the MQTT client
client = paho.Client(client_id="Device_Client")

# Set the callbacks
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish

# Connect to the MQTT broker
client.connect(server, port)

# Loop for continuous operation
client.loop_start()

# Publish messages in a loop
try:
    while True:
        message = input("Enter message to publish (or 'exit' to quit): ")
        if message == "exit":
            break
        client.publish("daisy/test", payload=message, qos=1)
        time.sleep(1)  # Delay between publishes
except KeyboardInterrupt:
    print("Exiting...")

# Stop the loop and disconnect after exiting
client.loop_stop()
client.disconnect()
