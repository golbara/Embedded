#include <WiFi.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "Audio.h"
#include "FS.h"

#define I2S_BCLK 27
#define I2S_LRC 26
#define I2S_DOUT 25

Audio audio;

// WiFi
String ssid = "SG";
String password = "pfdn2266";

// // MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "speech";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

int counter = 0;
WiFiClient espClient;
PubSubClient client(espClient);

String receivedMessage;

void setup()
{

  Serial.begin(4800);
  WiFi.disconnect();
  Serial.println("start");
  WiFi.begin(ssid, password);
  Serial.println("log");
  // Serial.println("log";)
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.println("Connecting...");
      delay(1500);
  }
    Serial.println("Coneected to wifi");
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(20); 
    Serial.println("last log");

  // setup mqtt
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    // String client_id = "esp32-client-";
    String client_id = "mqttx_5ad70d12";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
        Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
    Serial.println("test");
      Serial.println("Public EMQX MQTT broker connected");
      delay(20000);

    }
    else
    {
          Serial.println("1");
    Serial.println("1");
    Serial.println("1");
    Serial.println("1");
    Serial.println("1");
    Serial.println("1");
    Serial.println("1");
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  
  }
}

void CallApi(String receivedMessage)
{
    String persianText = receivedMessage;
    persianText.trim(); 
    if (persianText.length() > 0)
    {
        String Url = "https://one-api.ir/tts/?token=116231:65805a94ad157&action=microsoft&lang=fa-IR-DilaraNeural&q=";
        Url += persianText;
        Serial.println("Requesting TTS from: " + Url);
        delay(10000);
        audio.connecttohost(Url.c_str());
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    receivedMessage += (char)payload[i];
  }
  Serial.println();
  Serial.println("-----------------------");
  Serial.print("Received Message: ");
  Serial.println(receivedMessage);
}

void loop()
{
  // CallApi(rec);
  client.subscribe(topic); 
  Serial.println("HELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLO");
  Serial.println(receivedMessage);
  if (!receivedMessage.isEmpty())
  {
    CallApi(receivedMessage);
    client.publish(topic, receivedMessage.c_str());
    receivedMessage = "";
  }
  audio.loop();
  client.loop();
}