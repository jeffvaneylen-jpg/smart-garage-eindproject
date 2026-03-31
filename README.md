# IoT Smart Garage System (ESP32 + Blynk)

## Inleiding

Dit project beschrijft een slimme garageoplossing op basis van IoT-technologie.
Een ESP32 wordt gebruikt om hardwarecomponenten aan te sturen en te communiceren met een mobiele applicatie via Blynk.

Het systeem automatiseert toegangscontrole en houdt het aantal vrije parkeerplaatsen bij.

---

## Projectbeschrijving

Het systeem simuleert een parkeergarage waarin:

* voertuigen binnenkomen via RFID-authenticatie
* voertuigen buiten gaan via een drukknop of mobiele app
* het aantal vrije parkeerplaatsen automatisch wordt berekend
* de status lokaal en op afstand wordt weergegeven

### Probleem

Manuele toegangscontrole en capaciteitsbeheer zijn inefficiënt en foutgevoelig.

### Oplossing

Een geautomatiseerd systeem dat:

* toegang valideert via RFID
* parkeerplaatsen beheert
* realtime status weergeeft
* remote bediening mogelijk maakt

---

## Gebruikte technologieën

* ESP32 microcontroller
* RFID (MFRC522)
* Servo motoren
* LCD (I2C)
* Blynk IoT platform
* Arduino C++

Deze technologieën zijn geschikt voor IoT-toepassingen omdat ze lichtgewicht, flexibel en eenvoudig te integreren zijn.

---

## Hardware

* ESP32
* RFID module (MFRC522)
* 2 servo motoren
* 2 LEDs (groen en rood)
* drukknop
* LCD met I2C interface
* jumper wires en breadboard

---

## Pinconfiguratie

### RFID

| Functie | ESP32 |
| ------- | ----- |
| SDA     | 21    |
| SCK     | 18    |
| MOSI    | 23    |
| MISO    | 19    |
| RST     | 22    |
| VCC     | 3.3V  |
| GND     | GND   |

### Servo motoren

| Functie       | ESP32 |
| ------------- | ----- |
| RFID slagboom | 13    |
| Exit slagboom | 25    |

### LEDs

| Functie | ESP32 |
| ------- | ----- |
| Groen   | 4     |
| Rood    | 5     |

### Drukknop

| Functie | ESP32 |
| ------- | ----- |
| Input   | 27    |
| GND     | GND   |

### LCD (I2C)

| Functie | ESP32 |
| ------- | ----- |
| SDA     | 26    |
| SCL     | 14    |
| VCC     | 5V    |
| GND     | GND   |

---

## Blynk configuratie

### Datastreams

| Virtual Pin | Functie               |
| ----------- | --------------------- |
| V0          | Aantal vrije plaatsen |
| V1          | Exit knop             |
| V2          | Status LED            |
| V3          | Status tekst          |
| V4          | Laatste kaart         |

### Widgets

* Label gekoppeld aan V0 voor vrije plaatsen
* Button gekoppeld aan V1 (PUSH mode)
* LED gekoppeld aan V2
* Label gekoppeld aan V3 voor status
* Label gekoppeld aan V4 voor kaartinformatie

---

## Werking van het systeem

### Binnenkomen (RFID)

* RFID kaart wordt gescand
* systeem controleert of de kaart geldig is
* bij geldige kaart:

  * slagboom opent
  * vrije plaatsen verminderen
  * status wordt bijgewerkt

### Buiten gaan

* via drukknop of Blynk app
* slagboom opent
* vrije plaatsen verhogen

### Capaciteit

* wanneer het aantal vrije plaatsen 0 is:

  * systeem toont "VOL"
  * toegang wordt geweigerd

---

## LCD functionaliteit

Het LCD scherm toont:

* opstartstatus
* aantal vrije plaatsen
* toegangsstatus (welkom / geweigerd)
* garage status (VOL)
* exit meldingen

---

## Blynk functionaliteit

De mobiele applicatie biedt:

* realtime weergave van vrije plaatsen
* bediening van de slagboom via knop
* visuele status via LED
* tekstuele feedback
* laatste gescande kaart

---

## Testen en verificatie

Het systeem werd getest met volgende scenario’s:

* geldige RFID kaart geeft toegang
* ongeldige RFID kaart wordt geweigerd
* drukknop opent slagboom
* Blynk knop opent slagboom
* correcte update van vrije plaatsen
* correcte weergave van "VOL"

---

## Mogelijke problemen en oplossingen

| Probleem         | Oplossing                         |
| ---------------- | --------------------------------- |
| LCD toont niets  | contrast aanpassen                |
| LCD werkt niet   | I2C adres wijzigen (0x27 of 0x3F) |
| Blynk werkt niet | controleer wifi en token          |
| Servo werkt niet | controleer voeding (5V)           |


## Uitbreidingsmogelijkheden

* automatische voertuigdetectie met sensoren
* webdashboard voor beheer
* logging van gebruikers
* notificaties bij volle garage
* integratie met camera’s

---

## Conclusie

Dit project toont hoe een ESP32 gecombineerd met sensoren, actuatoren en een mobiele applicatie een volledig functioneel IoT-systeem kan vormen.
De oplossing automatiseert toegangscontrole en biedt zowel lokale als remote monitoring en bediening.
