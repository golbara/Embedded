#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Audio.h>

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the I2S interface for audio output
  Audio.begin();
}

void loop() {
  // Get Persian text input (replace this with your input method)
  // Wait for user input from the Serial console
  while (!Serial.available()) {
    delay(100);
  }

  // Read the Persian text from the Serial console
  String persianText = Serial.readStringUntil('\n');
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
        Audio.connecttohost("http://your-audio-server/audio.ogg");
        Audio.i2s_output_begin();

        // Read and play the audio file
        while (http.connected() || http.available()) {
          bytesRead = http.readBytes(buffer, bufferSize);
          Audio.write(buffer, bytesRead);
        }

        // Close the I2S connection
        Audio.i2s_output_end();
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

  // Clear the Serial buffer
  while (Serial.available()) {
    Serial.read();
  }

  delay(5000); // Adjust the delay based on your needs
}
