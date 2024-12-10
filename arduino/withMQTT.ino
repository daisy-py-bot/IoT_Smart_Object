//ok
 #include <WiFi.h>  

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//---- WiFi settings

const char* ssid = "Daisy";
const char* password = "dkt11.pyy";


//---- MQTT Broker settings
const char* mqtt_server = "172.20.10.2"; // replace with your broker url
//const char* mqtt_server = "172.20.10.2"; // replace with your broker url

const int mqtt_port =1883;

#define BUILTIN_LED 2

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];


const char* main_topic="FinalProjectYedu/#";




//==========================================
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //=======
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //=======
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}



//=====================================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);  //you could make this static
    // Attempt to connect
    if (client.connect(clientId.c_str())){//, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(main_topic);   // subscribe the topics here
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}

//================================================ setup
//================================================
void setup() {
  Serial.begin(115200);
  //while (!Serial) delay(1);
  Serial.println("Setting up");
  setup_wifi();
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  
  client.setServer(mqtt_server, 1883 );//mqtt_port
}




void MQTT(String jsonPayload){
  if (!client.connected()) reconnect();
  client.loop();
  publishMessage(node, jsonPayload,true); 

}






//======================================= publising as string
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}
