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
char ssidAP[] = "ESP32AP";
char passwordAP[] = "12345678";
IPAddress local_ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// MQTT Settings
const char* mqttServer = "broker.hivemq.com"; //  MQTT broker
const int mqttPort = 1883;
const char* mqttTopic = "iot/sensors";

// HTTP Settings
const char* httpServer = "http://localhost/iot_smart_object/db/create_sensor_reading.php"; 

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
  Serial.println("Starting program...");

  // Initialize Wi-Fi Access Point
  Serial.println("Setting up Wi-Fi Access Point...");
  WiFi.mode(WIFI_AP);
  if (WiFi.softAP(ssidAP, passwordAP)) {
    Serial.println("Wi-Fi Access Point created successfully.");
  } else {
    Serial.println("Failed to create Wi-Fi Access Point!");
  }

  if (WiFi.softAPConfig(local_ip, gateway, subnet)) {
    Serial.println("Soft AP Config set successfully.");
  } else {
    Serial.println("Failed to set Soft AP Config!");
  }
  Serial.println("Done setting up access point.");

  // Initialize Web Server Routes
  Serial.println("Setting up web server routes...");
  server.on("/", []() { 
    Serial.println("Serving main page.");
    server.send(200, "text/html", mainPage()); 
  });
  server.on("/settings", []() { 
    Serial.println("Serving settings page.");
    server.send(200, "text/html", settingsPage()); 
  });
  server.on("/togglePosting", []() { 
    Serial.println("Toggling posting method.");
    togglePostingMethod(); 
  });
  server.on("/toggleFanMode", []() { 
    Serial.println("Toggling fan mode.");
    toggleFanMode(); 
  });
  server.on("/manualFanOn", []() { 
    Serial.println("Turning manual fan on.");
    setManualFanState(true); 
  });
  server.on("/manualFanOff", []() { 
    Serial.println("Turning manual fan off.");
    setManualFanState(false); 
  });
  server.on("/setTemp", []() { 
    Serial.println("Updating trigger temperature.");
    updateTriggerTemperature(); 
  });
  server.on("/setNode", []() { 
    Serial.println("Updating node.");
    updateNode(); 
  });

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

  Serial.println("Starting web server...");
  server.begin();
  Serial.println("Web server started.");

  Serial.println("Setting up MQTT client...");
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.println("MQTT client setup complete.");

  Serial.println("Setup Complete. Connect to ESP32AP.");
}


// Loop Function
void loop() {
  blinkHeartbeat();        // Blink Heartbeat LED
  server.handleClient();   // Handle Web Server Requests
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

  Serial.printf("Temp: %.2f°C, Hum: %.2f%%, Light: %d\n", temperature, humidity, lightIntensity);
}



void LDR(){

  int desiredInterval=6000;
  static long previous=0;
  long current=millis();

  if (current-previous>=desiredInterval){
    lightIntensity = analogRead(LDRPIN); // take the ldr reading
    previous=current;  
    Serial.println(lightIntensity);

    String jsonPayload = "{";
    jsonPayload += "\"node_id\":" + node + ",";
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
    jsonPayload += "\"node_id\":" + node + ",";
    jsonPayload += "\"sensor_id\":\"temp\","; 
    jsonPayload += "\"sensor_value\":" + String(temperature);
    jsonPayload += "}";

    postData(jsonPayload);
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
    jsonPayload += "\"node_id\":" + String(node) + ",";      // Convert node to a String
    jsonPayload += "\"sensor_id\":\"humidity\",";           // Add quotes around "humidity"
    jsonPayload += "\"sensor_value\":" + String(humidity);  // Convert humidity to a String
    jsonPayload += "}";

    postData(jsonPayload);
  }
}





// Function to blink LED twice every 2 seconds
void blinkHeartbeat() {
    int ledState = LOW;    // Current state of the LED
    int blinkCount = 0;    // Count of blinks in the current cycle
    long previousMillis = 0; // Time tracker
    const long interval = 500; // Interval for each blink (500ms for on/off)
    static long cycleStartMillis = 0; // Start time of the 2-second cycle
    long currentMillis = millis();

  // Check if 2 seconds have passed since the cycle started
  if (currentMillis - cycleStartMillis >= 2000) {
    cycleStartMillis = currentMillis; // Reset the cycle
    blinkCount = 0;                   // Reset blink count
  }

  // Handle LED blinking
  if (blinkCount < 4) { // Each blink (on/off) takes 500ms, so 4 changes make 2 blinks
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; // Update the last time the LED changed state
      ledState = !ledState;           // Toggle LED state
      digitalWrite(LEDPIN, ledState); // Set the LED state
      blinkCount++;                   // Increment blink count
    }
  } else {
    // Ensure the LED is off after blinking
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


  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response: %d\n", httpResponseCode);
  } else {
    Serial.printf("HTTP POST Failed: %s\n", http.errorToString(httpResponseCode).c_str());
  }

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
      delay(5000);
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


