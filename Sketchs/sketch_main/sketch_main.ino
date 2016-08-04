/******************************************************************************
 *             _____                     _   _____
 *           / ____|                    | | |  __ \
 *          | (___  _ __ ___   __ _ _ __| |_| |  | | ___   ___  _ __
 *           \___ \| '_ ` _ \ / _` | '__| __| |  | |/ _ \ / _ \| '__|
 *           ____) | | | | | | (_| | |  | |_| |__| | (_) | (_) | |
 *          |_____/|_| |_| |_|\__,_|_|   \__|_____/ \___/ \___/|_|
 *
 * VERSION:  1
 *
 * AUTHORS:  Alisson Francisco Krul  <github.com/Alissonkrul>
 *           Bruno Henrique Braga    <github.com/brunofaczz>
 *           Carlos Augusto Grispan  <github.com/cagrispan>
 *           Lucas Ernesto Kindinger <github.com/kindingerlek>
 *
 * COPYRIGHT (c): This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * ABOUT PROJECT:
 *
 * GUIDE:
 *   This algorithm is based in 'Finite state machine', so keep it in your mind
 * when you will upgrade. Every state is a loop function and is represented by
 * name on enum struct, and 'stateName' in function. A state need conditions
 * to chenge to another. To change state, use 'SetState()' function. This
 * function will be execute at once time. If you need some script be execute at
 * once, write on this function.
 *
 */


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
#define     RELAY A3
#define   LCD_SDA A4
#define   LCD_SCL A5

#define  openTime 2

enum State {
  locked,
  denied,
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
void printKey(byte *buffer, int bufferSize);
void setLED(LedColor color);
void setState(State newState);
void loadState();
void stateLocked();
void stateOpen();
void stateProgramMode();

void setup() {
  // Start Comunication
  Serial.begin(9600);
  lcd.begin (16,2);
  SPI.begin();
  rfid.PCD_Init();

  // Clear rfid key memory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // Setup pins
  pinMode(      BTN, INPUT);
  pinMode(    RELAY, OUTPUT);
  pinMode(   BUZZER, OUTPUT);
  pinMode(  LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  setState(locked);

  nuidUtils.reset();
}

void loop() {
  loadState();
}

// Print key
void printKey(byte *buffer, int bufferSize)
{
  Serial.print("[");
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i]);
  }
  Serial.print("]\n");
}

// Define the color of leds
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

/* SetState
 *
 * This function is used for change state. All scripts inside this function
 * will be executed just one time.
 */
 void setState(State newState)
 {
  lastTime = millis();

  switch (newState) {
    case locked:
      Serial.println("Door is LOCKED");

      digitalWrite(RELAY, HIGH);

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("RESTRITO");
      setLED(red);

      digitalWrite(BUZZER,LOW);
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

      digitalWrite(RELAY,LOW);

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("LIBERADO");
      setLED(green);
      break;

    case denied:
      Serial.println("Access Denied!");

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(1, 1);
      lcd.print("NAO PERMITIDO!");

      for(int i = 0; i < 3; i++)
      {
        digitalWrite(BUZZER,HIGH);
        setLED(red);
        delay(200);

        digitalWrite(BUZZER,LOW);
        setLED(none);
        delay(200);
      }
      break;

    case programMode:
      Serial.println("Programming MODE");

      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("MODO");
      lcd.setCursor(5, 1);
      lcd.print("MASTER");
      setLED(yellow);
      break;
  }

  lastTime = millis();
  state = newState;
}

/* Load State
 *
 * This function is very simple, just load the function state. Is called every
 * tick of arduino.
 *
 * DO NOT SCRIPT HERE, STUPID!
 */
 void loadState()
 {
  switch (state) {
    case locked:
      stateLocked();
      break;

    case denied:
      stateDenied();
      break;

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

/* state Locked
 *
 * This fucntion is a implementation of locked state. Bassically, this state
 * will be looking for news RFID card, or WIFI command;
 *
 * IN: programMode, Opened, Denied
 * OUT: programMode, Opened, Locked, Denied
 */
 void stateLocked()
 {
  if ( ! rfid.PICC_IsNewCardPresent())
  return;

  if ( ! rfid.PICC_ReadCardSerial())
  return;

  Serial.print("\nNUID: ");
  printKey(rfid.uid.uidByte,4);;

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
  setState(denied);
}

/* state Denied
 *
 * This state just show lcd message to user for 3 seconds;
 *
 * IN: Locked
 * OUT: Locked
 */
void stateDenied()
{
  if(millis() > lastTime + 3000)
  {
    setState(locked);
    return;
  }
}

/* state Open
 *
 * This fucntion is a implementation of opened state. Its  make a buzzer ON for
 * all time in this state, and verify how long time has been passed after
 * entenring;
 *
 * IN: Locked, ProgramMode
 * OUT: Locked
 */
 void stateOpen()
 {
  digitalWrite(BUZZER,HIGH);
  if(millis() >=  lastTime + (openTime * 1000) )
  {
    setState(locked);
    return;
  }
}

/* state Program Mode
 *
 * This fucntion is a implementation of programMode state. This check if a new
 * rfid has been readed. If does, check if card is masterkey, and open door. If
 * new card isn't master, so add or remove from memory. Else, if time exceed 5
 * seconds, back to locked state.
 *
 * IN: Locked
 * OUT: Opened, Locked
 */
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
    {
      setState(opened);
    }
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
