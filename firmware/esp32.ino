#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "DHT.h"

/* ===== WiFi ===== */
const char* WIFI_SSID = "Airbox-F9B3";
const char* WIFI_PASS = "f75J4Gk7";

/* ===== ThingSpeak ===== */
String apiKey = "TFFNY56ZUE2YTF4V";

/* ===== IFTTT ===== */
const char* IFTTT_EVENT = "dark_alert";
const char* IFTTT_KEY   = "oaQnaDD_yG2-pN-w9bW3OMS9yAyOAWQCNvoGhBS5-zI";

/* ===== Pins ===== */
#define DHTPIN      27
#define DHTTYPE     DHT22
#define LDR_DO_PIN  26
#define LED_PIN     14

DHT dht(DHTPIN, DHTTYPE);

/* ===== DARK TIMER (20 seconds for test) ===== */
const unsigned long DARK_TIME_REQUIRED = 20UL * 1000UL;
unsigned long darkStartTime = 0;
bool emailSent = false;

/* ===== Send email via IFTTT ===== */
void sendDarkEmail() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("IFTTT: WiFi not connected");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();   // IMPORTANT for HTTPS on ESP32

  HTTPClient http;
  String url = String("https://maker.ifttt.com/trigger/") +
               IFTTT_EVENT + "/with/key/" + IFTTT_KEY;

  Serial.println("Sending IFTTT email...");
  Serial.println(url);

  if (!http.begin(client, url)) {
    Serial.println("IFTTT http.begin failed");
    return;
  }

  int code = http.GET();
  Serial.print("IFTTT HTTP code: ");
  Serial.println(code);

  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  delay(2000);

  pinMode(LDR_DO_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /* ===== Light sensor ===== */
  int rawDO = digitalRead(LDR_DO_PIN);

  // Your sensor behavior: HIGH = DARK
  bool isDark = (rawDO == HIGH);

  /* ===== DHT ===== */
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  /* ===== LED ===== */
  if (!isnan(temp) && temp < 26.0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  /* ===== DARK TIMER ===== */
  unsigned long now = millis();

  if (isDark) {
    if (darkStartTime == 0) darkStartTime = now;

    unsigned long darkFor = now - darkStartTime;
    Serial.print("DARK for ");
    Serial.print(darkFor / 1000);
    Serial.println(" seconds");

    if (!emailSent && darkFor >= DARK_TIME_REQUIRED) {
      sendDarkEmail();
      emailSent = true;   // send once per dark period
    }
  } else {
    darkStartTime = 0;
    emailSent = false;
    Serial.println("BRIGHT (reset)");
  }

  /* ===== Serial ===== */
  Serial.print("Light: ");
  Serial.print(isDark ? "DARK" : "BRIGHT");

  if (!isnan(temp) && !isnan(hum)) {
    Serial.print(" | Temp: ");
    Serial.print(temp);
    Serial.print(" C | Hum: ");
    Serial.print(hum);
    Serial.println(" %");
  } else {
    Serial.println(" | DHT FAILED");
  }

  delay(1000);   // 1-second resolution for timer
}
