#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase helpers
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// WiFi credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// Firebase API key
#define API_KEY "YOUR_API_KEY"

// Firebase RTDB URL
#define DATABASE_URL "YOUR_RTDB_URL"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Global variables
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup()
{
  // Initialize Serial
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(333);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase signup ok!");
    signupOK = true;
  }
  else
  {
    Serial.println("Firebase signup failed!");
  }

  // Assign callback function for token generation task
  config.token_status_callback = tokenStatusCallback;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop ()
{
  static int intValue = 0;
  float floatValue = 0.0;
  static bool boolValue = true;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0))
  {
    // Firebase is ready, we are signup and 10 seconds has passed
    // Save current time
    sendDataPrevMillis = millis();

    // Write sample int
    Serial.print("INT WRITE ");
    intValue++;
    if (Firebase.RTDB.setInt(&fbdo, "test/int", intValue))
    {
      Serial.println("OK");
      Serial.println("  PATH: " + fbdo.dataPath());
      Serial.println("  TYPE: " + fbdo.dataType());
      Serial.print("  VALUE: ");
      Serial.println(fbdo.intData());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    // Write sample float
    floatValue = 0.01 + random (0,100);
    Serial.print("FLOAT WRITE ");
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", floatValue))
    {
      Serial.println("OK");
      Serial.println("  PATH: " + fbdo.dataPath());
      Serial.println("  TYPE: " + fbdo.dataType());
      Serial.print("  VALUE: ");
      Serial.println(fbdo.floatData());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    // Write sample boolean
    boolValue = !boolValue;
    Serial.print("BOOLEAN WRITE ");
    if (Firebase.RTDB.setBool(&fbdo, "test/boolean", boolValue))
    {
      Serial.println("OK");
      Serial.println("  PATH: " + fbdo.dataPath());
      Serial.println("  TYPE: " + fbdo.dataType());
      Serial.print("  VALUE: ");
      Serial.println(fbdo.boolData());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    // Write sample string
    Serial.print("STRING WRITE ");
    if (Firebase.RTDB.setString(&fbdo, "test/string", "KAIXO!"))
    {
      Serial.println("OK");
      Serial.println("  PATH: " + fbdo.dataPath());
      Serial.println("  TYPE: " + fbdo.dataType());
      Serial.print("  VALUE: ");
      Serial.println(fbdo.stringData());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }
  }
}