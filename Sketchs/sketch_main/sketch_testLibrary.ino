// Load Libraries

#include <pt.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <NuidUtils.h>
#include <LiquidCrystal_I2C.h>

// Define Const
#define BT_TX 2 
#define BT_RX 3 
#define WIFI_TX 4 
#define WIFI_RX 5 
#define BTN 8 
#define RFID_RST 9 
#define RFID_SDA 10
#define RFID_MOSI 11
#define RFID_MISO 12
#define RFID_SCK 13
#define LED_RED  A0 
#define LED_GREEN A1 
#define BUZZER A2 
#define RELE A3 
#define LCD_SDA A4 
#define LCD_SCL A5 
 
enum State {
  locked,
  programMode,
  open
}

enum LedColor {
  red,
  green,
  yellow,
  none
}

// Variables
State state = State.locked;
int timeOpen = 10;
signed long lastTime;



//const byte *masterKey[] = {0x7C 0x68 0xC0 0x04};
byte  masterKey[] = {0x7C,0x68,0xC0,0x04};
int programMode = 1;
/*
 * 0- Read Only 
 * 1- Add Card Mode 
 * 2- Remove Card Mode 
 * 3- Working in...
 */

// Initialize the RFID shield
MFRC522 rfid(RFID_SDA, RFID_RST);
MFRC522::MIFARE_Key key; 

NuidUtils nuidUtils;

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

void printHex(byte *buffer, int bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i]);
    }
}

void setup() { 
  //Serial setup
  Serial.begin(9600);

  // Lcd setup
  lcd.begin (16,2);
  
  //RFID setup
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
    for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // LED
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED, OUTPUT);

}

void writeMessage(char message[], LedColor color, bool bip)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(message);

  setLED(color);
}

void setLED(LedColor color)
{
  switch (color) {
      case LedColor.red:
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        break;
      case LedColor.green:
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        break;
      case LedColor.yellow:
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
    case State.locked:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("RESTRITO");
      setLED(LedColor.red);
     break;

    case State.open:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ACESSO");
      lcd.setCursor(4, 1);
      lcd.print("LIBERADO");
      setLED(LedColor.GREEN);

      lastTime = millis();
      break;
     default:
       // do something
  }
  state = newState;
}

void loadState()
{
  switch (state) {
      case State.open:
        stateOpen();
        break;
      case State.programMode:
        stateProgramMode();
        break;
      default:
        stateLocked();

  }
}

void stateLocked()
{
  if(rfid is not present)
    return;

  if(wifi is available AND connected)
  {
    status = wifi.sendData(rfid);

    if(status)
    {
      setState(State.open);
    }
    
    return;
  }
  else
  {
    if(rfid in memory)
    {
      if(refid is master)
      {
        setState(State.ProgramMode);
        return;
      }

      setState(State.open);
      return;
    }
  }
  
  setState(State.locked);
}

void stateOpen()
{
  if(millis() >=  lastTime + (timeOpen * 1000) )
  {
    setState(State.locked);
    return;
  }

}

void stateProgramMode()
{

}

void loop() {  
  loadState();
}
