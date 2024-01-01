#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>

// WiFi
const char* ssid = "Abed";
const char* password = "11335566";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/barcode";
const char *topictype = "esp32/type";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial barcodeSerial(2);

String receivedMessage; 

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    barcodeSerial.begin(9600, SERIAL_8N1, 1, 3);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");

    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
        receivedMessage += (char) payload[i];
    }
    Serial.println();
    Serial.println("-----------------------");
    Serial.print("Received Message: ");
    Serial.println(receivedMessage);
}

void loop() {
    client.loop();
    if (barcodeSerial.available()) {
        String data;
        while (barcodeSerial.available()) {
            char ch = barcodeSerial.read();
            data += ch;
            delay(2);
        }
        Serial.println(data);
        Serial.println("Received Message: ");
        client.publish(topic, data.c_str());
        client.subscribe(topictype); // اشتراک گذاری تاپیک بعد از ارسال بارک

        Serial.print("Received Message: ");
        Serial.println(receivedMessage);
        if (!receivedMessage.isEmpty()) {
        client.publish(topic, receivedMessage.c_str());
        receivedMessage = "";
      }
      // else{
      //    Serial.println("Can't receive ");
      // }
    } 
    else {
        Serial.println("No data available");
        // client.publish(topic, "No data");
        // client.subscribe(topic);
    }
}