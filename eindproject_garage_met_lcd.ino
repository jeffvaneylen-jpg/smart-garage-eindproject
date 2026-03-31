#define BLYNK_TEMPLATE_ID "TMPL512qreZKO"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Gmk2gBF6aAbF3ILSaOpa08Fczic2Pdg8"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- WIFI ----------------
char ssid[] = "TheUrbanJungle";
char pass[] = "hM%m74x_fwLN!E";

// ---------------- RFID ----------------
#define SS_PIN      21
#define RST_PIN     22
#define SCK_PIN     18
#define MISO_PIN    19
#define MOSI_PIN    23

MFRC522 rfid(SS_PIN, RST_PIN);

// ---------------- HARDWARE ----------------
#define GREEN_LED   4
#define RED_LED     5
#define BUTTON_PIN  27

#define SERVO_RFID  13
#define SERVO_BTN   25

Servo servoRFID;
Servo servoButton;

// ---------------- LCD ----------------
// Als 0x27 niet werkt, probeer 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- PARKING ----------------
const int MAX_SPOTS = 5;
int freeSpots = MAX_SPOTS;
bool lastButtonState = HIGH;

// ---------------- TOEGELATEN KAARTEN ----------------
struct AllowedCard {
  byte uid[10];
  byte size;
  const char* name;
};

AllowedCard cards[] = {
  {{0x2E, 0x16, 0xEA, 0x69}, 4, "ROOD"},
  {{0x04, 0x4D, 0xB8, 0x02, 0x37, 0x1D, 0x90}, 7, "WIT"},
  {{0x02, 0x4C, 0x74, 0x81, 0x65, 0xE0, 0x00}, 7, "BLAUW"},
  {{0x93, 0x2F, 0xC6, 0x56}, 4, "GEEL"},
  {{0xB3, 0xFA, 0x7C, 0x14}, 4, "ZWART"}
};

const int cardCount = sizeof(cards) / sizeof(cards[0]);

// ---------------- HULPFUNCTIES ----------------
void ledsOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

void showLCD(const String& line1, const String& line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16));
  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, 16));
}

void showFreeSpots() {
  if (freeSpots <= 0) {
    showLCD("GARAGE STATUS", "VOL");
  } else {
    showLCD("Vrije plaatsen", String(freeSpots));
  }
}

void updateBlynkStatus(const String& statusText, const String& lastCard, int ledValue) {
  Blynk.virtualWrite(V0, freeSpots);
  Blynk.virtualWrite(V2, ledValue);
  Blynk.virtualWrite(V3, statusText);
  Blynk.virtualWrite(V4, lastCard);
}

void printUID(byte *uid, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid[i] < 0x10) Serial.print("0");
    Serial.print(uid[i], HEX);
    if (i < size - 1) Serial.print(" ");
  }
}

const char* checkCard(byte *uid, byte size) {
  for (int i = 0; i < cardCount; i++) {
    if (size != cards[i].size) continue;

    bool match = true;
    for (byte j = 0; j < size; j++) {
      if (uid[j] != cards[i].uid[j]) {
        match = false;
        break;
      }
    }

    if (match) return cards[i].name;
  }
  return NULL;
}

void openServo(Servo &s, const char* label) {
  Serial.print(label);
  Serial.println(" OPEN");
  s.write(90);
  delay(2000);
  s.write(0);
  Serial.print(label);
  Serial.println(" DICHT");
}

// ---------------- ACTIES ----------------
void accessGranted(const char* cardName) {
  Serial.print("TOEGANG OK - ");
  Serial.println(cardName);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  if (freeSpots > 0) {
    freeSpots--;
  }

  openServo(servoRFID, "SERVO RFID");

  showLCD("Welkom", String(cardName));
  updateBlynkStatus(String(freeSpots) + " vrij", String(cardName), 255);

  delay(1500);
  ledsOff();

  showFreeSpots();
  updateBlynkStatus((freeSpots <= 0) ? "VOL" : String(freeSpots) + " vrij", String(cardName), 0);
}

void accessDenied() {
  Serial.println("GEEN TOEGANG");

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  showLCD("GEEN TOEGANG", "");
  updateBlynkStatus("GEEN TOEGANG", "UNKNOWN", 255);

  delay(2000);
  ledsOff();

  showFreeSpots();
  updateBlynkStatus((freeSpots <= 0) ? "VOL" : String(freeSpots) + " vrij", "UNKNOWN", 0);
}

void exitAction(const String& source) {
  Serial.print("EXIT via ");
  Serial.println(source);

  if (freeSpots < MAX_SPOTS) {
    freeSpots++;
  }

  openServo(servoButton, "SERVO KNOP");

  showLCD("Uitrit open", source);
  updateBlynkStatus(String(freeSpots) + " vrij", source, 255);

  delay(1000);

  showFreeSpots();
  updateBlynkStatus((freeSpots <= 0) ? "VOL" : String(freeSpots) + " vrij", source, 0);
}

// ---------------- BLYNK ----------------
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  showFreeSpots();
  updateBlynkStatus((freeSpots <= 0) ? "VOL" : String(freeSpots) + " vrij", "-", 0);
}

BLYNK_WRITE(V1) {
  int state = param.asInt();
  if (state == 1) {
    exitAction("APP");
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ledsOff();

  servoRFID.setPeriodHertz(50);
  servoButton.setPeriodHertz(50);

  servoRFID.attach(SERVO_RFID, 500, 2400);
  servoButton.attach(SERVO_BTN, 500, 2400);

  servoRFID.write(0);
  servoButton.write(0);

  Wire.begin(26, 14);
  lcd.init();
  lcd.backlight();
  showLCD("Garage systeem", "Start...");

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("SYSTEEM KLAAR");
  Serial.println("Scan een kaart of druk op de knop");
  Serial.println("--------------------------------");

  showFreeSpots();
  updateBlynkStatus(String(freeSpots) + " vrij", "-", 0);
}

// ---------------- LOOP ----------------
void loop() {
  Blynk.run();

  // fysieke knop = exit
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    exitAction("BUTTON");
    delay(300);
  }
  lastButtonState = currentButtonState;

  // RFID = entry
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  printUID(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  const char* result = checkCard(rfid.uid.uidByte, rfid.uid.size);

  if (result != NULL) {
    if (freeSpots > 0) {
      accessGranted(result);
    } else {
      Serial.println("GARAGE VOL");
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);

      showLCD("GARAGE STATUS", "VOL");
      updateBlynkStatus("VOL", result, 255);

      delay(1500);
      ledsOff();

      showFreeSpots();
      updateBlynkStatus("VOL", result, 0);
    }
  } else {
    accessDenied();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
}