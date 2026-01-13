

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
 Kodstruktur
ESP32 Arduino-kod (baby_monitor.ino)
WiFi-anslutningshantering
NTP-tidssynkronisering
HC-SR04-sensoravläsning
MQTTS-klient med certifikat
JSON-payload-skapande
Felhantering och filtrering
Lambda-funktion (lambda_function.py)
Händelseparsning från IoT Rule
Discord webhook-integration
Statusfiltrering (endast "tom" utlöser notifikation)
Felhantering och loggning
Webbdashboard (index.html)
Responsiv design
Realtidsdatavisning
Statusindikatorer med färgkodning
Tabell med senaste avläsningar
Auto-uppdateringsfunktionalitet
🚀 Installationsinstruktioner
Förutsättningar
AWS-konto
Arduino IDE med ESP32-boardstöd
GitHub-konto
Discord-server med webhook
Steg 1: Hårdvaruinstallation
Anslut HC-SR04 till ESP32 enligt hårdvaruanslutningar
Anslut ESP32 till dator via USB
Steg 2: AWS IoT Core-installation
Skapa en Thing i AWS IoT Core
Generera och ladda ner certifikat:
Enhetscertifikat (.pem.crt)
Privat nyckel (.pem.key)
Amazon Root CA 1
Skapa och bifoga en IoT Policy med behörigheter:
iot:Connect
iot:Publish
iot:Subscribe
Notera din AWS IoT Endpoint
Steg 3: ESP32-kodkonfiguration
Installera nödvändiga bibliotek i Arduino IDE:
WiFi.h (inbyggd)
WiFiClientSecure.h (inbyggd)
PubSubClient (av Nick O'Leary)
Uppdatera kod med:
WiFi-uppgifter
AWS IoT Endpoint
Certifikatinnehåll (korrekt formaterat)
Ladda upp kod till ESP32
Steg 4: DynamoDB-installation
Skapa DynamoDB-tabell: BabyMonitorData
Partition key: timestamp (String)
Sort key: deviceId (String)

Steg 5: IoT Rules-installation:
  Regel 1: Spara till DynamoDB:
  SELECT * FROM 'baby_monitor/bed_status'
  Åtgärd: Infoga i DynamoDB-tabell

  Regel 2: Utlös Lambda:
  SELECT *, clientId() as deviceId, timestamp() as timestamp 
  FROM 'baby_monitor/bed_status' 
  WHERE bed_status = 'empty'
  Åtgärd: Anropa Lambda-funktion

Steg 6: Lambda-funktionsinstallation
Skapa Lambda-funktion: BabyMonitorNotificationFunction
Runtime: Python 3.10
Lägg till Discord webhook-URL i koden
Skapa deployment package med requests-bibliotek
Ladda upp och distribuera

Steg 7: Discord Webhook
Skapa webhook i Discord-kanal
Kopiera webhook-URL
Lägg till i Lambda-funktion

Steg 8: Amplify Dashboard
Skapa Amplify-app: BabyMonitorDashboard
Distribuera index.html manuellt
Få åtkomst via tillhandahållen URL

Testning
Test 1: Sensoravläsning
Placera objekt på olika avstånd
Verifiera att Serial Monitor visar korrekta avläsningar
Bekräfta avståndskalkyleringsnoggrannhet

Test 2: MQTT-kommunikation
Kontrollera AWS IoT MQTT-testklient
Verifiera att meddelanden publiceras var 5:e sekund
Bekräfta JSON-payload-struktur

Test 3: Datalagring
Kontrollera DynamoDB-tabell
Verifiera att alla fält är ifyllda
Bekräfta timestamp-noggrannhet

Test 4: Notifikationer
Simulera "tom" säng (avstånd > 30 cm)
Verifiera att Discord-notifikation visas
Kontrollera Lambda CloudWatch-loggar

Test 5: Dashboard
Få åtkomst till Amplify-URL
Verifiera att data visas korrekt
Testa uppdateringsfunktionalitet

Felsökning:

ESP32 ansluter inte till WiFi:
  -Kontrollera WiFi-uppgifter
  -Verifiera att WiFi-nätverket är 2.4GHz (ESP32 stöder inte 5GHz)
  -Kontrollera Serial Monitor för felmeddelanden
ESP32 ansluter inte till AWS IoT
  -Verifiera att certifikat är korrekt formaterade
  -Kontrollera AWS IoT Endpoint-URL
  -Se till att NTP-tidssynkronisering fungerar
  -Verifiera att IoT Policy är bifogad till certifikat
Inga Discord-notifikationer
  -Kontrollera Lambda CloudWatch-loggar för fel
  -Verifiera att Discord webhook-URL är korrekt
  -Se till att IoT Rule utlöser Lambda
  -Bekräfta att bed_status är "empty" i meddelanden
Dashboard visar inte data
  -Kontrollera webbläsarkonsolen för JavaScript-fel
  -Verifiera att Amplify-distributionen lyckades
  -Se till att index.html är korrekt formaterad

📈 Framtida förbättringar
 Anslut dashboard till live DynamoDB-data via API Gateway
 Lägg till temperatur- och fuktighetssensorer
 Implementera kameraflöde för visuell övervakning
 Lägg till mobilapp för iOS/Android
 Skapa historisk dataanalys och trender
 Lägg till stöd för flera sensorer för olika rum
 Implementera maskininlärning för sömnmönsteranalys
 Lägg till röstvarningar via Alexa/Google Home

 Teknologier och färdigheter som demonstreras
Inbyggda system: ESP32, Arduino, C++
IoT-protokoll: MQTT, MQTTS, TLS/SSL
Molntjänster: AWS IoT Core, Lambda, DynamoDB, Amplify
Säkerhet: X.509-certifikat, IAM-roller, kryptering
Programmering: C++ (Arduino), Python (Lambda), JavaScript (Dashboard)
Webbutveckling: HTML, CSS, responsiv design
DevOps: CI/CD med Amplify, serverlös arkitektur
Databas: NoSQL (DynamoDB)
API:er: REST, Discord webhooks
Versionskontroll: Git, GitHub

Rimsha

Student - Mjukvaruutvecklare i IoT och inbyggda system

Nackademin


