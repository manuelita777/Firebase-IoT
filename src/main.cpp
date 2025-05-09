#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase helpers
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
// Timestamp
String getLocalTimeISO();
String getLocalTimeUNIX();

// NTP
#define NTP_SERVER "pool.ntp.org"
#define NTP_GMT_OFFSET_SEC 3600
#define NTP_DAYLIGHT_OFFSET_SEC 3600

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Global variables
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup()
{
  dht.begin();


  pinMode(13,INPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(25,OUTPUT);


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

  // Initialize NTP
  configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER);

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
  int red=0;
  int green=0;
  int blue=0;
  bool bb =false;
  
  static int intValue = 0;
  float floatValue = 0.0;
  static bool boolValue = true;
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0))
  {
    
    // Firebase is ready, we are signup and 10 seconds has passed
    // Save current time
    sendDataPrevMillis = millis();

    String mitiempo = getLocalTimeUNIX();
    String miti = getLocalTimeISO();

    // Write sample int
    Serial.print("INT WRITE ");
    intValue++;
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/"+mitiempo+"/humedad", dht.readHumidity()))
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
    if (Firebase.RTDB.setFloat(&fbdo, "sensor/"+mitiempo+"/temperatura", dht.readTemperature()))
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
    if (Firebase.RTDB.setString(&fbdo, "sensor/"+mitiempo+"/timestamp", miti))
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
String y;
if (dht.readTemperature()>15 && dht.readTemperature()<25 && dht.readHumidity()<60)
{
  y="chillin";
}
else {
  y="malo";
}


Serial.print("BOOLEAN WRITE ");
    if (Firebase.RTDB.setString(&fbdo, "sensor/"+mitiempo+"/confort", y))
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
    /*Serial.print("STRING WRITE ");
    if (Firebase.RTDB.setString(&fbdo, "sensor/"+mitiempo+"/timestamp", fbdo.stringData()))
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
    }*/

    // Read sample int
    
    Serial.print("read/int: ");
    if (Firebase.RTDB.getInt(&fbdo, "actuador/regb/blue", &red))
    {
      Serial.println(red);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }


    Serial.print("read/int: ");
    if (Firebase.RTDB.getInt(&fbdo, "actuador/regb/green", &green))
    {
      Serial.println(green);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    
    Serial.print("read/int: ");
    if (Firebase.RTDB.getInt(&fbdo, "actuador/regb/red", &blue))
    {
      Serial.println(blue);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    // Read sample float
   /* float floatRead = 0.0;
    Serial.print("read/float: ");
    if (Firebase.RTDB.getFloat(&fbdo, "read/float", &floatRead))
    {
      Serial.println(floatRead);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }*/

    // Read sample boolean
    
    Serial.print("read/boolean: ");
    if (Firebase.RTDB.getBool(&fbdo, "actuador/led", &bb))
    {
      Serial.println(bb);
      
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }

    /*// Read sample string
    String stringRead = "";
    Serial.print("read/string: ");
    if (Firebase.RTDB.getString(&fbdo, "read/string", &stringRead))
    {
      Serial.println(stringRead);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("  REASON: " + fbdo.errorReason());
    }*/

    // Print time
   /**String timestamp_unix = getLocalTimeUNIX();
    Serial.println("UNIX: " + timestamp_unix);

    String timestamp_iso = getLocalTimeISO();
    Serial.println("ISO: " + timestamp_iso);
*/

    
      analogWrite(27, red);
    analogWrite(26, green);
   analogWrite(25, blue);
   digitalWrite(14,bb);
    
  }

  

  
  

  


}

String getLocalTimeISO()
{
  struct tm timeinfo;
  char buffer[20];

  // Get local time
  if(!getLocalTime(&timeinfo))
  {
    return "NTP Error!";
  }

  // Obtain ISO 8601 timestamp
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  return String(buffer);
}

String getLocalTimeUNIX()
{
  struct tm timeinfo;

  // Get local time
  if(!getLocalTime(&timeinfo))
  {
    return "NTP Error!";
  }

  // Obtain UNIX timestamp
  return String(mktime(&timeinfo));
}
