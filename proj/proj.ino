#include <WiFi.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;

// WiFi
String ssid = "Mmm";
String password = "ramzeshdoe";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "speech";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String receivedMessage;

void setup()
{
  // SetUP board
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  Serial.begin(115200);
  SD.begin(SD_CS);
  
  // wifi connection
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wi-Fi network");
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21);

  // WiFi.begin(ssid, password);
  // // barcodeSerial.begin(9600, SERIAL_8N1, 1, 3);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi..");
  // }
  // Serial.println("Connected to the Wi-Fi network");

  // setup mqtt
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public EMQX MQTT broker connected");
    }
    else
    {
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
  client.loop();
  client.subscribe(topic); 
  Serial.println(receivedMessage);
  if (!receivedMessage.isEmpty())
  {
    CallApi(receivedMessage);
    client.publish(topic, receivedMessage.c_str());
    receivedMessage = "";
  }
  audio.loop();
}