#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <DHT.h>

// Define Pins and Constants#define DHTPIN 4          // Pin connected to the DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT22     // DHT sensor type
#define LDRPIN 34         // Analog pin for LDR
#define FANPIN 5          // Pin to control the fan
#define LEDPIN 2          // Heartbeat LED pin

// Wi-Fi and Web Server
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
char ssidAP[] = "DAISYESP32AP";
char passwordAP[] = "12345678";

const char* sta_ssid = "Daisy";       // Wi-Fi network to connect to
const char* sta_password = "dkt11.pyy"; // Wi-Fi network password


unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

IPAddress local_ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// MQTT Settings
const char* mqttServer = "172.20.10.2"; //  MQTT broker
const int mqttPort = 1883;
const char* main_topic= "FinalProjectYedu/#";

// HTTP Settings
const char* httpServer = "http://172.20.10.2/iot_smart_object/db/create_sensor_reading.php"; 

// Variables
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity;
int lightIntensity;
// String postingMethod = "HTTP"; // Default posting method
String postingMethod = "MQTT"; // Default posting method
String fanMode = "AUTO";       // Default fan mode
int triggerTemperature = 30;   // Default temperature threshold
bool manualFanState = false;   // Manual fan state
String node = "node1";

// Function Prototypes
void readSensors();
void postData(String jsonPayload);
void sendHTTP(String jsonPayload);
void sendMQTT(String jsonPayload);
void togglePostingMethod();
void toggleFanMode();
void setManualFanState(bool state);
void updateTriggerTemperature();
String mainPage();
String settingsPage();
void blinkHeartbeat();
void setupMQTT();
void LDR();
void Temperature();
void Humidity();
void publishMessage(const char* topic, String payload , boolean retained);
  


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);                 // using the esp32 as both an AP and a station

  // configuring station mode (STA) - connecting to a hotspot
  WiFi.begin(sta_ssid, sta_password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi in STA mode");
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());


  // configuring access point mode (AP) to create a hotspot from the esp32
  Serial.println("Setting up Wi-Fi Access Point...");
  // delay(1000);

  WiFi.softAP(ssidAP, passwordAP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("Initialize Web Server Routes...");
  // delay(1000);

  // initializing Web Server Routes for controls on the esp32 in the air
  server.on("/", []() { server.send(200, "text/html", mainPage()); });
  server.on("/settings", []() { server.send(200, "text/html", settingsPage()); });
  server.on("/togglePosting", togglePostingMethod);
  server.on("/toggleFanMode", toggleFanMode);
  server.on("/manualFanOn", []() { setManualFanState(true); });
  server.on("/manualFanOff", []() { setManualFanState(false); });
  server.on("/setTemp", updateTriggerTemperature);
  server.on("/setNode", updateNode);

  server.begin();

  // Initialize Pins
  Serial.println("Initializing pins...");
  pinMode(FANPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(FANPIN, LOW); // Fan off
  digitalWrite(LEDPIN, LOW); // LED off
  Serial.println("Pins initialized.");

  // Initialize Sensors and MQTT
  Serial.println("Initializing DHT sensor...");
  dht.begin();
  Serial.println("DHT sensor initialized.");

  Serial.println("Setting up MQTT client...");
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.println("MQTT client setup complete.");

  Serial.println("Setup Complete. Connect to ESP32AP.");
}


// Loop Function
void loop() {

  static long previousServerMillis = 0;
  const long serverInterval = 500; // Check server every 50ms

  blinkHeartbeat();        // Blink Heartbeat LED

  // Handle server requests at intervals
  long currentMillis = millis();
  if (currentMillis - previousServerMillis >= serverInterval) {
    previousServerMillis = currentMillis;
    server.handleClient(); // Handle Web Server Requests
  }

  
  // server.handleClient();   // Handle Web Server Requests
  readSensors();           // Periodically Read Sensors

  if (fanMode == "AUTO" && temperature >= triggerTemperature) {
    digitalWrite(FANPIN, HIGH); // Turn fan on
  } else if (fanMode == "AUTO") {
    digitalWrite(FANPIN, LOW);  // Turn fan off
  }


}



// Function to Read Sensor Data
void readSensors() {
    Temperature();
    Humidity();
    LDR();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  
}



void LDR(){

  int desiredInterval=6000;
  static long previous=0;
  long current=millis();

  if (current-previous>=desiredInterval){
    lightIntensity = analogRead(LDRPIN); // take the ldr reading
    previous=current;  
    // Serial.println(lightIntensity);
    // Serial.printf("Light: %d\n", lightIntensity);

    String jsonPayload = "{";
    jsonPayload += "\"node_id\":\"" + String(node) + "\",";
    jsonPayload += "\"sensor_id\":\"light\","; 
    jsonPayload += "\"sensor_value\":" + String(lightIntensity);
    jsonPayload += "}";

    postData(jsonPayload);

  }
  
}



void Temperature(){
  int desiredInterval=6000;
  static long previous=0;
  long current=millis();

  if(current-previous>=desiredInterval){
    temperature = dht.readTemperature();
    previous=current;

    String jsonPayload = "{";
    jsonPayload += "\"node_id\":\"" + String(node) + "\",";
    jsonPayload += "\"sensor_id\":\"temp\","; 
    jsonPayload += "\"sensor_value\":" + String(temperature);
    jsonPayload += "}";

    postData(jsonPayload);
    // Serial.printf("Temp: %.2f°C", temperature);
  }
}



void Humidity(){
  int desiredInterval=3000;
  static long previous=0;
  long current=millis();

  if(current-previous>=desiredInterval){
    humidity = dht.readHumidity();
    previous=current;

    String jsonPayload = "{";
    jsonPayload += "\"node_id\":\"" + String(node) + "\",";      // Convert node to a String
    jsonPayload += "\"sensor_id\":\"humidity\",";           // Add quotes around "humidity"
    jsonPayload += "\"sensor_value\":" + String(humidity);  // Convert humidity to a String
    jsonPayload += "}";

    postData(jsonPayload);
    // Serial.printf("Hum: %.2f%%", humidity);
  }
}




void blinkHeartbeat() {
  static long previousMillis = 0;     // Time tracker for LED toggling
  const long waitInterval = 2000;     // Wait time between cycles (2 seconds)
  long currentMillis = millis();      // Get the current time

  // If 2 seconds have passed, start a new cycle
  if (currentMillis - previousMillis >= waitInterval) {
    previousMillis = currentMillis; // Reset the cycle start time
    // blinkCount = 0;                   // Reset the blink count
    digitalWrite(LEDPIN, HIGH); // Set the LED state
    delay(70);
    digitalWrite(LEDPIN, LOW); // Set the LED state
    delay(70);
    digitalWrite(LEDPIN, HIGH); // Set the LED state
    delay(70);
    digitalWrite(LEDPIN, LOW); // Set the LED state
  }

  
}



// Function to Post Data Based on Method
void postData(String jsonPayload) {
  if (postingMethod == "HTTP") {
    sendHTTP(jsonPayload);
  } else if (postingMethod == "MQTT") {
    sendMQTT(jsonPayload);
  }
}



// Function to Send Data via HTTP POST
void sendHTTP(String jsonPayload) {
  HTTPClient http;
  http.begin(httpServer); // Specify the API endpoint
  http.addHeader("Content-Type", "application/json");


  // Serial.println(jsonPayload);
  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    Serial.print("body: ");
    Serial.println(jsonPayload);


  http.end();
}




// Function to Send Data via MQTT
void sendMQTT(String jsonPayload) {
  if (!mqttClient.connected()) {
    setupMQTT();
  }

  String fullTopic = "FinalProjectYedu/" + node;
  // Concatenate the base topic with the node (subtopic)
  // publishMessage(fullTopic.c_str(), jsonPayload, true);  // Publish to the full topic
  publishMessage(fullTopic, jsonPayload,true); 
  
}


//======================================= publising as string
void publishMessage(String topic, String payload , boolean retained){

  if (mqttClient.publish(topic.c_str(), payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}



void setupMQTT() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);  //you could make this static
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())){//, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      mqttClient.subscribe(main_topic);   // subscribe the topics here
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 3 seconds");   // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}



// Function to Toggle Posting Method
void togglePostingMethod() {
  Serial.print("Current posting method is: ");
  Serial.println(postingMethod);
  postingMethod = (postingMethod == "HTTP") ? "MQTT" : "HTTP";
  server.send(200, "text/html", settingsPage());
  Serial.print("Toggled posting method to: ");
  Serial.println(postingMethod);
}


// Function to Toggle Fan Mode
void toggleFanMode() {
  Serial.print("Current fanmode: ");
  Serial.println(fanMode);
  fanMode = (fanMode == "AUTO") ? "MANUAL" : "AUTO";
  server.send(200, "text/html", settingsPage());
  Serial.print("Toggled fanmode to: ");
  Serial.println(fanMode);
}


// Function to Set Manual Fan State
void setManualFanState(bool state) {
  manualFanState = state;
  digitalWrite(FANPIN, state ? HIGH : LOW);
  server.send(200, "text/html", mainPage());
}


// Function to Update Trigger Temperature
void updateTriggerTemperature() {
  if (server.hasArg("temp")) {
    triggerTemperature = server.arg("temp").toInt();
  }
  server.send(200, "text/html", settingsPage());
}

// Function to Update Trigger Temperature
void updateNode() {
  Serial.println("Updated node from: ");
  Serial.print(node);
  if (server.hasArg("node")) {
    node = server.arg("node");
  }
  server.send(200, "text/html", settingsPage());
  Serial.print(" to: "); 
  Serial.print(node);
}


// Function to Generate Main Page HTML
String mainPage() {
  String page = "<!DOCTYPE html><html><head><title>Dashboard</title></head><body>";
  page += "<h1>Smart Object Dashboard</h1>";
  page += "<p>Temperature: " + String(temperature) + "°C</p>";
  page += "<p>Humidity: " + String(humidity) + "%</p>";
  page += "<p>Light Intensity: " + String(lightIntensity) + "</p>";
  page += "<a href='/settings'>Go to Settings</a>";
  page += "</body></html>";
  return page;
}


// Function to Generate Settings Page HTML
String settingsPage() {
  String page = "<!DOCTYPE html><html><head><title>Settings</title></head><body>";
  page += "<h1>Configuration Settings</h1>";
  page += "<p>Posting Method: " + postingMethod + "</p>";
  page += "<button onclick=\"location.href='/togglePosting'\">Switch Posting Method</button>";
  page += "<p>Fan Mode: " + fanMode + "</p>";
  page += "<button onclick=\"location.href='/toggleFanMode'\">Toggle Fan Mode</button>";

  if (fanMode == "AUTO") {
    page += "<p>Trigger Temperature: " + String(triggerTemperature) + "°C</p>";
    page += "<form action='/setTemp' method='POST'><input type='number' name='temp'>";
    page += "<button type='submit'>Set Temperature</button></form>";
  } else {
    page += "<button onclick=\"location.href='/manualFanOn'\">Turn Fan On</button>";
    page += "<button onclick=\"location.href='/manualFanOff'\">Turn Fan Off</button>";
  }
  page += "<p>Current node: " + node + "</p>";
  page += "<form action='/setNode' method='POST'><input type='text' name='node'>";
  page += "<button type='submit'>Set Node</button></form>";

  page += "</body></html>";
  
  return page;
}


