#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// https://one-api.ir/2022/12/29/api-%D9%85%D8%AA%D9%86-%D8%A8%D9%87-%DA%AF%D9%81%D8%AA%D8%A7%D8%B1-tts-api/
//api_token : 116231:65805a94ad157
const char* ssid = "SG";
const char* password = "pfdn2266";
const char* endpoint = "http://127.0.0.1:8000/user";

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

}

void loop() {
  // Get Persian text input (replace this with your input method)
  // String persianText = "سلام، چطوری؟";
    // Wait for user input from the Serial console
  while (!Serial.available()) {
    delay(100);
  }

  // Read the Persian text from the Serial console
  String persianText = Serial.readStringUntil('\n');
  persianText.trim(); // Remove leading and trailing whitespaces

  // Check if the input is not empty
  if (persianText.length() > 0) {
  // String persianText = urlEncode("higggg");
  // Serial.print(persianText);
  // Convert Persian text to speech using gTTS API
  // String Url = String(endpoint) + "?word=" + persianText;
  // String Url = "http://127.0.0.1:8000/user?word=hiii";
  String Url  = "https://one-api.ir/tts/?token=116231:65805a94ad157&action=microsoft&lang=fa-IR-DilaraNeural&q=";
  Url = Url + persianText;
  Serial.println("Requesting TTS from: " + Url);

  HTTPClient http;

  if (http.begin(Url)) {
    int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
      // Print the speech to the serial port
      Serial.println("Speech: " + String(speech));
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
