#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "SG";
const char* password = "pfdn2266";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "speech";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


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
        //String client_id = "mqttx_5ad70d12";
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
        client.subscribe(topic); // عضویت و نام نویسی در تاپیک مشخص شده برای دریافت پیامها
        Serial.println(receivedMessage);
          if (!receivedMessage.isEmpty()) {
            // ///////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////
      String persianText = receivedMessage;
        persianText.trim(); // Remove leading and trailing whitespaces

        if (persianText.length() > 0) {
          // Construct the TTS API URL
          String Url = "https://one-api.ir/tts/?token=116231:65805a94ad157&action=microsoft&lang=fa-IR-DilaraNeural&q=";
          Url += persianText;
          Serial.println("Requesting TTS from: " + Url);

          HTTPClient http;

          if (http.begin(Url)) {
            int httpResponseCode = http.GET();

            if (httpResponseCode == HTTP_CODE_OK) {
              // Read the audio data into a buffer
              const size_t bufferSize = 1024;  // Adjust the buffer size based on your needs
              uint8_t buffer[bufferSize];
              int bytesRead;

              // Open the I2S connection for audio playback
              AudioI2S.write(buffer, bufferSize);

              // Read and play the audio file
              while (http.connected() || http.available()) {
                bytesRead = http.readBytes(buffer, bufferSize);
                AudioI2S.write(buffer, bytesRead);
              }

              // Close the I2S connection
              AudioI2S.end();
            } else {
              Serial.println("Error in HTTP request. HTTP response code: " + String(httpResponseCode));
            }

            http.end();
          } else {
            Serial.println("Failed to connect to the server.");
          }
        } else {
          Serial.println("Please enter a Persian text.");
        }
          client.publish(topic, receivedMessage.c_str());
          receivedMessage = "";
        }
    } 
}