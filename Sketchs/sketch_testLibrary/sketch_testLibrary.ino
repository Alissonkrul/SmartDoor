#include <NuidUtils.h>
#include <SPI.h>
#include <MFRC522.h>
#include <pt.h>
#include <EEPROM.h>


//const byte *masterKey[] = {0x7C 0x68 0xC0 0x04};
byte  masterKey[] = {0x7C,0x68,0xC0,0x04};
int programMode = 1;
/*
 * 0- Read Only 
 * 1- Add Card Mode 
 * 2- Remove Card Mode 
 * 3- Working in...
 */

 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
NuidUtils nuidUtils;

void printHex(byte *buffer, int bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i]);
    }
}

void setup() {
 
  //Serial setup
  Serial.begin(9600);
  
  //RFID setup
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
    for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }



  // LED
  pinMode(3,OUTPUT);
  pinMode(2,OUTPUT);
  //Limpando os registros
  //nuidUtils.reset();
}
byte lastUid[4];
void loop() {
  // put your main code here, to run repeatedly:

  //if(programMode == 1){
    //Serial.print("Passe o cartão a ser adicionado");
      // Look for new cards
    if ( ! rfid.PICC_IsNewCardPresent())
     return;

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
      return;
      
      
      Serial.println("Um cartao foi lido, passe mais uma vez");
      delay(500);
      Serial.println("para cadastrar ou deletar caso ja existir!!");
      delay(500);
      Serial.print("NUID Lido: ");
      printHex(rfid.uid.uidByte,4);
      Serial.println("");
      if(nuidUtils.compare(lastUid,rfid.uid.uidByte)){
        if(nuidUtils.search(lastUid)){
            nuidUtils.removeByUid(lastUid);
        }else{
            nuidUtils.add(lastUid);
        }
        lastUid[0] = 0x00;
        lastUid[1] = 0x00;
        lastUid[2] = 0x00;
        lastUid[3] = 0x00;
      }else{
        lastUid[0] = rfid.uid.uidByte[0];
        lastUid[1] = rfid.uid.uidByte[1];
        lastUid[2] = rfid.uid.uidByte[2];
        lastUid[3] = rfid.uid.uidByte[3];
      } 
      delay(1000);

}
