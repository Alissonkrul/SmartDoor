/*
  NuidUtils.cpp - Library for read and write
  NUID in the EPROM memory of Arduino.
  Created by Alisson F. Krul, January 18, 2016.
  Released into the public domain.
*/

#include "NuidUtils.h"
#include "Arduino.h"

int _lenght = 0;

  NuidUtils::NuidUtils(){
     _lenght = this->getLength();
  }

  void NuidUtils::add(byte nuid[4]){
   
   if(!search(nuid)){
      int len = getLength();
      int pos = len++;
      this->writeLength(len);
      this->write(nuid,getLength());
      Serial.println("Esse Cartao foi cadastrado!!");
   }else{
      Serial.println("Cartao ja cadastrado");
   }
}

void NuidUtils::write(byte uid[4],int pos){
  int i = 0;
  int addrPos = (pos-1)*4;
  addrPos++;
  for (byte i = 0; i < 4; i++){
      byte bt = uid[i]; //B5
      EEPROM.write((addrPos+i), bt);
   }
}

void NuidUtils::read(byte uid[4], int pos){
   int i = 0;
   int addrPos = (pos-1)*4;
   addrPos++;
   for (byte i = 0; i < 4; i++){
      uid[i] = EEPROM.read(addrPos+i);
   }
}

void NuidUtils::remove(int pos){
   int i = pos;
   int len = this->getLength();
   byte auxNuid[4];
   if(!(i>len||i<1)){
      read(auxNuid,len);
      write(auxNuid,pos);
      byte auxNuid[] = {0x00,0x00,0x00,0x00};
      write(auxNuid,len);
      _lenght--;
      writeLength(_lenght);
   }
   Serial.println("Cartao removido");
}

bool NuidUtils::search(byte uidToSearch[4]){
   int len = this->getLength();
   byte auxUid[4];
   if(len == 0)
      return false;
   for (byte pos = 1; pos <= len; pos++){
      this->read(auxUid,pos);
      if(this->compare(auxUid,uidToSearch)){
         return true;
      }
   }
   return false;
}

int NuidUtils::searchPos(byte uidToSearch[4]){
   int len = this->getLength();
   byte auxUid[4];
   for (byte pos = 1; pos <= len; pos++){
      this->read(auxUid,pos);
      if(this->compare(auxUid,uidToSearch)){
         return pos;
      }
   }
   return 0;
}

void NuidUtils::removeByUid(byte uidtoDelete[4]){

   int pos = this->searchPos(uidtoDelete);
   Serial.println("Posicao deletada");
   Serial.println(pos);
   if(pos>0)
      this->remove(pos);
}



/*
string NuidUtils::print(byte uid[4]) {
 for (byte i = 0; i < 4; i++) {
   string.append((string)(nuid[i] < 0x10 ? " 0" : " ");
      string.append((string)nuid[i]);
   }
}
*/

bool NuidUtils::compare(byte nuid1[4], byte nuid2[4]) {
 for (byte i = 0; i < 4; i++) {
  if(nuid1[i]!=nuid2[i])
   return false;
}
return true;
}

int NuidUtils::getLength(){
   int len = EEPROM.read(0);
   _lenght = len;
   return len;
}

void NuidUtils::writeLength(int len){
   EEPROM.write(0,len);
   this->getLength();
};

void NuidUtils::reset(){
   EEPROM.write(0,0);
};