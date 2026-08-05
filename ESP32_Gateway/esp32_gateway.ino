#define BLYNK_TEMPLATE_ID "TMPL37Khjf63-"
#define BLYNK_TEMPLATE_NAME "Iot Enabled Smart Sugarcane Farming System"
#define BLYNK_AUTH_TOKEN "Dd_uIx_r-_hzW7oX6ayk6247u978bgel"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

const char* serverUrl = "YOUR_SERVER_URL";

#define SS   5
#define RST  14
#define DIO0 26

#define PUMP   13
#define VALVE1 27
#define VALVE2 25
#define VALVE3 33

bool valve1State = false;
bool valve2State = false;
bool valve3State = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

String msg = "";

BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Starting...");

  pinMode(PUMP, OUTPUT);
  pinMode(VALVE1, OUTPUT);
  pinMode(VALVE2, OUTPUT);
  pinMode(VALVE3, OUTPUT);

  digitalWrite(PUMP, HIGH);
  digitalWrite(VALVE1, HIGH);
  digitalWrite(VALVE2, HIGH);
  digitalWrite(VALVE3, HIGH);

  // ---------- WiFi ----------
  WiFi.begin(ssid, pass);

  lcd.clear();
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());

  // ---------- LoRa FIRST ----------
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    lcd.clear();
    lcd.print("LoRa Failed");
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x34);

  Serial.println("LoRa Ready");

  lcd.clear();
  lcd.print("LoRa Ready");

  // ---------- Blynk AFTER LoRa ----------
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(3000);   // timeout 3 sec only

  Serial.println("System Ready");
}

void loop() {
  Blynk.run();
  timer.run();

  receiveLoRa();
}

void receiveLoRa() {
  int packetSize = LoRa.parsePacket();

  if (!packetSize) return;

  msg = "";

  while (LoRa.available()) {
    msg += (char)LoRa.read();
  }

  Serial.println("Received: " + msg);

  showOnLCD(msg);
  parseAndSend(msg);
}

void parseAndSend(String data) {
  int nodeId = 0;
  int temperature = 0;
  int humidity = 0;
  int soil = 0;
  int rain = 0;

  int parsed = sscanf(
    data.c_str(),
    "Node %d: T:%d H:%d S:%d R:%d",
    &nodeId,
    &temperature,
    &humidity,
    &soil,
    &rain
  );

  if (parsed != 5) {
    Serial.println("Parsing failed");
    return;
  }

  String nodeLabel = "unknown";

  if (nodeId == 1) nodeLabel = "node1";
  if (nodeId == 2) nodeLabel = "node2";
  if (nodeId == 3) nodeLabel = "node3";

  String jsonData = "{";
  jsonData += "\"node\":\"" + nodeLabel + "\",";
  jsonData += "\"temperature\":" + String(temperature) + ",";
  jsonData += "\"humidity\":" + String(humidity) + ",";
  jsonData += "\"soil_moisture\":" + String(soil) + ",";
  jsonData += "\"rain\":" + String(rain);
  jsonData += "}";

  Serial.println(jsonData);

  sendPOST(jsonData);
}

void sendPOST(String payload) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");

  int responseCode = http.POST(payload);

  Serial.print("HTTP Code: ");
  Serial.println(responseCode);

  String response = http.getString();
  Serial.println(response);

  http.end();
}

void showOnLCD(String data) {
  int splitPoint = data.indexOf("S:");

  if (splitPoint == -1) splitPoint = 16;

  String line1 = data.substring(0, splitPoint);
  String line2 = data.substring(splitPoint);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16));

  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, 16));
}

void updatePump() {
  if (valve1State || valve2State || valve3State)
    digitalWrite(PUMP, LOW);
  else
    digitalWrite(PUMP, HIGH);
}

BLYNK_WRITE(V0) {
  valve1State = param.asInt();
  digitalWrite(VALVE1, valve1State ? LOW : HIGH);
  updatePump();
}

BLYNK_WRITE(V1) {
  valve2State = param.asInt();
  digitalWrite(VALVE2, valve2State ? LOW : HIGH);
  updatePump();
}

BLYNK_WRITE(V2) {
  valve3State = param.asInt();
  digitalWrite(VALVE3, valve3State ? LOW : HIGH);
  updatePump();
}