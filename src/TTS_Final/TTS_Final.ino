#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
// #include "SD.h"
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

const char* ssid = "Mmm";
const char* password = "ramzeshdoe";

void setup()
{
    Serial.begin(115200);
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting...");
        delay(1500);
    }
      Serial.println("Connected");
    Serial.println("Coneected to wifi");
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); 
}

void CallApi()
{
    while (!Serial.available())
    {
        delay(100);
    }

    Serial.println("test 3");
    String persianText = Serial.readStringUntil('\n');
    persianText.trim(); 

    if (persianText.length() > 0)
    {
        String Url = "https://one-api.ir/tts/?token=116231:65805a94ad157&action=microsoft&lang=fa-IR-DilaraNeural&q=";
        Url += persianText;
        Serial.println("Requesting TTS from: " + Url);
        audio.connecttohost(Url.c_str());
    }
}

void loop()
{
    CallApi();
    audio.loop();
}
