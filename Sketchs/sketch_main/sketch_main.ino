// Load Libraries
#include <pt.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <NuidUtils.h>
#include <LiquidCrystal_I2C.h>

// Define Consts
#define      BT_TX 2 
#define      BT_RX 3 
#define    WIFI_TX 4 
#define    WIFI_RX 5 
#define        BTN 8 
#define   RFID_RST 9 
#define  RFID_SDA 10
#define RFID_MOSI 11
#define RFID_MISO 12
#define  RFID_SCK 13
#define LED_GREEN A0
#define   LED_RED A1 
#define    BUZZER A2 
#define      RELE A3 
#define   LCD_SDA A4 
#define   LCD_SCL A5 

#define  openTime 2

enum State {
  locked,
  programMode,
  opened
};

enum LedColor {
  red,
  green,
  yellow,
  none
};

// Variables
State state = locked;
signed long lastTime;
byte  masterKey[] = {0x7C,0x68,0xC0,0x04};
MFRC522::MIFARE_Key key; 
NuidUtils nuidUtils;

// Initializers
MFRC522 rfid(RFID_SDA, RFID_RST);
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3, POSITIVE);

// Prototype Functions
void printHex(byte *buffer, int bufferSize);
void setLED(LedColor color);
void setState(State newState);
void loadState();
void stateLocked();
void stateOpen();
void stateProgramMode();

void setup() { 
  Serial.begin(9600);
  lcd.begin (16,2);
  SPI.begin();

  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  pinMode(BTN, INPUT); 
  pinMode(RELE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED, OUTPUT);

  setState(locked);

  nuidUtils.reset();
}

void loop() {  
  loadState();

  /*if (digitalRead(BTN) == HIGH)
  {
    setState(opened);
    return;
    }*/
  }

  void printHex(byte *buffer, int bufferSize) 
  {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i]);
    }
    Serial.print("\n");
  }

  void setLED(LedColor color)
  {
    switch (color) {
      case red:
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      break;
      case green:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      break;
      case yellow:
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      break;
      default:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
  }

  void setState(State newState)
  {
    switch (newState) {
      case locked:
      Serial.println("Door is LOCKED");
      
      digitalWrite(RELE, HIGH);
      
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("RESTRITO");
      setLED(red);

      digitalWrite(BUZZER,HIGH);
      delay(400);
      digitalWrite(BUZZER,LOW);
      delay(200);
      digitalWrite(BUZZER,HIGH);
      delay(400);
      digitalWrite(BUZZER,LOW);
      break;

      case opened:
      Serial.println("Door is OPEN");

      digitalWrite(RELE,LOW);

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("LIBERADO");
      setLED(green);

      lastTime = millis();
      break;
      default:
      Serial.println("Programming MODE");

      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("MODO");
      lcd.setCursor(5, 1);
      lcd.print("MASTER");
      setLED(yellow);

      lastTime = millis();
    }
    state = newState;
  }

  void loadState()
  {
    switch (state) {
      case opened:
      stateOpen();
      break;
      case programMode:
      stateProgramMode();
      break;
      default:
      stateLocked();

    }
  }

  void stateLocked()
  {
    if ( ! rfid.PICC_IsNewCardPresent())
    return;

    if ( ! rfid.PICC_ReadCardSerial())
    return;

    Serial.print("\nNUID: ");
    printHex(rfid.uid.uidByte,4);;

    if(nuidUtils.compare(masterKey,rfid.uid.uidByte))
    {
      Serial.println("Master key found!");
      setState(programMode);
      return;
    }

    Serial.println("Searching rfid in memory...");
    if(nuidUtils.search(rfid.uid.uidByte))
    {
      Serial.println("RFID key found in memory!");
      setState(opened);
      return;
    }

    Serial.println("RFID not permitted!");
    setState(locked);
  }

  void stateOpen()
  {
    digitalWrite(BUZZER,HIGH);
    if(millis() >=  lastTime + (openTime * 1000) )
    {
      setState(locked);
      return;
    }
  }

  void stateProgramMode()
  {
    if ( ! rfid.PICC_IsNewCardPresent())
    {
      if(millis() > lastTime + 5000 )
        setState(locked);
      return;
    }

    if ( ! rfid.PICC_ReadCardSerial())
      return;

    if(nuidUtils.compare(masterKey,rfid.uid.uidByte))
    {
      if (millis() > lastTime + 500 )
        setState(opened);
      return;
    }

    if( !nuidUtils.search(rfid.uid.uidByte))
    {
      nuidUtils.add(rfid.uid.uidByte);
      setState(opened);
      return;
    }

    nuidUtils.removeByUid(rfid.uid.uidByte);
    setState(locked);
    return;  
  }