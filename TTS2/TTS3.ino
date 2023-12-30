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
  String Url  = "https://one-api.ir/tts/?token=116231:65805a94ad157&action=microsoft&lang=fa-IR-DilaraNeural&q=";
  Url = Url + persianText;
  Serial.println("Requesting TTS from: " + Url);
  HTTPClient http;
  if (http.begin(Url)) {
    int httpResponseCode = http.GET();
    http.end();
  } else {
    Serial.println("Failed to connect to the server.");
  }
  } else {
    Serial.println("Please enter a Persian text.");
  }

}
