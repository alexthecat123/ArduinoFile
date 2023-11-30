//***********************************************************************************
//* ArduinoFile ProFile Emulator Software v1.2                                      *
//* By: Alex Anderson-McLeod                                                        *
//* Email address: alexelectronicsguy@gmail.com                                     *
//***********************************************************************************

//Try to get the creatiom/mod dates to do something
//Add while loops and make sure that there aren't any places where I assume the Lisa will be ready when I am.
//Maybe switch everything over to a 5V Teensy MCU?

#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include <EEPROM.h>

byte data[532] = {0x54, 0x68, 0x69, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x70, 0x75, 0x74, 0x73, 0x20, 0x61, 0x20, 0x6c, 0x6f, 0x74, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x74, 0x72, 0x65, 0x73, 0x73, 0x20, 0x6f, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x69, 0x6e, 0x67, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x20, 0x62, 0x79, 0x20, 0x73, 0x77, 0x65, 0x65, 0x70, 0x69, 0x6e, 0x67, 0x20, 0x74, 0x68, 0x65, 0x20, 0x68, 0x65, 0x61, 0x64, 0x73, 0x20, 0x61, 0x63, 0x72, 0x6f, 0x73, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6b, 0x20, 0x28, 0x72, 0x65, 0x61, 0x64, 0x69, 0x6e, 0x67, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x20, 0x30, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x6e, 0x20, 0x32, 0x35, 0x46, 0x45, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x6e, 0x20, 0x32, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x6e, 0x20, 0x32, 0x35, 0x46, 0x44, 0x2c, 0x20, 0x65, 0x74, 0x63, 0x29, 0x2e, 0x20, 0x49, 0x74, 0x20, 0x61, 0x6c, 0x73, 0x6f, 0x20, 0x72, 0x65, 0x70, 0x6f, 0x72, 0x74, 0x73, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x73, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x68, 0x65, 0x20, 0x75, 0x73, 0x65, 0x72, 0x2c, 0x20, 0x61, 0x6c, 0x6f, 0x6e, 0x67, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x61, 0x6e, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x70, 0x72, 0x65, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x20, 0x62, 0x79, 0x74, 0x65, 0x73, 0x20, 0x77, 0x68, 0x65, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x6f, 0x63, 0x63, 0x75, 0x72, 0x72, 0x65, 0x64, 0x2e, 0x20, 0x41, 0x6c, 0x74, 0x68, 0x6f, 0x75, 0x67, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6e, 0x6f, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x72, 0x65, 0x61, 0x64, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x70, 0x61, 0x72, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x64, 0x65, 0x74, 0x65, 0x72, 0x6d, 0x69, 0x6e, 0x65, 0x20, 0x64, 0x72, 0x69, 0x76, 0x65, 0x20, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x61, 0x75, 0x74, 0x6f, 0x6d, 0x61, 0x74, 0x69, 0x63, 0x61, 0x6c, 0x6c, 0x79, 0x2c, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x63, 0x61, 0x6e, 0x20, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x22, 0x6e, 0x22, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x20, 0x61, 0x20, 0x63, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x20, 0x64, 0x72, 0x69, 0x76, 0x65, 0x20, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x69, 0x6e, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x73, 0x20, 0x69, 0x66, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x2e, 0x20, 0x41, 0x6c, 0x74, 0x68, 0x6f, 0x75, 0x67, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6e, 0x6f, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x72, 0x65, 0x61, 0x64, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x70, 0x61, 0x72, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x64, 0x65, 0x74, 0x65, 0x72, 0x6d, 0x69, 0x6e, 0x65, 0x20, 0x64, 0x72, 0x69, 0x76, 0x65, 0x20, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x61, 0x75, 0x74, 0x6f, 0x6d, 0x61, 0x74, 0x69, 0x63, 0x61, 0x6c, 0x6c, 0x79, 0x2c, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x63, 0x61, 0x6e, 0x20, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x22, 0x6e, 0x22, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x65, 0x6e, 0x74, 0x65}; //the array that holds the block that's currently being read or written

byte spareTable[48] = {0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x03, 0x98, 0x00, 0x26, 0x00, 0x02, 0x14, 0x20, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x43, 0x61, 0x6D, 0x65, 0x6F, 0x2F, 0x41, 0x70, 0x68, 0x69, 0x64, 0x20, 0x30, 0x30, 0x30, 0x31}; //the array that holds the spare table

char fileName[256];

byte commandBuffer[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //the 6-byte command buffer
byte prevState = 1;
byte currentState = 1; //variables that are used for falling edge detection on the strobe line
int index = 0; //an index used when counting strobe pulses
byte *ogPointer;
uint32_t byteNum;
uint32_t startTime;
uint16_t timeout = 10000; //around 18ms
uint16_t currentTime = 0;
SdFat32 SD;
File32 disk;
File32 scratchFile;
File32 sourceFile;
File32 destFile;
//File32 KVStore;
//File32 KVCache;
FatFile rootDir;
int fileCount = 0;
uint16_t nonce;
uint16_t oldNonce;
unsigned long freeSpace;
uint8_t buf[4096];
uint16_t KVMoniker = 1024;
uint16_t KVAutoboot = 1557;
char extension[255] = ".image";
uint32_t uptime = 0; //if this isn't originally zero, then it resets each time we do the stuff mentioned on the next line down.
//uint32_t pointerTest = &uptime; //why is this needed to keep uptime from resetting each time we use the four bytes before data[] to store status?

//byte KVKey[20];
//byte CorrectKVKey[20] = {0x6D, 0x75, 0x6F, 0x97, 0xCD, 0xDA, 0xDE, 0x35, 0xF2, 0x4E, 0x6B, 0xA5, 0x88, 0x2F, 0x1A, 0x28, 0x34, 0x5B, 0x2D, 0xFA};

const int red = 39;
const int green = 40;
const int blue = 41;

void setup(){
  setLEDColor(1, 0, 0);
  Serial.begin(115200); //start serial communications
  nonce = EEPROM.read(5);
  EEPROM.write(5, nonce + 1);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  SD.begin(); //initialize the SD card
  rootDir.open("/");
  /*if(!KVStore.open("keyvaluestore.db", O_RDWR)){
    Serial.println(F("Key-value store not found! Creating it now..."));
    if(!KVStore.createContiguous("keyvaluestore.db", 34864620)){
      Serial.println(F("Failed to create key-value store! Halting..."));
      while(1);
    }
    Serial.println(F("Done!"));
  }


  Serial.println(F("Wiping key-value cache..."));
  rootDir.remove("keyvaluecache.db");
  if(!KVCache.createContiguous("keyvaluecache.db", 34864620)){
    Serial.println(F("Failed to clear key-value cache! Halting..."));
    while(1);
  }
  Serial.println(F("Done!"));*/

  if(!disk.open("profile.image", O_RDWR)){
    Serial.println(F("Default drive file profile.image not found! Halting..."));
    while(1);
  }

  /*KVCache.seekSet(532*65530);
  KVCache.read(KVKey, 20);
  for(int i = 0; i < 20; i++){
    printDataNoSpace(KVKey[i]);
  }
  Serial.println();
  int correct = 0;
  for(int i = 0; i < 65535; i++){
    Serial.println(i);
    KVCache.seekSet(i * 532);
    KVCache.read(KVKey, 20);
    for(int i = 0; i < 20; i++){
      if(KVKey[i] == CorrectKVKey[i]){
        correct++;
      }
      if(correct == 20){
        break;
      }
      correct = 0;
    }
  }
  Serial.print("Found it at index ");
  Serial.println(KVCache.curPosition());*/
  updateSpareTable();
  setLEDColor(0, 1, 0);
  Serial.println(F("ArduinoFile is ready!"));
  disk.seekSet(0);
  disk.read(data, 532);
  PORTE = PORTE & B11001111; //set the two pins that we're going to use for interrupts to inputs

  EICRB |= B00001010;
  EICRB &= B11111010; //set INT4 and INT5 to trigger on the falling edge
  EIMSK |= (0 << INT4);
  EIMSK |= (0 << INT5); //make sure that both interrupts are disabled initially (we actually never use INT5, but I might use it to speed things up in the future)
  delay(10);
}

void loop() {
  initPins(); //set all pins to their idle states
  cli(); //disable interrupts to keep them from slowing things down
  while(readCMD() == 1); //wait for CMD to go low
  if((PINF & B00010000) == B00000000){
    sei();
    delay(10);
    Serial.println(F("Resetting drive..."));
    delay(10);
    cli();
    return;
  }
  cli();
  sendData(0x01); //send an 0x01 to the host
  //PORTC = PORTC & B11011111;
  setBSY(); //and lower BSY to acknowledge our presence
  //startTime = millis();
  currentTime = 0;
  while((PINC & B00000001) == B00000000){
    currentTime++;
    if(currentTime >= timeout){
      /*sei();
      Serial.println("Timeout: Initial Handshake");
      cli();*/
      return;
    }
  } //wait for the host to raise CMD
  //while(readCMD() == 0);
  DDRF = B00000000; //set the bus into input mode so we can read from it
  //PORTC = PORTC | B00100000;

  currentTime = 0;
  while(PINF != 0x55){ //wait for the host to respond with an 0x55 and timeout if it doesn't
    currentTime++;
    if(currentTime >= timeout){
      sei();
      delay(10);
      Serial.println(F("Phase 1: Host didn't respond with a 55! Maybe the drive was reset?"));
      delay(10);
      cli();
      return;
    }
  }
  //!!!TIMEOUT IF R/W IS HIGH!!!

  //pointer = commandBuffer; //make the pointer point to the command buffer
  byte *pointer = commandBuffer; //make the pointer point to the data array
  unsigned int value;
  ogPointer = pointer;
  setLEDColor(0, 1, 0);
  PORTC = PORTC | B00000010; //if everything checks out, raise BSY
  currentTime = 0;
  byte oldPIN = 0;
  while((PINC & B00000001) == B00000001){ //do this for each of the remaining data bytes and ((PINC | B11111011)) == B11111011
    currentState = PINC & B00001000;
    if(currentState == B00000000 and prevState == B00001000){ //if we're on the falling edge of the strobe, put the next data byte on the bus and increment the pointer
      *pointer++ = PINF;

    }
    /*currentTime++;
    if(currentTime >= timeout){
      sei();
      Serial.println("Timeout: Command Bytes"); //THIS WAS CHANGED
      cli();
      return;
    }*/
    prevState = currentState;
  }

  sei();
  for(int i = 0; i < 6; i++){
    printDataNoSpace(commandBuffer[i]);
  }
  Serial.println();
  delay(1);
  cli();

  if(commandBuffer[0] == 0x00){
    readDrive(); //if the first byte of the command is 0, we need to do a read
  }
  else if(commandBuffer[0] == 0x01 or commandBuffer[0] == 0x02 or commandBuffer[0] == 0x03){
    writeDrive(commandBuffer[0] + 0x02); //if the first byte is a 1, 2, or 3, we need to do a write
  }
  //this emulator treats both writes and write-verifys exactly the same

  else{ //if we get some other command, print an error message and wait for the next handshake from the host
    sei();
    delay(10);
    Serial.println(F("Bad Command!"));
    for(int i = 0; i < 6; i++){
      printDataNoSpace(commandBuffer[i]);
    }
    Serial.println();
    Serial.println(pointer - ogPointer);
    Serial.println();
    sendData(0x55);
    setBSY();
    delay(10);
    cli();
  }
  sei();

  cli(); //if we don't enable and then disable interrupts here, weird behavior results for some reason

}

void readDrive(){
  sendData(0x02);
  setBSY(); //put the read command confirmation of 0x02 on the bus and lower BSY
  currentTime = 0;
  while(readCMD() == 0){
    currentTime++;
    if(currentTime >= timeout){
      /*sei();
      Serial.println("Timeout: Read Command Confirmation");
      cli();*/
      return;
    }
  } //wait for the host to raise CMD
  DDRF = B00000000; //set the bus into input mode
  currentTime = 0;
  while(PINF != 0x55){ //wait for the host to respond with an 0x55 and timeout if it doesn't
    currentTime++;
    if(currentTime >= timeout){
      sei();
      delay(10);
      Serial.println(F("Read phase 2: Host didn't respond with a 55! Maybe the drive was reset?"));
      delay(10);
      cli();
      return;
    }
  }
  byteNum = ((uint32_t)commandBuffer[1] << 16 | (uint32_t)commandBuffer[2] << 8 | (uint32_t)commandBuffer[3]);
  if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFF and commandBuffer[3] == 0xFF){
    for(int i = 0; i < 48; i++){
      data[i] = spareTable[i];
    }
    for(int i = 48; i < 532; i++){
      data[i] = 0xFF;
    }
  }
  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFD){ //emulator status
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    char days[5];
    char hours[3];
    char minutes[3];
    char seconds[3];
    sprintf(days, "%4d", ((uptime%(86400*30))/86400));
    sprintf(hours, "%02d", ((uptime%86400)/3600));
    sprintf(minutes, "%02d", ((uptime%3600)/60));
    sprintf(seconds, "%02d", uptime%60);
    for(int i = 0; i < 4; i++){ //uptime
      data[i] = days[i % 4];
    }
    for(int i = 4; i < 6; i++){ //uptime
      data[i] = hours[i % 2];
    }
    for(int i = 6; i < 8; i++){ //uptime
      data[i] = minutes[i % 2];
    }
    for(int i = 8; i < 10; i++){ //uptime
      data[i] = seconds[i % 2];
    }
    if(nonce != oldNonce){
      freeSpace = SD.vol()->freeClusterCount() * SD.vol()->sectorsPerCluster() * 512;
    }

    uptime += 2;

    oldNonce = nonce;
    char bytesFree[16];
    ultoa(freeSpace, bytesFree, 10);
    int i;
    for(i = 0; i < 15; i++){
      if(bytesFree[i] == 0x00){
        i--;
        break;
      }
    }
    int dataIndex = 24;
    for(int j = i; j >= 0; j--){ //bytes free
      data[dataIndex] = bytesFree[j];
      dataIndex--;
    }
    for(i = 14 - i; i > 0; i--){ //bytes free
      data[dataIndex] = 0x20;
      dataIndex--;
    }
    data[25] = 0x30; //load
    data[26] = 0x00; //load
    data[32] = 0x30; //load
    data[33] = 0x00; //load
    data[39] = 0x30; //load
    data[40] = 0x00; //load
    data[46] = 0x31; //processes
    data[47] = 0x00; //processes
    data[51] = 0x31; //processes
    data[52] = 0x00; //processes
    for(int i = 56; i < 532; i++){ //null terminate for the rest of the block
      data[i] = 0x00;
    }
    cli();
  }
  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFE){ //get file info
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    fileCount = 0;
    rootDir.rewind();
    while(scratchFile.openNext(&rootDir, O_RDONLY)){
      fileName[scratchFile.getName7(fileName, 256)] = 0x00;
      if(not scratchFile.isDir() and fileName[0] != '.' and strstr(fileName, extension) != 0){
        fileCount += 1;
      }
      scratchFile.close();
    }
    for(int i = 0; i < 4; i++){ //nonce
      data[i] = nonce;
    }
    data[4] = fileCount << 8; //num of files
    data[5] = fileCount;

    uint32_t fileSize;
    uint16_t dateModified[50];
    uint16_t timeModified[50];
    rootDir.rewind();
    if(((commandBuffer[4] << 8) + commandBuffer[5]) < fileCount){
      fileCount = 0;
      while(fileCount <= (commandBuffer[4] << 8) + commandBuffer[5]){
        scratchFile.openNext(&rootDir, O_RDONLY);
        fileName[scratchFile.getName7(fileName, 256)] = 0x00;
        if(not scratchFile.isDir() and fileName[0] != '.' and strstr(fileName, extension) != 0){
          fileCount += 1;
          fileSize = scratchFile.fileSize();
        }
        //scratchFile.getModifyDateTime(dateModified, timeModified);
        scratchFile.close();
      }
      for(int i = 6; i < 20; i++){ //last modified time
        //Serial.print(dateModified[i - 6]);
        //Serial.print(" ");
        data[i] = 0x42;
      }
      //Serial.println();

      data[20] = fileSize >> 72;
      data[21] = fileSize >> 64;
      data[22] = fileSize >> 56;
      data[23] = fileSize >> 48;
      data[24] = fileSize >> 40;
      data[25] = fileSize >> 32;
      data[26] = fileSize >> 24;
      data[27] = fileSize >> 16;
      data[28] = fileSize >> 8;
      data[29] = fileSize;

      char sizeString[5];
      if(fileSize == 5175296){
        data[30] = 0x20;
        data[31] = 0x20;
        data[32] = 0x35;
        data[33] = 0x4D;
      }
      else if(fileSize == 10350592){
        data[30] = 0x20;
        data[31] = 0x31;
        data[32] = 0x30;
        data[33] = 0x4D;
      }
      else{
        if(fileSize > 1089003999999){
          data[30] = 0x48;
          data[31] = 0x55;
          data[32] = 0x47;
          data[33] = 0x45;
        }
        else if(fileSize > 1089003999){
          sprintf(sizeString, "%3d", (fileSize/1089004000));
          strlcat(sizeString, "G", sizeof(sizeString));
          data[30] = sizeString[0];
          data[31] = sizeString[1];
          data[32] = sizeString[2];
          data[33] = sizeString[3];
        }
        else if(fileSize > 1089004){
          sprintf(sizeString, "%3d", (fileSize/1089004));
          strlcat(sizeString, "M", sizeof(sizeString));
          data[30] = sizeString[0];
          data[31] = sizeString[1];
          data[32] = sizeString[2];
          data[33] = sizeString[3];
        }
        else if(fileSize > 9999){
          sprintf(sizeString, "%3d", (fileSize/1090));
          strlcat(sizeString, "K", sizeof(sizeString));
          data[30] = sizeString[0];
          data[31] = sizeString[1];
          data[32] = sizeString[2];
          data[33] = sizeString[3];
        }
        else{
          sprintf(sizeString, "%4d", fileSize);
          data[30] = sizeString[0];
          data[31] = sizeString[1];
          data[32] = sizeString[2];
          data[33] = sizeString[3];
        }
      }
      for(int i = 20; data[i] == 0x00; i++){
        data[i] = 0x20;
      }
      int i = 0;
      for(i = 276; fileName[i - 276] != 0x00; i++){ //filename
        data[i] = fileName[i - 276];
      }
      for(; i < 532; i++){
        data[i] = 0x00;
      }
    }
    else{
      for(int i = 0; i < 532; i++){
        data[i] = 0x00;
      }
    }
    delay(10);
    cli();
  }

  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFF){ //retrieve key-value entry from the cache
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    if(commandBuffer[4] == 0x53 and commandBuffer[5] == 0x43){
      Serial.println(F("Loaded moniker/autoboot key-value pair!"));
      for(int i = 0; i < 532; i++){
        data[i] = EEPROM.read(KVMoniker + i);
      }
    }
    else if(commandBuffer[4] == 0x53 and commandBuffer[5] == 0x61){
      Serial.println(F("Loaded autoboot password key-value pair!"));
      for(int i = 0; i < 532; i++){
        data[i] = EEPROM.read(KVAutoboot + i);
      }
    }
    else{
      Serial.println(F("Error: Unsupported key-value load operation!"));
    }
    //Moniker address is 5343 in the cache and 53656C6563746F723A20636F6E666967202020D7 in the store

    //Serial.println(F("Warning: The attempted key-value cache read failed because key-value operations are not currently supported!"));
    /*KVCache.seekSet((commandBuffer[4] << 8) | (commandBuffer[5]));
    Serial.print(F("Reading entry "));
    printDataNoSpace(commandBuffer[4]);
    printDataNoSpace(commandBuffer[5]);
    Serial.println(F(" from the key-value cache..."));
    KVCache.read(data, 532);
    Serial.print(F("Key: "));
    for(int i = 0; i < 20; i++){
      printDataNoSpace(data[i]);
    }
    Serial.println();
    Serial.print(F("Value: "));
    for(int i = 20; i < 532; i++){
      printDataNoSpace(data[i]);
    }
    Serial.println();*/
    delay(1);
    cli();
  }

  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFC){ //selector rescue
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    if(commandBuffer[4] == 0xFF and commandBuffer[5] == 0xFF){ //replace selector with a spare from the rescue folder
      int replacementIndex = 0;
      char buffer[30];
      sprintf(buffer, "profile-backup-%d.image", replacementIndex);
      while(rootDir.exists(buffer)){
        replacementIndex++;
        sprintf(buffer, "profile-backup-%d.image", replacementIndex);
      }
      Serial.print(replacementIndex);
      Serial.println(F(" does not exist! Creating it..."));
      disk.close();
      if(!sourceFile.open("profile.image", O_RDWR)){
        Serial.println(F("Error opening current profile.image!"));
      }
      else if(!sourceFile.rename(buffer)){
        Serial.println(F("Error creating backup of current profile.image!"));
      }
      else if(!sourceFile.close() or !sourceFile.open("/rescue/selector.image", O_RDONLY)){
        Serial.println(F("Error opening backup selector image!"));
      }
      else if(!destFile.createContiguous("profile.image", sourceFile.fileSize())){
        Serial.println(F("Error creating new profile.image!"));
      }
      else{
        size_t n;
        uint32_t index = 0;
        while ((n = sourceFile.read(buf, sizeof(buf))) > 0){
          destFile.write(buf, n);
        }
        nonce = EEPROM.read(5);
        EEPROM.write(5, nonce + 1);
        Serial.println(F("Done backing up old image and restoring new image!"));
      }
      sourceFile.close();
      destFile.close();
      if(!disk.open("profile.image", O_RDWR)){
        Serial.println(F("Failed to reopen profile.image!"));
      }
      updateSpareTable();
    }
    else if((commandBuffer[4] >> 4) == 0x00){ //send a block of a selector ProFile image
      byteNum = ((commandBuffer[4] & B00001111) << 8 | commandBuffer[5]) * 532;
      if(!sourceFile.open("/rescue/selector.image", O_RDONLY)){
        Serial.println(F("Error opening backup Selector ProFile image!"));
      }
      else if((byteNum + 531) >= sourceFile.fileSize()){
        Serial.println(F("Error: Block is past the end of the file!"));
        for(int i = 0; i < 532; i++){
          data[i] = 0x00;
        }
      }
      else{
        disk.seekSet(byteNum);
        sourceFile.read(data, 532);
        Serial.print(F("Successfully read block "));
        printDataNoSpace(commandBuffer[4] & B00001111);
        printDataNoSpace(commandBuffer[5]);
        Serial.println(F(" of backup Selector ProFile image!"));
      }
      sourceFile.close();
    }
    else if((commandBuffer[4] >> 4) == 0x01){ //send a block of a selector 3.5 inch floppy image
      byteNum = ((commandBuffer[4] & B00001111) << 8 | commandBuffer[5]) * 532;
      if(!sourceFile.open("/rescue/selector.3.5inch.dc42", O_RDONLY)){
        Serial.println(F("Error opening backup Selector 3.5 image!"));
      }
      else if((byteNum + 531) >= sourceFile.fileSize()){
        Serial.println(F("Error: Block is past the end of the file!"));
        for(int i = 0; i < 532; i++){
          data[i] = 0x00;
        }
      }
      else{
        disk.seekSet(byteNum);
        sourceFile.read(data, 532);
        Serial.print(F("Successfully read block "));
        printDataNoSpace(commandBuffer[4] & B00001111);
        printDataNoSpace(commandBuffer[5]);
        Serial.println(F(" of backup Selector 3.5 DC42 image!"));
      }
      sourceFile.close();
    }
    else if((commandBuffer[4] >> 4) == 0x02){ //send a block of a selector Twiggy image
      byteNum = ((commandBuffer[4] & B00001111) << 8 | commandBuffer[5]) * 532;
      if(!sourceFile.open("/rescue/selector.twiggy.dc42", O_RDONLY)){
        Serial.println(F("Error opening backup Selector Twiggy image!"));
      }
      else if((byteNum + 531) >= sourceFile.fileSize()){
        Serial.println(F("Error: Block is past the end of the file!"));
        for(int i = 0; i < 532; i++){
          data[i] = 0x00;
        }
      }
      else{
        disk.seekSet(byteNum);
        sourceFile.read(data, 532);
        Serial.print(F("Successfully read block "));
        printDataNoSpace(commandBuffer[4] & B00001111);
        printDataNoSpace(commandBuffer[5]);
        Serial.println(F(" of backup Selector Twiggy DC42 image!"));
      }
      sourceFile.close();
    }
    else{
      Serial.println(F("Bad selector rescue command!"));
      for(int i = 0; i < 532; i++){
        data[i] = 0x00;
      }
    }
    delay(10);
    cli();
  }
  else if(byteNum < disk.fileSize()){
    sei();
    byteNum *= 532;
    disk.seekSet(byteNum);
    disk.read(data, 532);
    delay(1);
    cli();
  }
  else{
    sei();
    Serial.println(F("Error: Requested block is out of range!"));
    for(int i = 0; i < 532; i++){
      data[i] = 0x00;
    }
    delay(1);
    cli();
  }
  DDRF = B11111111; //set the bus into output mode
  PORTF = 0x00; //and put the first status byte on the bus.
  index = 0; //clear out the index
  for(byte *i = data - 4; i < data; i++){
    *i = 0x00;
  }
  clearBSY(); //and raise BSY
  //startTime = millis();
  currentTime = 0;
  byte *pointer = data - 4; //make the pointer point to the data array
  PORTF = *pointer++; //put the first data byte on the bus and increment the value of the pointer
  while((PINC & B00000001) == B00000001){ //do this for each of the remaining data bytes
    currentState = PINC & B00001000;
    if(currentState == B00000000 and prevState == B00001000){ //if we're on the falling edge of the strobe, put the next data byte on the bus and increment the pointer
      PORTF = *pointer++;
    }
    /*currentTime++;
    if(currentTime >= timeout){
      sei();
      Serial.println("Timeout: Read Command Data Bytes");
      cli();
      return;
    }*/
    prevState = currentState;
  }
}

void writeDrive(byte response){
  sendData(response); //send the appropriate response byte (the command byte plus two)
  setBSY(); //and lower BSY
  currentTime = 0;
  while(readCMD() == 0){
    currentTime++;
    if(currentTime >= timeout){
      /*sei();
      Serial.println("Timeout: Write Command Confirmation");
      cli();*/
      return;
    }
  } //wait for the host to raise CMD
  DDRF = B00000000; //set the bus into input mode
  currentTime = 0;
  while(PINF != 0x55){ //wait for the host to respond with an 0x55 and timeout if it doesn't
    currentTime++;
    if(currentTime >= timeout){
      sei();
      delay(10);
      Serial.println(F("Write phase 2: Host didn't respond with a 55! Maybe the drive was reset?"));
      delay(10);
      cli();
      return;
    }
  }
  clearBSY(); //if everything looks good, raise BSY

  byte *pointer = data; //make the pointer point toward our data array
  currentTime = 0;
  while((PINC & B00000001) == B00000001){ //do this for each of the 532 bytes that we're receiving
    currentState = PINC & B00001000;
    if(currentState == B00000000 and prevState == B00001000){ //when we detect a falling edge on the strobe line, read the data bus, save it contents into the data array, and increment the pointer
      *pointer++ = PINF;
    }
    /*currentTime++;
    if(currentTime >= timeout){
      sei();
      Serial.println("Timeout: Write Command Data Bytes");
      cli();
      return;
    }*/
    prevState = currentState;
  }
  currentTime = 0;
  while(readCMD() == 1){
    currentTime++;
    if(currentTime >= timeout){
      /*sei();
      Serial.println("Timeout: Write Command Second Handshake");
      cli();*/
      return;
    }
  } //wait for the host to lower CMD
  sendData(0x06); //send the appropriate response of 0x06
  setBSY(); //and lower BSY
  currentTime = 0;
  while(readCMD() == 0){
    currentTime++;
    if(currentTime >= timeout){
      /*sei();
      Serial.println("Timeout: Write Command Second Handshake Part 2");
      cli();*/
      return;
    }
  } //wait for the host to raise CMD
  DDRF = B00000000; //set the bus into input mode
  currentTime = 0;
  while(PINF != 0x55){ //wait for the host to respond with an 0x55 and timeout if it doesn't
    currentTime++;
    if(currentTime >= timeout){
      sei();
      delay(10);
      Serial.println(F("Write phase 3: Host didn't respond with a 55! Maybe the drive was reset?"));
      delay(10);
      cli();
      return;
    }
  }
  bool halt = false;
  byteNum = ((uint32_t)commandBuffer[1] << 16 | (uint32_t)commandBuffer[2] << 8 | (uint32_t)commandBuffer[3]);
  if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFF and commandBuffer[3] == 0xFD and commandBuffer[4] == 0xFE and commandBuffer[5] == 0xAF){ //built-in command
    sei();
    if(data[0] == 0x48 and data[1] == 0x41 and data[2] == 0x4C and data[3] == 0x54){ //halt
      halt = true;
      Serial.println(F("Halting emulator..."));
      setLEDColor(1, 1, 1);
    }
    if(data[0] == 0x49 and data[1] == 0x4D and data[2] == 0x41 and data[3] == 0x47 and data[4] == 0x45 and data[5] == 0x3A){ //switch image files
      setLEDColor(0, 0, 1); //fix LED issue
      Serial.print(F("Switching to image file "));
      int i = 6;
      while(1){
        if(data[i] == 0x00){
          fileName[i - 6] = 0x00;
          break;
        }
        fileName[i - 6] = data[i];
        Serial.write(data[i]);
        i++;
      }
    }
    Serial.println();
    disk.close();
    if(!disk.open(fileName, O_RDWR)){
      Serial.println(F("Error opening image file!"));
      disk.close();
      if(!disk.open("profile.image", O_RDWR)){
        Serial.println(F("Failed to reopen profile.image!"));
      }
      updateSpareTable();
    }
    else{
      updateSpareTable();
      Serial.println(F("Success!"));
    }
    delay(10);
    cli();
  }
  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFF){
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    if(commandBuffer[4] == 0xFF and commandBuffer[5] == 0xFF);
    else if(commandBuffer[4] == 0x53 and commandBuffer[5] == 0x43){
      Serial.println(F("Wrote to moniker/autoboot key-value pair!"));
      for(int i = 0; i < 532; i++){
        EEPROM.write(KVMoniker + i, data[i]);
      }
    }
    else if(commandBuffer[4] == 0x53 and commandBuffer[5] == 0x61){
      Serial.println(F("Wrote to autoboot password key-value pair!"));
      for(int i = 0; i < 532; i++){
        EEPROM.write(KVAutoboot + i, data[i]);
      }
    }
    else{
      Serial.println(F("Error: Unsupported key-value write operation!"));
    }
    delay(1);
    cli();
  }
  else if(commandBuffer[1] == 0xFF and commandBuffer[2] == 0xFE and commandBuffer[3] == 0xFE){ //FS commands
    setLEDColor(0, 0, 1); //fix LED issue
    sei();
    if(commandBuffer[4] == 0x63 and commandBuffer[5] == 0x70){ //copy
      Serial.print(F("Copying "));
      int i = 0;
      while(1){
        if(data[i] == 0x00){
          fileName[i] = 0x00;
          break;
        }
        fileName[i] = data[i];
        Serial.write(data[i]);
        i++;
      }
      i++;
      if(!sourceFile.open(fileName, O_RDONLY)){
        Serial.println(F("Error opening source file!"));
      }
      else{
        Serial.print(F(" to "));
        int offset = i;
        while(1){
          if(data[i] == 0x00){
            fileName[i - offset] = 0x00;
            break;
          }
          fileName[i - offset] = data[i];
          Serial.write(fileName[i - offset]);
          i++;
        }
        Serial.print(F(" size "));
        Serial.println(sourceFile.fileSize());
        if(!destFile.createContiguous(fileName, sourceFile.fileSize())){
          Serial.println(F("Error creating destination file!"));
        }
        else{
          size_t n;
          uint32_t index = 0;
          while ((n = sourceFile.read(buf, sizeof(buf))) > 0){
            destFile.write(buf, n);
          }
          nonce = EEPROM.read(5);
          EEPROM.write(5, nonce + 1);
          Serial.println(F("Done!"));
        }
      }
      sourceFile.close();
      destFile.close();
    }
    else if(commandBuffer[4] == 0x6D and commandBuffer[5] == 0x6B){ //create new image, normal
      setLEDColor(0, 0, 1); //fix LED issue
      Serial.print(F("Making new 5MB image called "));
      int i = 0;
      while(1){
        if(data[i] == 0x00){
          fileName[i] = 0x00;
          break;
        }
        fileName[i] = data[i];
        i++;
      }
      Serial.println();
      if(!destFile.createContiguous(fileName, 5175296)){
        Serial.println(F("Error creating destination file!"));
      }
      else{
        Serial.println(F("Done!"));
      }
      nonce = EEPROM.read(5);
      EEPROM.write(5, nonce + 1);
      destFile.close();
    }
    else if(commandBuffer[4] == 0x6D and commandBuffer[5] == 0x78){ //create new image, extended
      setLEDColor(0, 0, 1); //fix LED issue
      Serial.print(F("Making new image of size "));
      int i = 0;
      while(1){
        if(data[i] == 0x00){
          fileName[i] = 0x00;
          break;
        }
        fileName[i] = data[i];
        i++;
      }
      uint32_t size = strtol(fileName, NULL, 10);
      Serial.print(size);
      i++;
      int offset = i;
      Serial.print(F(" called "));
      while(1){
        if(data[i] == 0x00){
          fileName[i - offset] = 0x00;
          break;
        }
        fileName[i - offset] = data[i];
        Serial.write(fileName[i - offset]);
        i++;
      }
      Serial.println();
      if(!destFile.createContiguous(fileName, size)){
        Serial.println(F("Error creating destination file!"));
      }
      else{
        Serial.println(F("Done!"));
      }
      nonce = EEPROM.read(5);
      EEPROM.write(5, nonce + 1);
      destFile.close();
    }
    else if(commandBuffer[4] == 0x72 and commandBuffer[5] == 0x6D){ //delete
      setLEDColor(0, 0, 1); //fix LED issue
      int i = 0;
      Serial.print(F("Deleting file "));
      while(1){
        if(data[i] == 0x00){
          fileName[i] = 0x00;
          break;
        }
        fileName[i] = data[i];
        Serial.write(data[i]);
        i++;
      }
      Serial.println();
      if(!rootDir.remove(fileName)){
        Serial.println(F("Failed to delete file!"));
      }
      nonce = EEPROM.read(5);
      EEPROM.write(5, nonce + 1);
    }
    else if(commandBuffer[4] == 0x6D and commandBuffer[5] == 0x76){ //move aka rename
      setLEDColor(0, 0, 1); //fix LED issue
      Serial.print(F("Renaming "));
      int i = 0;
      while(1){
        if(data[i] == 0x00){
          fileName[i] = 0x00;
          break;
        }
        fileName[i] = data[i];
        Serial.write(data[i]);
        i++;
      }
      i++;
      if(!sourceFile.open(fileName, O_RDWR)){
        Serial.println();
        Serial.println(F("Error opening source file!"));
      }
      else{
        Serial.print(" to ");
        int offset = i;
        while(1){
          if(data[i] == 0x00){
            fileName[i - offset] = 0x00;
            break;
          }
          fileName[i - offset] = data[i];
          Serial.write(data[i]);
          i++;
        }
        Serial.println();
        if(!sourceFile.rename(fileName)){
          Serial.println(F("Error renaming file!"));
        }
      }
      sourceFile.close();
      nonce = EEPROM.read(5);
      EEPROM.write(5, nonce + 1);
    }
    else if(commandBuffer[4] == 0x73 and commandBuffer[5] == 0x78){ //sx (set extension)
      setLEDColor(0, 0, 1); //fix LED issue
      int i = 0;
      while(1){
        if(data[i] == 0x00){
          extension[i] = 0x00;
          break;
        }
        extension[i] = data[i];
        i++;
      }
      Serial.println(F("Changing file extension to "));
      Serial.print(extension);
      Serial.println();
      nonce = EEPROM.read(5);
      EEPROM.write(5, nonce + 1);
    }
    delay(10);
    cli();
  }
  else if(byteNum < disk.fileSize()){
    sei();
    byteNum *= 532;
    disk.seekSet(byteNum);
    disk.write(data, 532);
    disk.flush();
    delay(1);
    cli();
  }
  else{
    sei();
    Serial.println(F("Error: Requested block is out of range!"));
    delay(5);
    cli();
  }
  clearBSY(); //if things look good, raise BSY
  DDRF = B11111111; //and set the bus into output mode
  currentTime = 0;
  index = 0; //zero out the index
  PORTF = 0x00; //and put the first status byte on the bus
  while((PINC & B00000001) == B00000001){ //do this for the remaining three status bytes
    currentState = PINC & B00001000;
    if(currentState == B00000000 and prevState == B00001000){ //if we're on the falling edge of the strobe, send the next status byte and increment the index
      PORTF = 0x00;
      index += 1;
    }
    currentTime++;
    if(currentTime >= timeout){
      return;
    }
    prevState = currentState;
  }
  if(halt == true){
    while(1);
  }
}

void initPins(){
  DDRC = B11000010; //set CHK, OCD, and BSY to outputs and the rest of the control signals to inputs
  clearBSY(); //make sure that BSY is raised
  DDRF = B00000000; //set the bus to input mode
  setPCHK();
  setPOCD(); //and lower CHK and OCD (is this really necessary?)
}

void sendData(byte parallelBits){ //makes it more user-friendly to put data on the bus
  DDRF = B11111111; //set the bus to output mode
  PORTF = parallelBits; //and write the parallelBits to the bus
}

byte receiveData(){ //makes it more user-friendly to receive data
  DDRF = B00000000; //set the bus to input mode
  return PINF; //and return whatever's on the bus
}

void updateSpareTable(){
  if(disk.fileSize() == 5175296){
    spareTable[8] = 0x20; //Name
    spareTable[9] = 0x20;
    spareTable[10] = 0x20;
    spareTable[15] = 0x00; //Device Number
    spareTable[16] = 0x03; //ROM Revision
    spareTable[17] = 0x98;
    spareTable[18] = 0x00; //Drive Size
    spareTable[19] = 0x26;
    spareTable[20] = 0x00;
    Serial.println(F("Switching to the 5MB ProFile spare table."));
  }
  else if(disk.fileSize() == 10350592){
    spareTable[8] = 0x31; //Name
    spareTable[9] = 0x30;
    spareTable[10] = 0x4D;
    spareTable[15] = 0x10; //Device Number
    spareTable[16] = 0x04; //ROM Revision
    spareTable[17] = 0x04;
    spareTable[18] = 0x00; //Drive Size
    spareTable[19] = 0x4C;
    spareTable[20] = 0x00;
    Serial.println(F("Switching to the 10MB ProFile spare table."));
  }
  else{
    spareTable[8] = 0x20; //Name
    spareTable[9] = 0x20;
    spareTable[10] = 0x20;
    spareTable[15] = 0x00; //Device Number
    spareTable[16] = 0x03; //ROM Revision
    spareTable[17] = 0x98;
    spareTable[18] = (disk.fileSize()/532) >> 16; //Drive Size
    spareTable[19] = (disk.fileSize()/532) >> 8;
    spareTable[20] = (disk.fileSize()/532);
    Serial.print(F("Switching to a custom spare table for drive of size "));
    printDataNoSpace(spareTable[18]);
    printDataNoSpace(spareTable[19]);
    printDataNoSpace(spareTable[20]);
    Serial.println(F(" blocks."));
  }
}


//all of these functions just make it easier to set, clear, and read the control signals for the drive

void setBSY(){
  setLEDColor(0, 0, 0);
  PORTC = PORTC & B11111101;
}

void clearBSY(){
  setLEDColor(0, 1, 0);
  PORTC = PORTC | B00000010;
}

void setPARITY(){
  PORTC = PORTC & B11011111;
}

void clearPARITY(){
  PORTC = PORTC | B00100000;
}
void setPCHK(){
  PORTC = PORTC & B10111111;
}

void clearPCHK(){
  PORTC = PORTC | B01000000;
}

void setPOCD(){
  PORTC = PORTC & B01111111;
}

void clearPOCD(){
  PORTC = PORTC | B10000000;
}

bool readCMD(){
  return bitRead(PINC, 0);
}

bool readRW(){
  return bitRead(PINC, 2);
}

bool readSTRB(){
  return bitRead(PINC, 3);
}

bool readPRES(){
  return bitRead(PINC, 4);
}

bool readParity(){
  return bitRead(PINC, 5);
}

void setLEDColor(bool r, bool g, bool b){
  digitalWrite(red, r);
  digitalWrite(green, g);
  digitalWrite(blue, b);
}


//the following functions just format hex data for printing over serial and are only used for debugging


void printDataNoSpace(byte data){
  Serial.print(data>>4, HEX);
  Serial.print(data&0x0F, HEX);
}

void printDataSpace(byte data){
  Serial.print(data>>4, HEX);
  Serial.print(data&0x0F, HEX);
  Serial.print(F(" "));
}

void printRawData(){
  Serial.println(F("Raw Data:"));
  Serial.print(F("0000: "));
  for(int i = 0; i <= 531; i++){
    Serial.print(data[i]>>4, HEX);
    Serial.print(data[i]&0x0F, HEX);
    Serial.print(F(" "));
    if((i + 1) % 8 == 0 and (i + 1) % 16 != 0){
      Serial.print(F("  "));
    }
    if((i + 1) % 16 == 0){
      Serial.print(F("        "));
      for(int j = i - 15; j <= i; j++){
        if(data[j] <= 0x1F){
          Serial.print(F("."));
        }
        else{
          Serial.write(data[j]);
        }
      }
      Serial.println();
      if((i + 1) < 0x100){
        Serial.print(F("00"));
      }
      if((i + 1) >= 0x100){
        Serial.print(F("0"));
      }
      Serial.print((i + 1), HEX);
      Serial.print(F(": "));
    }
  }
  Serial.print("                                              ");
  for(int i = 528; i < 532; i++){
    if(data[i] <= 0x1F){
      Serial.print(F("."));
    }
    else{
      Serial.write(data[i]);
    }
  }
  Serial.println();
}
