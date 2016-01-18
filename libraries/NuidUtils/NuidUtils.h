/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef NuidUtils_h
#define NuidUtils_h

#include "Arduino.h"
#include "EEPROM.h"

class NuidUtils
{
  public:
   NuidUtils();
   void read(byte uid[4], int pos);
   void write(byte uid[4],int pos);
   void remove(int pos);
   void add(byte nuid[4]);
   bool search(byte uidToSearch[4]);
   bool compare(byte nuid1[4], byte nuid2[4]);
   int getLength();
   void writeLength(int len);
   void reset();
   int searchPos(byte uidToSearch[4]);
   void removeByUid(byte nuid[4]);


  private:
    int lenght;
};
#endif