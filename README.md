

# 🍼 IoT-baserad Babymonitor
### Spjälsängs-/Sängmonitor med ESP32 och AWS

**Student:** Rimsha  
**Utbildning:** Mjukvaruutvecklare i IoT och inbyggda system  

---

## 📋 Projektöversikt

Detta projekt är ett IoT-baserat babymonitorsystem som använder en ultraljudssensor för att detektera om ett barn finns i en spjälsäng eller säng. Systemet skickar realtidsdata till AWS molntjänster, lagrar data i en databas, utlöser notifikationer via Discord när sängen är tom, och visar data på en live webbdashboard.

**Live Dashboard:** https://staging.d2e6rhdbf0mrl0.amplifyapp.com/

---

## 🎯 Projektmål

- Demonstrera säker IoT-enhetskommunikation med MQTTS
- Implementera molnbaserad datalagring och bearbetning
- Skapa ett realtidsnotifikationssystem
- Bygga en webbaserad datavisualiseringsdashboard
- Tillämpa kunskap om inbyggda system, molntjänster och fullstack-utveckling

---

## 🛠️ Hårdvarukomponenter

| Komponent | Beskrivning | Syfte |

| **ESP32** | Mikrokontroller med WiFi | Huvudprocessenhet och IoT-gateway |
| **HC-SR04** | Ultraljud-avståndssensor | Mäter avstånd för att detektera säng-beläggning |
| **USB-kabel** | Ström och programmering | Strömförsörjer ESP32 och laddar upp kod |
| **Hoppledningar** | Anslutningar | Kopplar sensor till ESP32 |

### Hårdvaruanslutningar:
HC-SR04 → ESP32
VCC     → 5V
GND     → GND
TRIG    → GPIO 13
ECHO    → GPIO 12


---

## 🏗️ Systemarkitektur
<img width="1920" height="1080" alt="system-arkitektur" src="https://github.com/user-attachments/assets/b2279a1d-16a3-4029-b9a2-ce43baadea84" />

Skalbarhet:

Systemets skalbara design:

Systemet är arkitekterat med AWS-tjänster (Managed Services) som naturligt skalar från en enskild enhet till tusentals, utan krav på ombyggnation av infrastrukturen.

Horisontell skalning (Fler enheter):

•	Implementation: Varje ny ESP32 tilldelas ett unikt deviceId och ett eget X.509-certifikat.
•	Datalagring: DynamoDB använder deviceId som sort key, vilket möjliggör snabba sökningar även när datamängden växer för hundratals olika rum.
•	Kapacitet: AWS IoT Core hanterar miljontals samtidiga MQTT-anslutningar automatiskt.

Vertikal skalning (Fler funktioner):

Systemet är förberett för att hantera utökad sensor-data (t.ex. temperatur, luftfuktighet eller ljudnivå) genom att enkelt expandera JSON-payloaden:

{
  "deviceId": "esp32-01",
  "distance": 92,
  "temp": 22.5,
  "humidity": 45,
  "sound_level": "low"
} 

Framtida arkitektur för produktion:

För att gå från en simulation till en fullskalig konsumentprodukt krävs följande steg:

1.	API Gateway: Implementera ett REST API mellan DynamoDB och Dashboarden för att hämta live-data på ett säkert och kontrollerat sätt.
2.	AWS SNS: Ersätta enkla webhooks med Simple Notification Service för att kunna skicka larm via både SMS, E-post och Push-notiser samtidigt.
3.	Cognito: Lägga till användarautentisering så att föräldrar endast kan se data från sina egna specifika enheter.

Skalbarhetsmått:
Resurs:	      Nuvarande:	Skalbart till:
Enheter:	        1 st	     1 000+ st
Meddelanden: ~17 000 / dag	  17M+ / dag
Användare:	     1 st	     Obegränsat via Amplify CDN

Slutsats: Genom att använda en serverlös (Serverless) arkitektur minimeras både kostnad och administrativt arbete vid tillväxt, då AWS sköter all resursallokering automatiskt.


## ☁️ AWS-tjänster som används

### 1. **AWS IoT Core**
- Hanterar säker MQTT-kommunikation
- Autentiserar ESP32 med X.509-certifikat
- Dirigerar meddelanden med IoT Rules

### 2. **AWS DynamoDB**
- NoSQL-databas som lagrar alla sensoravläsningar
- Lagrar: timestamp, deviceId, distance, bed_status, message

### 3. **AWS Lambda**
- Serverlös funktion som utlöses av IoT Rules
- Skickar Discord-notifikationer när sängen är tom
- Skriven i Python 3.10

### 4. **AWS Amplify**
- Hostar webbdashboarden
- Statisk webbplatsdistribution
- Publik URL för datavisualisering

### 5. **Discord Webhook**
- Tar emot realtidsvarningar
- Notifierar när sängstatus ändras till "tom"

---

## 🔐 Säkerhetsfunktioner

- **MQTTS (MQTT över TLS)** - Krypterad kommunikation
- **X.509-certifikat** - Enhetsautentisering
- **AWS IAM-roller** - Minsta behörighetskontroll
- **Privata nycklar** - Säkert lagrade på ESP32
- **NTP-tidssynkronisering** - Certifikatvalidering

---

## 📊 Dataflöde

1. **ESP32** läser avstånd från HC-SR04-sensor var 5:e sekund
2. **ESP32** bestämmer sängstatus:
   - `avstånd > 30 cm` → "tom"
   - `avstånd ≤ 30 cm` → "upptagen"
   - `ogiltig avläsning` → "fel"
3. **ESP32** publicerar JSON-data till AWS IoT Core via MQTTS
4. **AWS IoT Rule #1** lagrar data i DynamoDB
5. **AWS IoT Rule #2** utlöser Lambda när status = "tom"
6. **Lambda-funktionen** skickar notifikation till Discord
7. **Webbdashboard** visar data från DynamoDB

### Exempel på datapayload

```json
{
  "distance": 92,
  "bed_status": "empty",
  "message": "Sängen är tom!",
  "timestamp": "1768184513277",
  "deviceId": "esp32-baby-monitor-01"
}
 Kodstruktur:

ESP32 Arduino-kod (baby_monitor.ino)
- WiFi-anslutningshantering och NTP-tidssynkronisering.
- HC-SR04-sensoravläsning och MQTTS-klient med certifikat.
- JSON-payload-skapande samt felhantering och filtrering.

Lambda-funktion (lambda_function.py)
  - Händelseparsning från IoT Rule och Discord webhook-integration.
  - Statusfiltrering (endast "empty" utlöser notifikation) samt felhantering.

Webbdashboard (index.html)
  - Responsiv design med realtidsdatavisning.
  - Statusindikatorer med färgkodning och auto-uppdateringsfunktionalitet.

 Installationsinstruktioner:

Förutsättningar:

- AWS-konto och GitHub-konto.
- Arduino IDE med ESP32-boardstöd.
- Discord-server med webhook

Steg 1: Hårdvaruinstallation:
- Anslut HC-SR04 till ESP32 (VCC->5V, GND->GND, TRIG->GPIO 13, ECHO->GPIO 12).
- Anslut ESP32 till dator via USB.

Steg 2: AWS IoT Core-installation:
- Skapa en "Thing" och ladda ner certifikat (Device cert, Private key, Amazon Root CA 1).
- Skapa och bifoga en IoT Policy (iot:Connect, iot:Publish, iot:Subscribe).

Steg 3: ESP32-kodkonfiguration:
- Installera biblioteken WiFiClientSecure och PubSubClient.
- Uppdatera koden med WiFi-uppgifter, AWS Endpoint och certifikat.

Steg 4: DynamoDB-installation:
- Skapa tabellen BabyMonitorData med Partition key timestamp (String) och Sort key deviceId (String).

Steg 5: IoT Rules-installation:
- Regel 1 (DynamoDB): SELECT * FROM 'baby_monitor/bed_status'.
- Regel 2 (Lambda): SELECT *, clientId() as deviceId, timestamp() as timestamp FROM 'baby_monitor/bed_status' WHERE bed_status = 'empty'.

Steg 6: Lambda-funktion:
- Skapa funktion med Python 3.10. Ladda upp deployment package med requests-biblioteket.

Steg 7: Discord Webhook:
- Skapa webhook i Discord och klistra in URL i Lambda-koden.

Steg 8: Amplify Dashboard:
- Skapa Amplify-app och distribuera index.html manuellt via en ZIP-fil.

 Testning:
1. Sensoravläsning: Verifiera korrekta avstånd i Serial Monitor.
2. MQTT: Kontrollera att meddelanden publiceras i AWS MQTT-testklient.
3. Datalagring: Verifiera att data dyker upp i DynamoDB-tabellen.
4. Notifikationer: Simulera "tom" säng och kontrollera Discord.
5. Dashboard: Öppna Amplify-URL och verifiera att data visualiseras.

Felsökning:
- WiFi: Kontrollera att nätverket är 2.4GHz.
- AWS IoT: Verifiera certifikatformatering och att NTP-tidssynk fungerar.
- Discord: Kontrollera Lambda CloudWatch-loggar för SSL- eller importfel.
- Dashboard: Kontrollera webbläsarkonsolen för JavaScript-fel.

Framtida förbättringar:
- Anslut dashboard till live DynamoDB-data via API Gateway.
- Lägg till temperatur- och fuktighetssensorer.
- Implementera maskininlärning för sömnmönsteranalys.

Teknologier:
- Hårdvara: ESP32, C++, HC-SR04.
- Cloud: AWS IoT Core, Lambda, DynamoDB, Amplify.
- Protokoll: MQTT, MQTTS, TLS/SSL, HTTPS.
