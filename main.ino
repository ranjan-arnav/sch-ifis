#include <ESP8266WiFi.h>

const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";
const char* host = "your_server_ip";

const int soilMoisturePin = A0;
const int pumpPin = D1;
const int ureaValvePin = D2;
const int nitrogenValvePin = D3;

const int moistureThreshold = 500;
const int pumpDuration = 2000;
const int ureaDuration = 1000;
const int nitrogenDuration = 1000;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  pinMode(ureaValvePin, OUTPUT);
  pinMode(nitrogenValvePin, OUTPUT);

  digitalWrite(pumpPin, LOW);
  digitalWrite(ureaValvePin, LOW);
  digitalWrite(nitrogenValvePin, LOW);

  Serial.println("Starting setup...");
  connectToWiFi();
  Serial.println("Setup complete.");
}

void loop() {
  int soilMoisture = analogRead(soilMoisturePin);

  Serial.print("Soil Moisture Level: ");
  Serial.println(soilMoisture);

  if (soilMoisture < moistureThreshold) {
    Serial.println("Soil moisture is low. Starting irrigation process.");

    startIrrigation();
    Serial.println("Irrigation process completed.");

    Serial.println("Sending moisture data to the server...");
    bool sent = sendMoistureData(soilMoisture);
    if (sent) {
      Serial.println("Moisture data sent successfully.");
    } else {
      Serial.println("Failed to send moisture data.");
    }
  } else {
    Serial.println("Soil moisture level is adequate. No irrigation needed.");
  }

  Serial.println("Waiting for next check...");
  delay(60000); // Check soil moisture every minute
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi.");
    // Optional: add more error handling or retry logic
  }
}

void startIrrigation() {
  Serial.println("Activating pump...");
  digitalWrite(pumpPin, HIGH);
  delay(pumpDuration);
  digitalWrite(pumpPin, LOW);
  Serial.println("Pump deactivated.");

  Serial.println("Opening urea valve...");
  digitalWrite(ureaValvePin, HIGH);
  delay(ureaDuration);
  digitalWrite(ureaValvePin, LOW);
  Serial.println("Urea valve closed.");

  Serial.println("Opening nitrogen valve...");
  digitalWrite(nitrogenValvePin, HIGH);
  delay(nitrogenDuration);
  digitalWrite(nitrogenValvePin, LOW);
  Serial.println("Nitrogen valve closed.");
}

bool sendMoistureData(int moistureLevel) {
  if (client.connect(host, 80)) {
    String url = "/update?moisture=" + String(moistureLevel);
    Serial.print("Sending request: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(500); // Wait for the response to be sent
    client.stop();
    return true;
  } else {
    Serial.println("Connection to server failed.");
    return false;
  }
}
