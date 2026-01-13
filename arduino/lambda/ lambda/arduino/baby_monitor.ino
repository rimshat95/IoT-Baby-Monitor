#include <WiFi.h>
#include <WiFiClientSecure.h> // För säker MQTTS-kommunikation
#include <PubSubClient.h>     // För MQTT-protokollet

// ---- WIFI-UPPGIFTER ----
const char* ssid = "Tele2_60e6e0";
const char* password = "5vsxkf5u";

// ---- AWS IOT CORE-UPPGIFTER ----
const char* aws_mqtt_server = "a29cq3ntks7d7c-ats.iot.eu-north-1.amazonaws.com";
const int aws_mqtt_port = 8883; // MQTTS standardport

// Ditt unika Client ID för denna ESP32-enhet
const char* mqtt_client_id = "esp32-baby-monitor-01";

// AWS IoT Core X.509 Certifikat och Privata Nyckel (från din AWS IoT-enhet)
const char* certificate_pem_crt = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUa4X6Z2puC34D0fv2EdzRaGSV73IwDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDExMjAxMTky\n" \
"OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAO6D4/g0AWpJT8H4SBm9\n" \
"qyiwXyxW3xAGznGRC+vcuhbfF47b7Mfyp0EqMx1XfhO3RqSc0qW+tSZarFAC4kB/\n" \
"VTxI6fXFMWlNLaJyVZirVCYxmn8mPoEngRditjtlzkTQW5aZn1UpXOu/PsjAH418\n" \
"miyvHwOW6V/MIhPt/qrzgZJUkLHEBnZa9U8ia2wKD+0c5mNXknI7Rei2k54fJDeu\n" \
"11J+4Zqv3yDmxFb1kiTsRX55B7TJ6t+katPT8RMsik5XifZlyU2aD//hBEgpoddi\n" \
"zoPdQUe8J2URolxTvhpZ0sdTUXfbFGYRWwHzyCNfy++Y62qckAVqfXUox/4pD34T\n" \
"hU0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUUAbtohr5LBF3Q675XvOm1IeKm0YwHQYD\n" \
"VR0OBBYEFEH+5O9jTAfMCSrsxBj0GlTtZRkGMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQArd/JuJaDUvNTUTYvBDSuAcZAu\n" \
"pEjf8PaTCcCpnPCMcww7Zw6Y44oAkJtaKUmf56qoQbOZPZ3Hezfgrh/kUZISoB8Y\n" \ 
"dBvM8EPOuAXEX1FPRGeaWmOKQKZrexHS7U2GFXAPqkX+26/9/Z3tGS55+AEGCUXS\n" \
"KBW1dXwlhFWu/BqnRkSxf0ksDiM0f4aspSQwcHYq0jXcIt4RSRYsrInCpsl1k/wb\n" \
"IaDYJ6tCtJRjsx6Ua/zVZJFDOd2jtFL67Psh+R2xpPLznEr2idk3+04TNyHiC7ZK\n" \
"iJ1n5EYuOQZRhhAEw5xaeKatMFrmpyHH7XdUNgmU4M72vfnV8jVNtGRu+bZP\n" \
"-----END CERTIFICATE-----\n";


//  RSA PRIVATE KEY
const char* private_pem_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA7oPj+DQBaklPwfhIGb2rKLBfLFbfEAbOcZEL69y6Ft8Xjtvs\n" \
"x/KnQSozHVd+E7dGpJzSpb61JlqsUALiQH9VPEjp9cUxaU0tonJVmKtUJjGafyY+\n" \
"gSeBF2K2O2XORNBblpmfVSlc678+yMAfjXyaLK8fA5bpX8wiE+3+qvOBklSQscQG\n" \
"dlr1TyJrbAoP7RzmY1eScjtF6LaTnh8kN67XUn7hmq/fIObEVvWSJOxFfnkHtMnq\n" \
"36Rq09PxEyyKTleJ9mXJTZoP/+EESCmh12LOg91BR7wnZRGiXFO+GlnSx1NRd9sU\n" \
"ZhFbAfPII1/L75jrapyQBWp9dSjH/ikPfhOFTQIDAQABAoIBAQCBR1ePNpD1zRq/\n" \
"5hUebqGRAlfzrnplM2s+PwIZK6BpuS4JrsaecbWaKodeBgg+I37UxnOehsmlKkxv\n" \
"acC8eMf4TwUQw3tOuhhcDmC6yVJpCUXWWSJT2terl0Ny2nF89vpNnKKyBCNrXFNp\n" \
"eEkh5UGjqp/C0E9aL0jhSIRaoHXAm62Ch2TNTSKTpsNc4D72H3m0YGzXZbWCUpx+\n" \
"YJNZ6OvmrVnlNi+/omr13OABrnXcELbVSUDCa1b48vyRt39gZgK6ab0kGLCPzGUG\n" \
"wezKo8R+qBxvxD7itO5lU1jE8Dv7eDfz+2Ak7hHwt8rzjE8177TOsKNaTdocMYfB\n" \
"nUbCf+l5AoGBAP3GAtc1KIZdws1V7iJX2qs8esqVsX30cQChufV0lQVpjtyGCk2M\n" \
"s38TsvT5CO1topN/haI6SosKXNEeTCNoNr1x0vVqDMk4o8GwW7KqEj6kMPuaUXgt\n" \
"6xOF5yHZHrzdhjR3qs0zaHgDOkAp9+xattca3Fg76eQnY171NitPJFMLAoGBAPCb\n" \
"m8WOAe62urRFgPul5z0+KxS+srWMO8gMt9i8/M6GjkM+EiOw3iO4Yua/tepnPj1k\n" \
"pMSiUtDd53QZHKSXBCpVPGR8wVkb3nwYTyt+bjAtwu/YZKekNIut3cGn8wZ+LGQw\n" \
"JcFZ2UgDp/aeKJeZKEhbL1FSPEov7FPPVIbBh8AHAoGAfo89424TY89m01jDhkUi\n" \
"PLvQ0S+OcS5zHMmLICqgFy7pnHkMslg1PG7qlNmN5jsDFWRHlPxVKLpmR7RAbHgQ\n" \
"tLILb3EQJ1oj8PivAo9mwe7IrvHrJ3BfCEGDX5IUjkQ8JfZ4LBYzR4b0igNkLW+g\n" \
"N+6hbS+UGcC7AiRwe3DgwLkCgYEA7eFK3sSJkJNEf9p/rUzXFCuUuEvZr7hXmm9I\n" \
"jJY8c4uAt2Z01KiW10YDMhO0LW2pkVUhue6qZioeHSB+uxM7YQcoUEkvd4QgMRxv\n" \
"mZbEJgn4Qtz0lvG3qalaJ45+rZqtFNitWz4TnilPNHols8wOkYrle0cMqGQ9CJdh\n" \
"AGHybp0CgYBt/lF2GbgQ+CYmlexdtS5pDt/AdMGi6aPBy2TUJd1kb09J/cwS/she\n" \
"gtqMydwVTNbjGO+dKh9hdX8mywxCnlw06+1ofdfdw1jcmRVAvg9KrYvZJ6M2unoV\n" \
"u7B3eD1vzIxZ2+i9ds/wIwQCOLsIxmg3hOXeBMzK5Nd8OdR+aZod7Q==\n" \
"-----END RSA PRIVATE KEY-----\n";

const char* aws_root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";



// ---- HC-SR04 Pinnar ----
const int TRIG_PIN = 13; // ESP32 pin ansluten till HC-SR04 Trig pin
const int ECHO_PIN = 12; // ESP32 pin ansluten till HC-SR04 Echo pin
const float SOUND_SPEED_CM_US = 0.0343;

// ---- Avståndsinställningar för Spjälsängs-/Sängmonitor ----
const int THRESHOLD_BED_EMPTY_CM = 30; // Om avståndet är större än detta, antas sängen vara tom
const int MIN_VALID_DISTANCE_CM = 2;   // HC-SR04 minsta mätbara avstånd
const int MAX_VALID_DISTANCE_CM = 200; // HC-SR04 max ca 400cm, men begränsa för sänganvändning

// ---- MQTT-ämnen ----
const char* mqtt_publish_topic = "baby_monitor/bed_status"; // Ämne att publicera till

// ---- WiFi och MQTT Klientinstanser ----
WiFiClientSecure espClient; // Använder säker klient för MQTTS
PubSubClient client(espClient);

// ---- Tidshantering för publicering (skicka var 5:e sekund) ----
unsigned long lastMsg = 0;
const long publishInterval = 5000; // 5 sekunder

// ---- Funktion för att mäta avstånd ----
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms
  long distance = duration * SOUND_SPEED_CM_US / 2;

  // Enkel filtrering/validering
  if (duration == 0 || distance < MIN_VALID_DISTANCE_CM || distance > MAX_VALID_DISTANCE_CM) {
    return -1; // Indikera ogiltig avläsning
  }
  return distance;
}

// ---- Funktion för att återansluta till WiFi ----
void connectWiFi() {
  Serial.print("Ansluter till WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi ansluten!");
  Serial.print("IP-adress: ");
  Serial.println(WiFi.localIP());
}

// ---- Funktion för att återansluta till MQTT (AWS IoT Core) ----
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Ansluter till AWS IoT Core...");
    // För MQTTS med certifikat behövs ingen användarnamn/lösenord i connect()
    if (client.connect(mqtt_client_id)) {
      Serial.println("ansluten!");
    } else {
      Serial.print("misslyckades, rc=");
      Serial.print(client.state()); // Skriver ut felkod
      Serial.println(" - försöker igen om 5 sekunder");
      delay(5000);
    }
  }
}

// ---- Setup-funktion (körs en gång vid uppstart) ----
void setup() {
  Serial.begin(115200);
  delay(2000); // Ge tid för Serial Monitor att initiera
  Serial.println("\n--- IoT-baserad Spjälsängs-/Sängmonitor Startad ---");

  // Konfigurera HC-SR04 pinnar
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Anslut till WiFi
  connectWiFi();

  // Konfigurera säker WiFi-klient med certifikat
  espClient.setCACert(aws_root_ca);
  espClient.setCertificate(certificate_pem_crt);
  espClient.setPrivateKey(private_pem_key);

  // Ställ in MQTT-server
  client.setServer(aws_mqtt_server, aws_mqtt_port);
  // client.setBufferSize(512); // Kan behövas om payload blir stor
}

// ---- Loop-funktion (körs kontinuerligt) ----
void loop() {
  // Se till att WiFi är ansluten
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Se till att MQTT-klienten är ansluten
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop(); // Måste anropas regelbundet för att upprätthålla MQTT-anslutningen

  // Publicera data med ett visst intervall
  unsigned long now = millis();
  if (now - lastMsg > publishInterval) {
    lastMsg = now;

    long distance = readDistanceCm();
    String bed_status;
    String status_message;

    if (distance == -1) {
      bed_status = "error";
      status_message = "Mätfel/Utanför räckvidd";
      Serial.println(status_message);
    } else {
      Serial.print("Avstånd: ");
      Serial.print(distance);
      Serial.println(" cm");

      if (distance > THRESHOLD_BED_EMPTY_CM) {
        bed_status = "empty";
        status_message = "Sängen är tom!";
      } else {
        bed_status = "occupied";
        status_message = "Barn i sängen.";
      }
    }

    // Skapa JSON payload
    String payload = "{";
    payload += "\"distance\":" + String(distance) + ",";
    payload += "\"bed_status\":\"" + bed_status + "\",";
    payload += "\"message\":\"" + status_message + "\"";
    payload += "}";

    // Publicera telemetri
    if (client.publish(mqtt_publish_topic, payload.c_str())) {
      Serial.println("✓ Telemetri skickad: " + payload);
    } else {
      Serial.print("✗ Telemetri misslyckades! MQTT state: ");
      Serial.println(client.state());
    }
  }
}

