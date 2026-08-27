#include <WiFi.h>
#include <HTTPClient.h>

// ================= WIFI =================
const char* ssid = "Realme";
const char* password = "00000000";

// ================= SMS API =================
const char* apiKey = "cd_luc_260526_oOx-KN";
const char* templateID = "103";
const char* mobileNumber = "918019630920";

const char* var1 = "GAS ALERT";
const char* var2 = "Gas Leakage Detected";

// ================= PINS =================
#define MQ6_PIN     27
#define BUZZER_PIN  25
#define LED_PIN     23

bool smsSent = false;

// ================= SEND SMS =================
void sendSMS()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi Not Connected");
    return;
  }

  HTTPClient http;

  String url =
      "http://www.circuitdigest.cloud/api/v1/send_sms?ID=" +
      String(templateID);

  http.begin(url);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", apiKey);

  String payload =
      "{\"mobiles\":\"" + String(mobileNumber) +
      "\",\"var1\":\"" + String(var1) +
      "\",\"var2\":\"" + String(var2) +
      "\"}";

  Serial.println("Sending SMS...");

  int httpCode = http.POST(payload);

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0)
  {
    Serial.println("SMS Sent Successfully");
    Serial.println(http.getString());
  }
  else
  {
    Serial.println("SMS Failed");
  }

  http.end();
}

void setup()
{
  Serial.begin(115200);

  pinMode(MQ6_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.println();
  Serial.println("Starting System...");
  Serial.println("Connecting to WiFi");

  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.println("WiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("WiFi Connection Failed");
  }

  Serial.println("MQ-6 Warmup Started...");
  delay(20000);
  Serial.println("System Ready!");
}

void loop()
{
  int gasState = digitalRead(MQ6_PIN);

  Serial.print("MQ6 State: ");
  Serial.println(gasState);

  // Most MQ-6 modules become LOW when gas is detected
  if (gasState == LOW)
  {
    Serial.println("GAS DETECTED!");

    if (!smsSent)
    {
      sendSMS();
      smsSent = true;
    }

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(200);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    smsSent = false;

    delay(500);
  }
}