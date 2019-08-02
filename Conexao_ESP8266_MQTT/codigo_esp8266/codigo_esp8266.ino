/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WLL-Inatel";
const char* password = "inatelsemfio";
const char* mqtt_server = "smartcampus.inatel.br";
const char* mqtt_user = "5d1cebfd6a4890001a84a5ff"; //Colocar ID do Device
const char* mqtt_pass = "LRfiWHe96MqIkeRip0RWwGuc"; //Colocar token do Device
const char* topico_to_publish = "application_5d1cebdf6a4890001a84a5fc_device_5d1cebfd6a4890001a84a5ff/save"; //Colocar o tópico que irá publicar - /save
const char* topico_to_subscribe = "application_5d1cebdf6a4890001a84a5fc_device_5d1cebfd6a4890001a84a5ff/listen"; //Colocar o tópico que irá se inscrever - /listen

char* str_payload = "";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long int lastMsg = 0;
String msg;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  str_payload = "";
  int final_i = 0;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    str_payload[i] = ((char)payload[i]);
    final_i = i+1;
  }
  str_payload[final_i] = '\0';
  Serial.println();
  Serial.println("PAYLOAD:");
  Serial.println(str_payload);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect( clientId.c_str(), mqtt_user, mqtt_pass ) ) {
      Serial.println("connected");
      // Quando conectado, publica um "OK"
      client.publish(topico_to_publish, "{\"STATUS\":\"OK\"");
      // E se inscreve no topico
      client.subscribe(topico_to_subscribe);
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Espera 5s antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() - lastMsg > 5000) {
    Serial.print("Publish message: ");
    msg = "{";
    msg+= "\"millis_since_began\":" + String(millis());
    msg+=  "}";
    Serial.println(msg);
    client.publish(topico_to_publish, msg.c_str());
    lastMsg = millis();
  }
  
}
