#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <DHT.h>

// Define Pins and Constants
#define DHTPIN 4          // Pin connected to the DHT sensor
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


IPAddress local_ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// MQTT Settings
const char* mqttServer = "broker.hivemq.com"; //  MQTT broker
const int mqttPort = 1883;
const char* mqttTopic = "iot/sensors";

// HTTP Settings
const char* httpServer = "http://172.20.10.2/iot_smart_object/db/create_sensor_reading.php"; 

// Variables
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity;
int lightIntensity;
String postingMethod = "HTTP"; // Default posting method
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
  

// // Setup Function
// void setup() {
//   Serial.begin(115200);

//   Serial.println("Starting program")
//   // Initialize Wi-Fi Access Point
//   WiFi.mode(WIFI_AP);
//   WiFi.softAP(ssidAP, passwordAP);
//   WiFi.softAPConfig(local_ip, gateway, subnet);
  
//   Serial.println("Done setting up access point")

//   // Initialize Web Server Routes
//   server.on("/", []() { server.send(200, "text/html", mainPage()); });
//   server.on("/settings", []() { server.send(200, "text/html", settingsPage()); });
//   server.on("/togglePosting", togglePostingMethod);
//   server.on("/toggleFanMode", toggleFanMode);
//   server.on("/manualFanOn", []() { setManualFanState(true); });
//   server.on("/manualFanOff", []() { setManualFanState(false); });
//   server.on("/setTemp", updateTriggerTemperature);
//   server.on("/setNode", updateNode);

  

//   // Initialize Pins
//   pinMode(FANPIN, OUTPUT);
//   pinMode(LEDPIN, OUTPUT);
//   digitalWrite(FANPIN, LOW); // Fan off
//   digitalWrite(LEDPIN, LOW); // LED off

//   // Initialize Sensors and MQTT
//   dht.begin();
//   server.begin();

//   mqttClient.setServer(mqttServer, mqttPort);
//   Serial.println("Setup Complete. Connect to ESP32AP.");
// }


void setup() {
  Serial.begin(115200);

  WiFi.begin(sta_ssid, sta_password);
    Serial.println("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

  delay(2000);
  Serial.println("Starting program...");

  // Initialize Wi-Fi Access Point
  Serial.println("Setting up Wi-Fi Access Point...");
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passwordAP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("Initialize Web Server Routes...");
  delay(1000);
  // Initialize Web Server Routes
  server.on("/", []() { server.send(200, "text/html", mainPage()); });
  server.on("/settings", []() { server.send(200, "text/html", settingsPage()); });
  server.on("/togglePosting", togglePostingMethod);
  server.on("/toggleFanMode", toggleFanMode);
  server.on("/manualFanOn", []() { setManualFanState(true); });
  server.on("/manualFanOff", []() { setManualFanState(false); });
  server.on("/setTemp", updateTriggerTemperature);

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
    Serial.printf("Light: %d\n", lightIntensity);

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
    Serial.printf("Temp: %.2f°C", temperature);
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
    Serial.printf("Hum: %.2f%%", humidity);
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


  Serial.println(jsonPayload);
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


  if (mqttClient.publish(mqttTopic, jsonPayload.c_str())) {
    Serial.println("MQTT Message Sent Successfully");
  } else {
    Serial.println("MQTT Message Sending Failed");
  }
}



// Function to Setup MQTT Connection
void setupMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("MQTT Connection Failed. Retrying in 5 seconds...");
      delay(500);
    }
  }
}




// Function to Toggle Posting Method
void togglePostingMethod() {
  postingMethod = (postingMethod == "HTTP") ? "MQTT" : "HTTP";
  server.send(200, "text/html", settingsPage());
}


// Function to Toggle Fan Mode
void toggleFanMode() {
  fanMode = (fanMode == "AUTO") ? "MANUAL" : "AUTO";
  server.send(200, "text/html", settingsPage());
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
  if (server.hasArg("node")) {
    node = server.arg("node");
  }
  server.send(200, "text/html", settingsPage());
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

    page += "<p>Current node: " + node + "</p>";
    page += "<form action='/setNode' method='POST'><input type='text' name='node'>";
    page += "<button type='submit'>Set Node</button></form>";

  if (fanMode == "AUTO") {
    page += "<p>Trigger Temperature: " + String(triggerTemperature) + "°C</p>";
    page += "<form action='/setTemp' method='POST'><input type='number' name='temp'>";
    page += "<button type='submit'>Set Temperature</button></form>";
  } else {
    page += "<button onclick=\"location.href='/manualFanOn'\">Turn Fan On</button>";
    page += "<button onclick=\"location.href='/manualFanOff'\">Turn Fan Off</button>";
  }
  page += "</body></html>";
  return page;
}


