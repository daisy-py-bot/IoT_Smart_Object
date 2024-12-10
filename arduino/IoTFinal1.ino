#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// initializiing lcd screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define Pins and Constants#define DHTPIN 4          
#define DHTPIN 4
#define DHTTYPE DHT22                         // DHT sensor type
#define LDRPIN 34                            // Analog pin for LDR
#define FANPIN 5                            // Pin to control the fan
#define LEDPIN 2                           // Heartbeat LED pin
#define FAN_MODE_LED 25                   // Pin for Fan Mode LED simulation
#define FAN_STATE_LED 26                 // Pin for Fan State LED simulation

// manual
// Wi-Fi and Web Server
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
char ssidAP[] = "DAISYESP32AP";
char passwordAP[] = "12345678";

const char* sta_ssid = "Zvikomborero";       // Wi-Fi network to connect to
const char* sta_password = "12345678"; // Wi-Fi network password


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
const char* httpServer = "http://192.168.88.29/iot/FinalProject/iot_smart_object/db/create_sensor_reading.php"; 

// Variables
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity;
int lightIntensity;
String postingMethod = "HTTP"; // Default posting method
// String postingMethod = "MQTT"; // Default posting method
String fanMode = "AUTO";       // Default fan mode
float triggerTemperature = 30.0;   // Default temperature threshold
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
void setupLCD();
void displayIPAddress();
void setupFanLEDs();
  
// on-board led heartbeat rate (millis)
int heartbeatRate = 70;

void setup() {
  Serial.begin(115200);
  setupLCD();
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
  displayIPAddress();
  lcd.clear();
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
  server.on("/setHeartbeat", updateHeartbeatRate);
  server.on("/api/sensors", fetchSensorValues); 
  server.on("/api/ldr", fetchLDRData);         // Fetch LDR data


  server.begin();

  // Initialize Pins
  Serial.println("Initializing pins...");
  pinMode(FANPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(FANPIN, LOW); // Fan off
  digitalWrite(LEDPIN, LOW); // LED off
  setupFanLEDs();
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
    digitalWrite(FAN_STATE_LED, HIGH); // Turn fan on
  } 
  // else if (fanMode == "AUTO") {
  //   digitalWrite(FAN_STATE_LED, LOW);  // Turn fan off
  // }
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
  // displaying the reading on lcd screen
  displayReadings(temperature, humidity, lightIntensity);
}

// function to setup lcd screen
void setupLCD() {
  Wire.begin(21, 22);
  lcd.begin(16,2);       
  lcd.backlight();   
  lcd.clear();       
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
}

// function to display reading on lcd
void displayReadings(float temperature, float humidity, int lightIntensity) {
  // Row 1: Temperature and Humidity
  // lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 1);
  lcd.print("%");

  // Row 2: Light Intensity
  lcd.setCursor(0, 1);
  lcd.print("Light:");
  lcd.print(lightIntensity);
  // lcd.print("");              // Clear remaining characters
}

// function to display the ESP32 AP on lcd screen
void displayIPAddress() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 IP Address");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.softAPIP().toString().c_str());
  delay(10000);                 // giving time to connect to the ESP32 AP
}

// function to set up LEDs simulating fan operation
void setupFanLEDs() {
  pinMode(FAN_MODE_LED, OUTPUT);
  pinMode(FAN_STATE_LED, OUTPUT);

  // LEDs off initially
  digitalWrite(FAN_MODE_LED, HIGH);                         // HIGH for automatic fanMode(initial mode)
  digitalWrite(FAN_STATE_LED, LOW);
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
    digitalWrite(LEDPIN, HIGH);
    delay(heartbeatRate);
    digitalWrite(LEDPIN, LOW);
    delay(heartbeatRate);
    digitalWrite(LEDPIN, HIGH);
    delay(heartbeatRate);
    digitalWrite(LEDPIN, LOW);
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

// publising as string
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

  if (fanMode == "AUTO") {
    fanMode = "MANUAL";
    digitalWrite(FAN_MODE_LED, LOW); // Fan Mode LED OFF
  } else {
    fanMode = "AUTO";
    digitalWrite(FAN_MODE_LED, HIGH); // Fan Mode LED ON

  }
  server.send(200, "text/html", settingsPage());
  Serial.print("Toggled fanmode to: ");
  Serial.println(fanMode);
}


// Function to Set Manual Fan State
void setManualFanState(bool state) {
  manualFanState = state;
  digitalWrite(FAN_STATE_LED, state ? HIGH : LOW);
  server.send(200, "text/html", mainPage());
}


// Function to Update Trigger Temperature
void updateTriggerTemperature() {
  if (server.hasArg("temp")) {
    triggerTemperature = server.arg("temp").toFloat();
  }
  server.send(200, "text/html", settingsPage());
}

// function to update onboard led beeping rate
void updateHeartbeatRate() {
  if (server.hasArg("rate")) {
    heartbeatRate = server.arg("rate").toInt();
    Serial.print("Updated heartbeat rate to: ");
    Serial.println(heartbeatRate);
  } else {
    Serial.println("No rate argument received.");
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

// fetching values for the page on the esp32
void fetchSensorValues() {
  String json = "{";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"humidity\":" + String(humidity) + ",";
  json += "\"light\":" + String(lightIntensity);
  json += "}";
  server.send(200, "application/json", json);
}

// fetch ldr data from the database
void fetchLDRData() {
  HTTPClient htt;
  String endpoint = "http://192.168.88.29/iot/FinalProject/iot_smart_object/db/get_ldr_data.php";
  htt.begin(endpoint);

  // fetch the data
  int httpResponseCode = htt.GET(); 

  if (httpResponseCode >= 200 && httpResponseCode < 300) {
    String jsonResponse = htt.getString();
    server.send(200, "application/json", jsonResponse);                 // Send response back to the client
  } else {
    String error = "{\"error\":\"Failed to fetch data. HTTP code: " + String(httpResponseCode) + "\"}";
    server.send(500, "application/json", error);
  }

  htt.end();
}

// Function to Generate Main Page HTML
String mainPage() {
  String page = "<!DOCTYPE html><html><head><title>Dashboard</title>";
  
  // JavaScript for live sensor updates and LDR data fetching
  page += "<script>";
  
  // Fetch and update sensor values every 5 seconds
  page += "setInterval(async function() {";
  page += "  const response = await fetch('/api/sensors');";
  page += "  const data = await response.json();";
  page += "  document.getElementById('temp').innerText = data.temperature + '°C';";
  page += "  document.getElementById('hum').innerText = data.humidity + '%';";
  page += "  document.getElementById('light').innerText = data.light + ' lux';";
  page += "}, 5000);";
  
  // Fetch LDR data and display in a table
  page += "async function fetchLDRData() {";
  page += "  const response = await fetch('/api/ldr');";
  page += "  const data = await response.json();";
  page += "  const ldrTableBody = document.getElementById('ldrTableBody');";
  page += "  ldrTableBody.innerHTML = data.map(r => `<tr><td>${r.value} lux</td><td>${r.timestamp}</td></tr>`).join('');";
  page += "}";
  page += "</script>";
  
  page += "</head><body>";

  // HTML for the dashboard
  page += "<h1>Smart Object Dashboard</h1>";
  page += "<p>Temperature: <span id='temp'>Loading...</span></p>";
  page += "<p>Humidity: <span id='hum'>Loading...</span></p>";
  page += "<p>Light Intensity: <span id='light'>Loading...</span></p>";

  // Buttons for manual fan control (only if in MANUAL mode)
  if (fanMode == "MANUAL") {
    page += "<button onclick=\"location.href='/manualFanOn'\">Start Fan</button>";
    page += "<button onclick=\"location.href='/manualFanOff'\">Stop Fan</button>";
  }

  // Button to fetch and display LDR data
  page += "<button onclick=\"fetchLDRData()\">Show Last 15 LDR Data</button>";

  // Table for displaying LDR data
  page += "<table border='1' style='margin-top:20px;'>";
  page += "<thead><tr><th>Light Intensity</th><th>Timestamp</th></tr></thead>";
  page += "<tbody id='ldrTableBody'></tbody>";
  page += "</table>";

  // Link to settings page
  page += "<a href='/settings'>Go to Settings</a>";
  page += "</body></html>";

  return page;
}

// Function to Generate Settings Page HTML
String settingsPage() {
  String page = "<!DOCTYPE html><html><head><title>Settings</title></head><body>";
  page += "<h1>Configuration Settings</h1>";

  // Smart Object Identification
  page += "<h2>Smart Object Identification</h2>";
  page += "<form action='/setNode' method='POST'>";
  page += "<input type='text' name='node' placeholder='Enter unique name'>";
  page += "<button type='submit'>Set Name</button>";
  page += "</form>";

  // Posting Method
  page += "<h2>Posting Method</h2>";
  page += "<p>Current Method: " + postingMethod + "</p>";
  page += "<button onclick=\"location.href='/togglePosting'\">Switch Posting Method</button>";

  // Fan Mode and Trigger Temperature
  page += "<h2>Fan Control</h2>";
  page += "<p>Fan Mode: " + fanMode + "</p>";
  page += "<button onclick=\"location.href='/toggleFanMode'\">Toggle Fan Mode</button>";

  if (fanMode == "AUTO") {
    page += "<p>Trigger Temperature: " + String(triggerTemperature) + "°C</p>";
    page += "<form action='/setTemp' method='POST'>";
    page += "<input type='number' step='0.1' name='temp' placeholder='Enter trigger temp'>";
    page += "<button type='submit'>Set Temperature</button>";
    page += "</form>";
  }

  // Heartbeat Rate Configuration
  page += "<h2>Heartbeat Configuration</h2>";
  page += "<p>Current Heartbeat Rate: " + String(heartbeatRate) + " ms</p>";
  page += "<form action='/setHeartbeat' method='POST'>";
  page += "<input type='number' name='rate' placeholder='Enter rate in ms'>";
  page += "<button type='submit'>Set Heartbeat Rate</button>";
  page += "</form>";

  page += "</body></html>";
  return page;
}




