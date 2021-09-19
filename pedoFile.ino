//***********************************************************************************
//* The ProFile Electronic Data Orchestrator For Invigorating Lethargic Electronics *
//* PedoFile Software v1.0                                                          *
//* By: Alex Anderson-McLeod                                                        *
//* Email address: alexelectronicsguy@gmail.com                                     *
//***********************************************************************************




byte packetNum = 0x01;
bool testMenu = false;
bool confirmOperation = false;
byte notPacketNum = 0xFE;
bool failed = false;
bool repeat = false;
byte SOH = 0x01;
byte STX = 0x02;
byte ACK = 0x06;
byte NAK = 0x15;
byte EOT = 0x04;
byte CAN = 0x18;
byte driveSize[3];
byte padding = 0x1A; //0x1A
byte inData;
byte data[1064];
byte crcArray[1024];
uint16_t backupErrors = 0;
uint32_t currentIndex;
uint32_t writeIndex;
uint32_t dataIndex = 0;
byte lowCRC;
byte highCRC;
byte checksum = 0x00;
uint16_t CRC;
uint16_t actualCRC;
uint32_t currentBlock = 0;
uint32_t totalBlocks = 0;
byte ackStatus = 0x02;
byte rawData[1029];
long int start;
//byte dataStripped[1024];
bool done = false;
//File xModemData;
uint16_t highestBlock = 0;


byte status[5];
//byte data[532];
char input[6];
uint8_t serialBytes[50];
byte address[3];
//byte parity[532];


char *statusMessages[3][8] = {{"Operation Unsuccessful", "This bit is unused", "Timeout Error", "CRC Error", "Seek Error - Cannot Read Header", "Resend Data", "More Than 532 Bytes Sent", "Host Acknowledge Was Not 0x55"},
                          {"This bit is unused", "Seek Error - Wrong Track", "Sparing Occured", "Cannot Read Spare Table", "Bad Block Table Full", "This bit is unused", "Spare Table Full", "Seek Error - Cannot Read Error"},
                          {"This bit is unused", "This bit is unused", "This bit is unused", "This bit is unused", "This bit is unused", "This bit is unused", "Invalid Block Number", "Drive Has Been Reset"}};

char acceptableHex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f'};


byte num = 0x00;


byte read = 0x00;
byte write = 0x01;
byte writeVerify = 0x02;
byte writeForceSparing = 0x03;

byte defaultRetries = 0x64;
byte defaultSpareThreshold = 0x14;

String command;
String inputCommand;
String name;
char userInput;

void setup(){
  Serial.begin(115200);
  Serial.setTimeout(10);
  setParallelDir('IN');

  DDRC = B00011101;
  initPins();
  mainMenu();
}




bool profileRead(byte address0, byte address1, byte address2, byte retryCount=defaultRetries, byte spareThreshold=defaultSpareThreshold){
  bool handshakeSuccessful = profileHandshake();
  byte commandResponse = sendCommandBytes(read, address0, address1, address2, retryCount, spareThreshold);
  readStatusBytes();
  readData();
  if(handshakeSuccessful == 0){
    Serial.println("Handshake failed!");
    return false;
  }
  if(commandResponse != 0x02){
    Serial.println("Command confirmation failed!");
    return false;
  }
  return true;
}


bool profileWrite(byte address0, byte address1, byte address2, byte retryCount=defaultRetries, byte spareThreshold=defaultSpareThreshold){
  bool handshakeSuccessful = profileHandshake();
  byte commandResponse = sendCommandBytes(write, address0, address1, address2, retryCount, spareThreshold);
  writeData();
  readStatusBytes();
  if(handshakeSuccessful == 0){
    Serial.println("Handshake failed!");
    return false;
  }
  if(commandResponse != 0x03){
    Serial.println("Command confirmation failed!");
    return false;
  }
  return true;
}


void mainMenu(){
  clearScreen();
  Serial.println(F("Welcome to the PedoFile!"));
  Serial.println(F("The ProFile Electronic Data Orchestrator For Invigorating Lethargic Electronics"));
  //Serial.println(F("Welcome to the ProFile interface adapter thing!"));
  Serial.println(F("By: Alex Anderson-McLeod"));
  Serial.println();
  Serial.println(F("1 - Reset Drive"));
  Serial.println(F("2 - Read Spare Table"));
  Serial.println(F("3 - Read Block"));
  Serial.println(F("4 - Backup Entire Drive"));
  Serial.println(F("5 - Write Buffer to Block"));
  Serial.println(F("6 - Restore Drive From Backup"));
  Serial.println(F("7 - Write Zeros to Drive"));
  Serial.println(F("8 - Low-Level Format"));
  Serial.println(F("9 - Drive Tests"));
  Serial.println();
  Serial.println(F("Note: All numbers are in hex unless otherwise specified."));
  Serial.print(F("Please select an option: "));
}

void testSubMenu(){
  clearScreen();
  Serial.println(F("Welcome to the PedoFile!"));
  Serial.println(F("The ProFile Electronic Data Orchestrator For Invigorating Lethargic Electronics"));
  //Serial.println(F("Welcome to the ProFile interface adapter thing!"));
  Serial.println(F("By: Alex Anderson-McLeod"));
  Serial.println();
  Serial.println(F("Drive Tests"));
  Serial.println(F("1 - Sequential Read"));
  Serial.println(F("2 - Sequential Write"));
  Serial.println(F("3 - Random Read"));
  Serial.println(F("4 - Random Write"));
  Serial.println(F("5 - Butterfly Read"));
  Serial.println(F("6 - Read-Write-Read"));
  Serial.println(F("7 - Return to Main Menu"));
  Serial.println();
  Serial.println(F("Note: All numbers are in hex unless otherwise specified."));
  Serial.print(F("Please select an option: "));
}

int readDelay = 1;

void clearScreen(){
  Serial.write(27);
  Serial.print(F("[2J"));
  Serial.write(27);
  Serial.print(F("[H"));
}

void flushInput(){
  while(Serial.available()){
    char x = Serial.read();
  }
}


void printStatus(){
  bool statusGood = 1;
  Serial.print(F("Drive Status: "));
  for(int i = 0; i < 4; i++){
    Serial.print(status[i]>>4, HEX);
    Serial.print(status[i]&0x0F, HEX);
    Serial.print(F(" "));
  }
  Serial.println();
  Serial.println();
  Serial.println(F("Status Interpretation:"));
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 8; j++){
      if(bitRead(status[i], j) == 1){
        statusGood = 0;
        Serial.println(statusMessages[i][j]);
      }
    }
  }
  if(statusGood == 1){
    Serial.println(F("Status looks good!"));
  }
  if(status[3] | B00000000 != B00000000){
    Serial.print(F("Number of Retries: "));
    Serial.print(status[3]>>4, HEX);
    Serial.print(status[3]&0x0F, HEX);
    Serial.println();
  }
  Serial.println();
}

void loop() {
  if(Serial.available()){
    command = Serial.readStringUntil("\r");
    command.trim();
    if(command.equals("1") and testMenu == false){
      clearScreen();
      Serial.println(F("Resetting drive..."));
      resetDrive();
      Serial.print(F("Reset successful! Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }
    if(command.equals("2") and testMenu == false){
      clearScreen();
      Serial.println(F("Reading spare table..."));
      Serial.println(F("Command: 00 FF FF FF 64 14"));
      bool readSuccess = profileRead(0xFF, 0xFF, 0xFF);
      if(readSuccess == 0){
        Serial.println();
        Serial.println(F("WARNING: Errors were encountered during the read operation. The following data may be incorrect."));
      }
      /*for(int i = 0; i < 532; i++){
        if(checkParity(data[i], parity[i]) == false){
          Serial.println("The parity was all screwed up!");
          printDataNoSpace(data[i]);
          Serial.println();
        }
      }*/
      Serial.println();
      printRawData();
      Serial.println();
      //printRawParity();
      //Serial.println();
      printStatus();
      Serial.println(F("Data Analysis:"));
      Serial.print(F("Device Name: "));
      for(int i = 0; i < 13; i++){
        Serial.write(data[i]);
      }
      Serial.println();
      Serial.print(F("Device Number: "));
      for(int i = 13; i < 16; i++){
        Serial.print(data[i]>>4, HEX);
        Serial.print(data[i]&0x0F, HEX);
      }
      if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x00){
        Serial.println(F(" (5MB ProFile)"));
      }
      else if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x10){
        Serial.println(F(" (10MB ProFile)"));
      }
      else if(data[13] == 0x00 and data[14] == 0x01 and data[15] == 0x00){
        Serial.println(F(" (10MB Widget)"));
      }
      else{
        Serial.println(F(" (Unknown Drive Type)"));
      }
      Serial.print(F("Firmware Revision: "));
      Serial.print(data[16], HEX);
      Serial.print(F("."));
      Serial.print(data[17]>>4, HEX);
      Serial.println(data[17]&0x0F, HEX);
      Serial.print(F("Total Blocks: "));
      for(int i = 18; i < 21; i++){
        Serial.print(data[i]>>4, HEX);
        Serial.print(data[i]&0x0F, HEX);
      }
      Serial.println();
      Serial.print(F("Bytes Per Block: "));
      for(int i = 21; i < 23; i++){
        Serial.print(data[i]>>4, HEX);
        Serial.print(data[i]&0x0F, HEX);
      }
      Serial.println();
      Serial.print(F("Total Spares: "));
      Serial.print(data[23]>>4, HEX);
      Serial.print(data[23]&0x0F, HEX);
      Serial.println();
      Serial.print(F("Spares Allocated: "));
      Serial.print(data[24]>>4, HEX);
      Serial.print(data[24]&0x0F, HEX);
      Serial.println();
      Serial.print(F("Bad Blocks: "));
      Serial.print(data[25]>>4, HEX);
      Serial.print(data[25]&0x0F, HEX);
      Serial.println();
      Serial.println();
      Serial.print(F("Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }
    if(command.equals("3") and testMenu == false){
      clearScreen();
      Serial.print(F("Please enter the block number that you want to read: "));
      while(1){
        if(readSerialValue(6) == true){
          break;
        }
        else{
          Serial.print(F("Please enter the block number that you want to read: "));
        }
      }
      for(int i = 0; i < 3; i++){
        address[i] = serialBytes[i];
      }
      int retries = defaultRetries;
      int spare = defaultSpareThreshold;
      Serial.print(F("Use the default retry count of "));
      printDataNoSpace(retries);
      Serial.print(F(" (return for yes, 'n' for no)? "));
      while(1){
        if(Serial.available()) {
          delay(50);
          userInput = Serial.read();
          flushInput();
          if(userInput == 'n'){
            Serial.print(F("Please enter the desired retry count: "));
            while(1){
              if(readSerialValue(2) == true){
                retries = serialBytes[0];
                break;
              }
              else{
                Serial.print(F("Please enter the desired retry count: "));
              }
            }
            break;
          }
          else if(userInput == '\r'){
            break;
          }
          else{
            Serial.print(F("Use the default retry count of "));
            printDataNoSpace(retries);
            Serial.print(F(" (return for yes, 'n' for no)? "));
          }
        }
      }
      Serial.print(F("Use the default spare threshold of "));
      printDataNoSpace(spare);
      Serial.print(F(" (return for yes, 'n' for no)? "));
      while(1){
        if(Serial.available()) {
          delay(50);
          userInput = Serial.read();
          flushInput();
          if(userInput == 'n'){
            Serial.print(F("Please enter the desired spare threshold: "));
            while(1){
              if(readSerialValue(2) == true){
                spare = serialBytes[0];
                break;
              }
              else{
                Serial.print(F("Please enter the desired spare threshold: "));
              }
            }
            break;
          }
          else if(userInput == '\r'){
            break;
          }
          else{
            Serial.print(F("Use the default spare threshold of "));
            printDataNoSpace(spare);
            Serial.print(F(" (return for yes, 'n' for no)? "));
          }
        }
      }
      Serial.println();
      Serial.print(F("Reading block "));
      for(int i = 0; i < 3; i++){
        printDataNoSpace(address[i]);
      }
      Serial.println(F("..."));
      Serial.print(F("Command: 00 "));
      for(int i = 0; i < 3; i++){
        printDataSpace(address[i]);
      }
      printDataSpace(retries);
      printDataNoSpace(spare);
      Serial.println();
      Serial.println();
      bool readSuccess = profileRead(address[0], address[1], address[2], retries, spare);
      if(readSuccess == 0){
        Serial.println(F("WARNING: Errors were encountered during the read operation. The following data may be incorrect."));
        Serial.println();
      }
      printRawData();
      Serial.println();
      printStatus();
      Serial.println();
      Serial.print(F("Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }
    if(command.equals("4") and testMenu == false){
      clearScreen();
      backupErrors = 0;
      getDriveType();
      Serial.println();
      Serial.println(F("Start XMODEM receiver now..."));
      delay(2000);
      packetNum = 0x01;
      uint16_t highestBlock = 0;
      notPacketNum = 0xFE;
      uint32_t totalBlocks = 0;
      currentIndex = 0;
      currentBlock = 0;
      totalBlocks = (driveSize[0]<<16 | driveSize[1]<<8 | driveSize[2]);
      driveSize[0] = currentBlock >> 16;
      driveSize[1] = currentBlock >> 8;
      driveSize[2] = currentBlock;
      byte successPacket;
      bool failure = false;
      if(startTransmission() == false){
        failure = true;
      }
      while(failure == false){
        startNewPacket();
        for(int i = 0; i < 1024; i++){
          if(currentIndex % 1064 == 0 and currentBlock != totalBlocks + 2){
            driveSize[0] = currentBlock >> 16;
            driveSize[1] = currentBlock >> 8;
            driveSize[2] = currentBlock;
            readTwoBlocks(driveSize[0], driveSize[1], driveSize[2]);
            currentBlock += 2;
          }
          if(currentIndex % 1064 == 0 and currentBlock == totalBlocks + 2){
            for(int j = 0; j < 1064; j++){
              data[j] = padding;
            }
            currentBlock += 2;
          }
          crcArray[i] = data[currentIndex % 1064];
          Serial.write(data[currentIndex % 1064]);
          currentIndex += 1;
          }
        successPacket = finishPacket();
        while(successPacket == 0x01){
          startNewPacket();
          for(int i = 0; i < 1024; i++){
           Serial.write(crcArray[i]);
          }
          successPacket = finishPacket();
        }
        if(successPacket == 0x00){
          failure = true;
        }
        if(currentBlock >= totalBlocks + 2){
          break;
        }
      }
        if(failure == false){
          finishTransmission();
          delay(2000);
          if(backupErrors != 0){
            Serial.print(F("Warning: "));
            Serial.print(backupErrors);
            Serial.print(F(" disk errors were encountered during the operation!"));
            Serial.println();
          }
          Serial.println();
          Serial.println(F("XMODEM transfer complete!"));
        }
        else{
          Serial.println();
          Serial.println(F("Transfer timed out!"));
        }
        while(Serial.available()){
          byte data = Serial.read();
        }
        Serial.print(F("Press return to continue..."));
        flushInput();
        while(!Serial.available());
        mainMenu();
        flushInput();
      //}
    }
    if(command.equals("5") and testMenu == false){
      clearScreen();
      Serial.print(F("Please enter the block number that you want to write to: "));
      while(1){
        if(readSerialValue(6) == true){
          break;
        }
        else{
          Serial.print(F("Please enter the block number that you want to write to: "));
        }
      }
      for(int i = 0; i < 3; i++){
        address[i] = serialBytes[i];
      }
      Serial.println();
      Serial.print(F("Writing the buffer to block "));
      for(int i = 0; i < 3; i++){
        printDataNoSpace(address[i]);
      }
      Serial.println(F("..."));
      Serial.print(F("Command: 01 "));
      for(int i = 0; i < 3; i++){
        printDataSpace(address[i]);
      }
      printDataSpace(defaultRetries);
      printDataNoSpace(defaultSpareThreshold);
      Serial.println();
      Serial.println();
      bool writeSuccess = profileWrite(address[0], address[1], address[2]);
      if(writeSuccess == 0){
        Serial.println(F("WARNING: Errors were encountered during the write operation. The data may have been written incorrectly."));
        Serial.println();
      }
      printStatus();
      Serial.println();
      Serial.print(F("Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }

    if(command.equals("6") and testMenu == false){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      backupErrors = 0;
      failed = false;
      packetNum = 0x01;
      notPacketNum = 0xFE;
      currentIndex = 0;
      dataIndex = 0;
      writeIndex = 0;
      actualCRC = 0;
      CRC = 0;
      currentBlock = 0;
      uint32_t totalBlocks = 0;
      done = false;
      byte writeData[1064]; //THIS IS DATA
      ackStatus = 0x02;
      failed = false;
      if(confirmOperation == true){
        getDriveType();

        Serial.println();
        /*Serial.print("Initializing SD card...");
        if (!SD.begin(53)) {
          Serial.println("initialization failed!");
          while (1);
        }
        Serial.println("initialization done.");
        xModemData = SD.open("rx5.bin", FILE_WRITE);*/

        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        Serial.println(F("Start XMODEM sender when ready..."));
        while(1){
          for(writeIndex = 0; writeIndex < 1064; writeIndex++){
            if(currentIndex % 1024 == 0){
              receivePacket();
            }
            if(done == true){
              delay(2000);
              break;
            }
            data[writeIndex] = crcArray[currentIndex % 1024];
            currentIndex += 1;
          }
          if(done == true){
            break;
          }
          /*for(int i = 0; i < 1064; i++){
            xModemData.write(data[i]);
          }*/
          driveSize[0] = currentBlock >> 16;
          driveSize[1] = currentBlock >> 8;
          driveSize[2] = currentBlock;
          writeTwoBlocks(driveSize[2], driveSize[1], driveSize[0]);
          currentBlock += 2;
        }
        flushInput();
        delay(500);
        flushInput();
        if((currentIndex * 2) / 1064 != highestBlock and failed == false){
          Serial.print(F("File Size Mismatch: Drive size is "));
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
          Serial.print(F(" blocks, but received file was "));
            printDataNoSpace(((currentIndex * 2) / 1064) >> 16);
            printDataNoSpace(((currentIndex * 2) / 1064) >> 8);
            printDataNoSpace(((currentIndex * 2) / 1064));
          Serial.println(F(" blocks!"));
        }
        Serial.println();
        if(backupErrors != 0){
          Serial.print(F("Warning: "));
          Serial.print(backupErrors);
          Serial.print(F(" disk errors were encountered during the operation!"));
          Serial.println();
        }
        if(failed == false){
          Serial.println(F("Drive restore succeeded!"));
        }
        else{
          Serial.println(F("Drive restore failed!"));
        }
      }

      Serial.print(F("Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }



    if(command.equals("7") and testMenu == false){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      if(confirmOperation == true){
        getDriveType();
        Serial.println();

        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        for(long int i = 0; i < highestBlock; i++){
          for(int j = 0; j < 532; j++){
            data[j] = 0x00;
          }
          driveSize[0] = i >> 16;
          driveSize[1] = i >> 8;
          driveSize[2] = i;
          profileWrite(driveSize[0], driveSize[1], driveSize[2]);
          Serial.print(F("Now writing zeros to block "));
          for(int j = 0; j < 3; j++){
            printDataNoSpace(driveSize[j]);
          }
          Serial.print(" of ");
          printDataNoSpace((highestBlock - 1) >> 16);
          printDataNoSpace((highestBlock - 1) >> 8);
          printDataNoSpace(highestBlock - 1);
          Serial.print(F(". Progress: "));
          Serial.print(((float)i/(highestBlock - 1))*100);
          Serial.print(F("%"));
          Serial.write("\033[1000D");
          if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
            Serial.println();
            Serial.println();
            Serial.print(F("Error writing block "));
            printDataNoSpace(i >> 16);
            printDataNoSpace(i >> 8);
            printDataNoSpace(i);
            Serial.println(F("!"));
            Serial.println(F("Status Interpretation:"));
            for(int k = 0; k < 3; k++){
              for(int l = 0; l < 8; l++){
                if(bitRead(status[k], l) == 1){
                  //statusGood = 0;
                  Serial.println(statusMessages[k][l]); //All status errors show up every time
                }
              }
            }
            Serial.println();
          }


        }
      }

      Serial.println();
      Serial.print(F("Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }
    if(command.equals("8") and testMenu == false){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      if(confirmOperation == true){
        Serial.println(F("Reading spare table to determine drive size..."));
        Serial.println(F("Command: 00 FF FF FF 64 14"));
        //profileRead(0xFF, 0xFF, 0xFF);
        data[13] = 0x00;
        data[14] = 0x00;
        data[15] = 0x00;
        if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x00){
          Serial.print(F("Drive is a 5MB ProFile with "));
        }
        else if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x10){
          Serial.print(F("Drive is a 10MB ProFile with "));
        }
        else if(data[13] == 0x00 and data[14] == 0x01 and data[15] == 0x00){
          Serial.print(F("Drive is a 10MB Widget with "));
        }
        else{
          Serial.print(F("Drive type is unknown with "));
        }
        //byte driveSize[3];
        for(int i = 18; i < 21; i++){
          Serial.print(data[i]>>4, HEX);
          Serial.print(data[i]&0x0F, HEX);
          driveSize[i - 18] = data[i];
        }

        bool selectedSize = false;
        Serial.print(F(" blocks. Is this correct (return for yes, 'n' for no)? "));
        flushInput();
        while(1){
          if(Serial.available()){
            delay(50);
            userInput = Serial.read();
            flushInput();
            if(userInput == 'n'){
              Serial.println();
              Serial.println(F("1 - 5MB ProFile"));
              Serial.println(F("2 - 10MB ProFile"));
              Serial.println(F("3 - 10MB Widget"));
              Serial.print(F("Please select your drive from this list: "));
              while(selectedSize == false){
                if(Serial.available()){
                  delay(50);
                  userInput = Serial.read();
                  flushInput();
                  if(userInput == '1'){
                    selectedSize = true;
                    driveSize[0] = 0x00;
                    driveSize[1] = 0x26;
                    driveSize[2] = 0x00;
                  }
                  else if(userInput == '2'){
                    selectedSize = true;
                    driveSize[0] = 0x00;
                    driveSize[1] = 0x4C;
                    driveSize[2] = 0x00;
                  }
                  else if(userInput == '3'){
                    selectedSize = true;
                    driveSize[0] = 0x00;
                    driveSize[1] = 0x4C;
                    driveSize[2] = 0x00;
                  }
                  else{
                    Serial.println();
                    Serial.println(F("1 - 5MB ProFile"));
                    Serial.println(F("2 - 10MB ProFile"));
                    Serial.println(F("3 - 10MB Widget"));
                    Serial.print(F("Please select your drive from this list: "));
                  }
                }
              }
              break;
            }

            else if(userInput == '\r'){
              break;
            }
            else{
              if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x00){
                Serial.print(F("Drive is a 5MB ProFile with "));
              }
              else if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x10){
                Serial.print(F("Drive is a 10MB ProFile with "));
              }
              else if(data[13] == 0x00 and data[14] == 0x01 and data[15] == 0x00){
                Serial.print(F(" Drive is a 10MB Widget with "));
              }
              else{
                Serial.print(F(" (Unknown Drive Type)"));
              }
              for(int i = 18; i < 21; i++){
                Serial.print(data[i]>>4, HEX);
                Serial.print(data[i]&0x0F, HEX);
                driveSize[i - 18] = data[i];
              }
              Serial.print(F(" blocks. Is this correct (return for yes, 'n' for no)? "));
            }
          }

          }
        }

        Serial.println();

        driveSize[0] = 0x00;
        driveSize[1] = 0x26;
        driveSize[2] = 0x00;

        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        if(highestBlock != 0x2600 and confirmOperation == true){
          confirmOperation = false;
          Serial.println(F("This device only currently supports low-level formatting 5MB ProFiles."));
        }
        if(confirmOperation == true){
          Serial.print(F("Install jumper and press return to continue..."));
          flushInput();
          while(!Serial.available());
          flushInput();
          Serial.println(F("Low-level formatting drive..."));
          profileHandshake();
          sendCommandBytes(0x03, 0x00, 0x00, 0x00, defaultRetries, defaultSpareThreshold);
          //clearCMD();
          while(readBsy() != 1);
          Serial.print(F("Remove jumper and press return to continue..."));
          flushInput();
          while(!Serial.available());
          flushInput();
          Serial.println(F("Scanning drive for bad blocks..."));
          profileHandshake();
          sendCommandBytes(0x04, 0x00, 0x00, 0x00, defaultRetries, defaultSpareThreshold);
          //clearCMD();
          while(readBsy() != 1);
          Serial.println(F("Creating spare table..."));
          profileHandshake();
          sendCommandBytes(0x05, 0x00, 0x00, 0x00, defaultRetries, defaultSpareThreshold);
          //clearCMD();
          while(readBsy() != 1);
          Serial.print(F("Low-level format successful! "));
        }
        Serial.print(F("Press return to continue..."));
        flushInput();
        while(!Serial.available());
        mainMenu();
        flushInput();
      }
    if(command.equals("9") and testMenu == false){
      testMenu = true;
      clearScreen();
      testSubMenu();
      flushInput();
    }
    if(command.equals("7") and testMenu == true){
      testMenu = false;
      clearScreen();
      mainMenu();
      flushInput();
    }
    if(command.equals("1") and testMenu == true){
      clearScreen();
      getDriveType();
      repeatTest();
      uint16_t highestBlock = 0;
      bool firstTime = true;
      long int passes = 1;
      Serial.println();
      highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
      while(repeat == true or firstTime == true){
        for(long int i = 0; i < highestBlock; i++){
          driveSize[0] = i >> 16;
          driveSize[1] = i >> 8;
          driveSize[2] = i;
          profileRead(driveSize[0], driveSize[1], driveSize[2]);
          Serial.print(F("Now reading block "));
          for(int j = 0; j < 3; j++){
            printDataNoSpace(driveSize[j]);
          }
          Serial.print(" of ");
          printDataNoSpace((highestBlock - 1) >> 16);
          printDataNoSpace((highestBlock - 1) >> 8);
          printDataNoSpace(highestBlock - 1);
          Serial.print(F(". Progress: "));
          Serial.print(((float)i/(highestBlock - 1))*100);
          Serial.print(F("%"));
          if(repeat == true){
            Serial.print(" - Pass ");
            Serial.print(passes);
          }
          Serial.write("\033[1000D");
          if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
            Serial.println();
            Serial.println();
            Serial.print(F("Error reading block "));
            printDataNoSpace(i >> 16);
            printDataNoSpace(i >> 8);
            printDataNoSpace(i);
            Serial.println(F("!"));
            Serial.println(F("Status Interpretation:"));
            for(int k = 0; k < 3; k++){
              for(int l = 0; l < 8; l++){
                if(bitRead(status[k], l) == 1){
                  //statusGood = 0;
                  Serial.println(statusMessages[k][l]);
                }
              }
            }
            Serial.println();
          }
        }
        firstTime = false;
        passes += 1;
      }


      Serial.println();
      Serial.print(F("Read test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }



    if(command.equals("6") and testMenu == true){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      if(confirmOperation == true){
        getDriveType();
        repeatTest();
        bool firstTime = true;
        long int passes = 1;
        Serial.println();
        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        while(repeat == true or firstTime == true){
          for(long int i = 0; i < highestBlock; i++){
            driveSize[0] = i >> 16;
            driveSize[1] = i >> 8;
            driveSize[2] = i;
            profileRead(driveSize[0], driveSize[1], driveSize[2]);
            Serial.print(F("Phase 1 - Now reading block "));
            for(int j = 0; j < 3; j++){
              printDataNoSpace(driveSize[j]);
            }
            Serial.print(" of ");
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
            Serial.print(F(". Progress: "));
            Serial.print(((float)i/(highestBlock - 1))*100);
            Serial.print(F("%"));
            if(repeat == true){
              Serial.print(" - Pass ");
              Serial.print(passes);
            }
            Serial.write("\033[1000D");
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error reading block "));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
          }
          for(int i = 0; i < 532; i+=3){
            data[i] = 0x55;
            crcArray[i] = 0x55;
          }
          for(int i = 1; i < 532; i+=3){
            data[i] = 0x5A;
            crcArray[i] = 0x5A;
          }
          for(int i = 2; i < 532; i+=3){
            data[i] = 0xAA;
            crcArray[i] = 0xAA;
          }
          data[530] = 0x70;
          data[531] = 0x75;
          crcArray[530] = 0x70;
          crcArray[531] = 0x75;
          Serial.println();
          for(long int i = 0; i < highestBlock; i++){
            driveSize[0] = i >> 16;
            driveSize[1] = i >> 8;
            driveSize[2] = i;
            profileWrite(driveSize[0], driveSize[1], driveSize[2]);
            Serial.print(F("Phase 2 - Now writing test pattern to block "));
            for(int j = 0; j < 3; j++){
              printDataNoSpace(driveSize[j]);
            }
            Serial.print(" of ");
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
            Serial.print(F(". Progress: "));
            Serial.print(((float)i/(highestBlock - 1))*100);
            Serial.print(F("%"));
            if(repeat == true){
              Serial.print(" - Pass ");
              Serial.print(passes);
            }
            Serial.write("\033[1000D");
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error writing block "));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
          }
          bool blockError = false;
          Serial.println();
          for(long int i = 0; i < highestBlock; i++){
            driveSize[0] = i >> 16;
            driveSize[1] = i >> 8;
            driveSize[2] = i;
            profileRead(driveSize[0], driveSize[1], driveSize[2]);
            Serial.print(F("Phase 3 - Now rereading block "));
            for(int j = 0; j < 3; j++){
              printDataNoSpace(driveSize[j]);
            }
            Serial.print(" of ");
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
            Serial.print(F(". Progress: "));
            Serial.print(((float)i/(highestBlock - 1))*100);
            Serial.print(F("%"));
            if(repeat == true){
              Serial.print(" - Pass ");
              Serial.print(passes);
            }
            Serial.write("\033[1000D");
            for(int m = 0; m < 532; m++){
              if(data[m] != crcArray[m]){
                blockError = true;
              }
            }
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0 or blockError == true){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              blockError = false;
              Serial.print(F("Error reading block "));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
          }
          firstTime = false;
          passes += 1;
        }
      }



      Serial.println();
      Serial.print(F("Read-Write-Read test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }




    if(command.equals("2") and testMenu == true){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      if(confirmOperation == true){
        getDriveType();
        repeatTest();
        bool firstTime = true;
        long int passes = 1;
        Serial.println();
        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        while(repeat == true or firstTime == true){
          for(long int i = 0; i < highestBlock; i++){
            driveSize[0] = i >> 16;
            driveSize[1] = i >> 8;
            driveSize[2] = i;
            for(int i = 0; i < 532; i++){
              data[i] = B01010101;
            }
            profileWrite(driveSize[0], driveSize[1], driveSize[2]);
            Serial.print(F("Now writing block "));
            for(int j = 0; j < 3; j++){
              printDataNoSpace(driveSize[j]);
            }
            Serial.print(" of ");
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
            Serial.print(F(". Progress: "));
            Serial.print(((float)i/(highestBlock - 1))*100);
            Serial.print(F("%"));
            if(repeat == true){
              Serial.print(" - Pass ");
              Serial.print(passes);
            }
            Serial.write("\033[1000D");
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error writing block with pattern 01010101"));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
            for(int i = 0; i < 532; i++){
              data[i] = B10101010;
            }
            profileWrite(driveSize[0], driveSize[1], driveSize[2]);
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error writing block with pattern 10101010"));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
          }
          firstTime = false;
          passes += 1;
        }
      }



      Serial.println();
      Serial.print(F("Write test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }

    if(command.equals("3") and testMenu == true){
      clearScreen();
      getDriveType();
      repeatTest();
      uint16_t highestBlock = 0;
      bool firstTime = true;
      randomSeed(analogRead(0));
      long int passes = 1;
      uint16_t randomBlock = 0;
      Serial.println();
      highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
      while(repeat == true or firstTime == true){
        for(long int i = 0; i < highestBlock; i++){
          randomBlock = random(0, highestBlock);
          driveSize[0] = randomBlock >> 16;
          driveSize[1] = randomBlock >> 8;
          driveSize[2] = randomBlock;
          profileRead(driveSize[0], driveSize[1], driveSize[2]);
          Serial.print(F("Now reading randomly selected block "));
          for(int j = 0; j < 3; j++){
            printDataNoSpace(driveSize[j]);
          }
          Serial.print(" of ");
          printDataNoSpace((highestBlock - 1) >> 16);
          printDataNoSpace((highestBlock - 1) >> 8);
          printDataNoSpace(highestBlock - 1);
          Serial.print(F(". Progress: "));
          Serial.print(((float)i/(highestBlock - 1))*100);
          Serial.print(F("%"));
          if(repeat == true){
            Serial.print(" - Pass ");
            Serial.print(passes);
          }
          Serial.write("\033[1000D");
          if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
            Serial.println();
            Serial.println();
            Serial.print(F("Error reading block "));
            printDataNoSpace(randomBlock >> 16);
            printDataNoSpace(randomBlock >> 8);
            printDataNoSpace(randomBlock);
            Serial.println(F("!"));
            Serial.println(F("Status Interpretation:"));
            for(int k = 0; k < 3; k++){
              for(int l = 0; l < 8; l++){
                if(bitRead(status[k], l) == 1){
                  //statusGood = 0;
                  Serial.println(statusMessages[k][l]); //All status errors show up every time
                }
              }
            }
            Serial.println();
          }
        }
        firstTime = false;
        passes += 1;
      }


      Serial.println();
      Serial.print(F("Read test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }






    if(command.equals("4") and testMenu == true){
      clearScreen();
      confirm();
      uint16_t highestBlock = 0;
      if(confirmOperation == true){
        getDriveType();
        repeatTest();
        bool firstTime = true;
        randomSeed(analogRead(0));
        long int passes = 1;
        uint16_t randomBlock = 0;
        Serial.println();
        highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
        while(repeat == true or firstTime == true){
          for(long int i = 0; i < highestBlock; i++){
            randomBlock = random(0, highestBlock);
            driveSize[0] = randomBlock >> 16;
            driveSize[1] = randomBlock >> 8;
            driveSize[2] = randomBlock;
            for(int i = 0; i < 532; i++){
              data[i] = B01010101;
            }
            profileWrite(driveSize[0], driveSize[1], driveSize[2]);
            Serial.print(F("Now writing randomly selected block "));
            for(int j = 0; j < 3; j++){
              printDataNoSpace(driveSize[j]);
            }
            Serial.print(" of ");
            printDataNoSpace((highestBlock - 1) >> 16);
            printDataNoSpace((highestBlock - 1) >> 8);
            printDataNoSpace(highestBlock - 1);
            Serial.print(F(". Progress: "));
            Serial.print(((float)i/(highestBlock - 1))*100);
            Serial.print(F("%"));
            if(repeat == true){
              Serial.print(" - Pass ");
              Serial.print(passes);
            }
            Serial.write("\033[1000D");
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error writing block "));
              printDataNoSpace(randomBlock >> 16);
              printDataNoSpace(randomBlock >> 8);
              printDataNoSpace(randomBlock);
              Serial.print(F(" with pattern 01010101"));
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
            for(int i = 0; i < 532; i++){
              data[i] = B10101010;
            }
            profileWrite(driveSize[0], driveSize[1], driveSize[2]);
            if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
              Serial.println();
              Serial.println();
              Serial.print(F("Error writing block "));
              printDataNoSpace(i >> 16);
              printDataNoSpace(i >> 8);
              printDataNoSpace(i);
              Serial.print(F(" with pattern 01010101"));
              Serial.println(F("!"));
              Serial.println(F("Status Interpretation:"));
              for(int k = 0; k < 3; k++){
                for(int l = 0; l < 8; l++){
                  if(bitRead(status[k], l) == 1){
                    //statusGood = 0;
                    Serial.println(statusMessages[k][l]); //All status errors show up every time
                  }
                }
              }
              Serial.println();
            }
          }
          firstTime = false;
          passes += 1;
        }
      }



      Serial.println();
      Serial.print(F("Write test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }


    if(command.equals("5") and testMenu == true){
      clearScreen();
      getDriveType();
      repeatTest();
      uint16_t highestBlock = 0;
      bool firstTime = true;
      long int passes = 1;
      uint16_t butterflyBlock;
      Serial.println();
      highestBlock = (driveSize[0]<<16) | (driveSize[1]<<8) | (driveSize[2]);
      while(repeat == true or firstTime == true){
        for(long int i = 0; i < highestBlock; i++){
          if(i % 2 == 0){
            butterflyBlock = i;
          }
          else{
            butterflyBlock = highestBlock - i;
          }
          driveSize[0] = butterflyBlock >> 16;
          driveSize[1] = butterflyBlock >> 8;
          driveSize[2] = butterflyBlock;
          profileRead(driveSize[0], driveSize[1], driveSize[2]);
          Serial.print(F("Now performing butterfly read test on block "));
          for(int j = 0; j < 3; j++){
            printDataNoSpace(driveSize[j]);
          }
          Serial.print(" of ");
          printDataNoSpace((highestBlock - 1) >> 16);
          printDataNoSpace((highestBlock - 1) >> 8);
          printDataNoSpace(highestBlock - 1);
          Serial.print(F(". Progress: "));
          Serial.print(((float)i/(highestBlock - 1))*100);
          Serial.print(F("%"));
          if(repeat == true){
            Serial.print(" - Pass ");
            Serial.print(passes);
          }
          Serial.write("\033[1000D");
          if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0){ //Make it so that we go back up to the progress line after printing an error
            Serial.println();
            Serial.println();
            Serial.print(F("Error reading block "));
            printDataNoSpace(butterflyBlock >> 16);
            printDataNoSpace(butterflyBlock >> 8);
            printDataNoSpace(butterflyBlock);
            Serial.println(F("!"));
            Serial.println(F("Status Interpretation:"));
            for(int k = 0; k < 3; k++){
              for(int l = 0; l < 8; l++){
                if(bitRead(status[k], l) == 1){
                  //statusGood = 0;
                  Serial.println(statusMessages[k][l]); //All status errors show up every time
                }
              }
            }
            Serial.println();
          }
        }
        firstTime = false;
        passes += 1;
      }


      Serial.println();
      Serial.print(F("Butterfly read test completed. Press return to continue..."));
      flushInput();
      while(!Serial.available());
      mainMenu();
      flushInput();
    }

    else{
      if(testMenu == true){
        testSubMenu();
      }
      else{
        mainMenu();
      }
      //SD.begin(53);
      //File root = SD.open("/");
      //printDirectory(root, 0);

    }
  }
}

void confirm(){
  confirmOperation = false;
  Serial.print(F("WARNING: This operation will destroy all data on your drive! Are you sure you want to continue (return for yes, 'n' to cancel)? "));
  while(1){
    if(Serial.available()){
      delay(50);
      userInput = Serial.read();
      flushInput();
      if(userInput == 'n'){
        break;
        }
      else if(userInput == '\r'){
        confirmOperation = true;
        break;
      }
      else{
        Serial.print(F("WARNING: This operation will destroy all data on your drive! Are you sure you want to continue (return for yes, 'n' to cancel)? "));
        break;
      }
    }

  }
}


void startReceipt(){
  long int originalStart = millis();
  start = millis();
  while(1){
    if(millis() - start >= 3000){
      Serial.write(0x43);
      start = millis();
    }
    if(Serial.available()){
      break;
    }
    if(millis() - originalStart >= 30000){
      Serial.println();
      Serial.println(F("Operation failed - XMODEM sender never started!"));
      done = true;
      failed = true;
      break;
    }
  }
}

void receivePacket(){
  if(ackStatus == 0x00){
    Serial.write(NAK);
  }
  else if(ackStatus == 0x01){
    Serial.write(ACK);
  }
  else if(ackStatus == 0x02){
    startReceipt();
  }
  start = millis();
  while(!Serial.available() and done == false){
    if(millis() - start >= 5000){
      delay(2000);
      Serial.println(F("Operation failed - XMODEM timeout error!"));
      done = true;
      failed = true;
      break;
    }
  }
  dataIndex = 0;
  while(dataIndex < 1029 and done == false){
    if(Serial.available()){
      rawData[dataIndex] = Serial.read();
      dataIndex += 1;
      if(rawData[0] == EOT){
        delay(2000);
        //xModemData.close();
        Serial.write(ACK);
        done = true;
        break;
      }
    }
  }
  if(done == false){
    for(int i = 0; i < 1024; i++){
      crcArray[i] = rawData[i + 3];
    }
    CRC = calc_crc(1024);
    /*delay(10000);
    Serial.println(CRC);*/
    actualCRC = rawData[1027] << 8 | rawData[1028];
    /*Serial.println(actualCRC);
    Serial.println(rawData[1027], BIN);
    Serial.println(rawData[1028], BIN);
    delay(5000);*/

    if(CRC == actualCRC and packetNum == rawData[1] and notPacketNum == rawData[2] and done == false){
      packetNum += 1;
      notPacketNum -= 1;
      //Serial.write(ACK);
      ackStatus = 0x01;
      /*for(int i = 0; i < 1024; i++){
        data[i] = rawData[i + 3];
      }*/
    }
    else if((CRC != actualCRC or packetNum != rawData[1] or notPacketNum != rawData[2]) and done == false){
      //Serial.write(NAK);
      ackStatus = 0x00;
    }
  }
}

void getDriveType(){
  Serial.println(F("Reading spare table to determine drive size..."));
  Serial.println(F("Command: 00 FF FF FF 64 14"));
  profileRead(0xFF, 0xFF, 0xFF);
  if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x00){
    Serial.print(F("Drive is a 5MB ProFile with "));
  }
  else if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x10){
    Serial.print(F("Drive is a 10MB ProFile with "));
  }
  else if(data[13] == 0x00 and data[14] == 0x01 and data[15] == 0x00){
    Serial.print(F("Drive is a 10MB Widget with "));
  }
  else{
    Serial.print(F("Drive type is unknown with "));
  }
  //byte driveSize[3];
  for(int i = 18; i < 21; i++){
    Serial.print(data[i]>>4, HEX);
    Serial.print(data[i]&0x0F, HEX);
    driveSize[i - 18] = data[i];
  }
  Serial.print(F(" blocks. Is this correct (return for yes, 'n' for no)? "));
  while(1){
    if(Serial.available()) {
      delay(50);
      userInput = Serial.read();
      flushInput();
      if(userInput == 'n'){
        Serial.print(F("Please enter the size of your drive in blocks: "));
        while(1){
          if(readSerialValue(6) == true and serialBytes[0] % 2 == 0){
            driveSize[0] = serialBytes[0];
            driveSize[1] = serialBytes[1];
            driveSize[2] = serialBytes[2];
            break;
          }
          else{
            Serial.print(F("Please enter the size of your drive in blocks: "));
          }
        }
        break;
      }
      else if(userInput == '\r'){
        break;
      }
      else{
        if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x00){
          Serial.print(F("Drive is a 5MB ProFile with "));
        }
        else if(data[13] == 0x00 and data[14] == 0x00 and data[15] == 0x10){
          Serial.print(F("Drive is a 10MB ProFile with "));
        }
        else if(data[13] == 0x00 and data[14] == 0x01 and data[15] == 0x00){
          Serial.print(F(" Drive is a 10MB Widget with "));
        }
        else{
          Serial.print(F(" (Unknown Drive Type)"));
        }
        for(int i = 18; i < 21; i++){
          Serial.print(data[i]>>4, HEX);
          Serial.print(data[i]&0x0F, HEX);
          driveSize[i - 18] = data[i];
        }
        Serial.print(F(" blocks. Is this correct (return for yes, 'n' for no)? "));
      }
    }
  }
}

void repeatTest(){
  repeat = false;
  Serial.print(F("Loop the test forever (return for no, 'y' for yes)? "));
  while(1){
    if(Serial.available()){
      delay(50);
      userInput = Serial.read();
      flushInput();
      if(userInput == 'y'){
        repeat = true;
        break;
      }
      else if(userInput == '\r'){
        break;
      }
      else{
        Serial.print(F("Loop the test forever (return for no, 'y' for yes)? "));
      }
    }
  }
}

void writeTwoBlocks(byte address0, byte address1, byte address2){
  bool handshakeSuccessful = profileHandshake();
  byte commandResponse = sendCommandBytes(write, address0, address1, address2, defaultRetries, defaultSpareThreshold);
  if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0 or handshakeSuccessful == 0 or commandResponse != 0x03){
    backupErrors += 1;
  }
  uint32_t fullAddress = address0 << 16 | address1 << 8 | address2;
  fullAddress += 1;
  setRW();
  //delay(1);
  clearCMD();
  //delay(1);
  while(readBsy() != 1){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      break;
    }
  }
  //delay(1);
  for(int i = 0; i < 532; i++){
    sendData(data[i]);
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearRW();
  //delay(1);
  setCMD();
  //delay(1);
  //setSTRB();
  delayMicroseconds(readDelay);
  while(readBsy() != 0 and receiveData != 0x06){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      break;
    }
  }
  setSTRB();
  //delay(1);
  clearSTRB();
  //delay(1);
  setRW();
  //delay(1);
  sendData(0x55);
  //delay(1);
  clearCMD();
  readStatusBytes();
  handshakeSuccessful = profileHandshake();
  address0 = fullAddress >> 16;
  address1 = fullAddress >> 8;
  address2 = fullAddress;
  commandResponse = sendCommandBytes(write, address0, address1, address2, defaultRetries, defaultSpareThreshold);
  if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0 or handshakeSuccessful == 0 or commandResponse != 0x03){
    backupErrors += 1;
  }
  setRW();
  //delay(1);
  clearCMD();
  //delay(1);
  while(readBsy() != 1){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      break;
    }
  }
  //delay(1);
  for(int i = 532; i < 1064; i++){
    sendData(data[i]);
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearRW();
  //delay(1);
  setCMD();
  //delay(1);
  //setSTRB();
  delayMicroseconds(readDelay);
  while(readBsy() != 0 and receiveData != 0x06){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      break;
    }
  }
  setSTRB();
  //delay(1);
  clearSTRB();
  //delay(1);
  setRW();
  //delay(1);
  sendData(0x55);
  //delay(1);
  clearCMD();
  readStatusBytes();
}

void readTwoBlocks(byte address0, byte address1, byte address2){
  bool handshakeSuccessful = profileHandshake();
  byte commandResponse = sendCommandBytes(read, address0, address1, address2, defaultRetries, defaultSpareThreshold);
  readStatusBytes();
  if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0 or handshakeSuccessful == 0 or commandResponse != 0x02){
    backupErrors += 1;
  }
  for(int i = 0; i <= 531; i++){ //Read all 532 bytes from the drive into the data array
    data[i] = receiveData();
    //parity[i] = readParity();
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearCMD();
  delayMicroseconds(readDelay);
  handshakeSuccessful = profileHandshake();
  uint32_t fullAddress = address0 << 16 | address1 << 8 | address2;
  fullAddress += 1;
  commandResponse = sendCommandBytes(read, fullAddress >> 16, fullAddress >> 8, fullAddress, defaultRetries, defaultSpareThreshold);
  readStatusBytes();
  if((status[0] & B11111101) != 0 or (status[1] & B11011110) != 0 or (status[2] & B01000000) != 0 or handshakeSuccessful == 0 or commandResponse != 0x02){
    backupErrors += 1;
  }
  for(int i = 532; i < 1064; i++){ //Read all 532 bytes from the drive into the data array
    data[i] = receiveData();
    //parity[i] = readParity();
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearCMD();
  delayMicroseconds(readDelay);
}


bool startTransmission(){
  start = millis();
  while(!Serial.available()){
    if(millis() - start > 30000){
      return false;
    }
  }
  while(Serial.available()){
    inData = Serial.read();
  }
  if(inData != 0x43){

  }
  return true;
}

void startNewPacket(){
  Serial.write(STX);
  Serial.write(packetNum);
  Serial.write(notPacketNum);
}
byte finishPacket(){
  CRC = calc_crc(1024);
  lowCRC = CRC;
  highCRC = CRC >> 8;
  Serial.write(highCRC);
  Serial.write(lowCRC);
  packetNum += 0x01;
  notPacketNum -= 0x01;
  checksum = 0x00;
  start = millis();
  while(!Serial.available()){
    if(millis() - start > 5000){
      return 0x00;
    }
  }
    while(Serial.available()){
      inData = Serial.read();
      if(inData == NAK){
        packetNum -= 0x01;
        notPacketNum += 0x01;
        return 0x01;
      }
    }
    return 0x02;
}

void finishTransmission(){
  Serial.write(EOT);
}

uint16_t calc_crc(int n){
// Initial value. xmodem uses 0xFFFF but this example
// requires an initial value of zero.
uint16_t x = 0x0000;


for(int i = 0; i < n; i++){
  x = crc_xmodem_update(x, (uint16_t)crcArray[i]);
}
return(x);
}

// See bottom of this page: http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
// Polynomial: x^16 + x^12 + x^5 + 1 (0x1021)
uint16_t crc_xmodem_update (uint16_t crc, uint8_t info)
{
crc = crc ^ ((uint16_t)info << 8);
for (int i=0; i<8; i++) {
  if (crc & 0x8000){
    crc = (crc << 1) ^ 0x1021; //(polynomial = 0x1021)
  }
  else{
    crc <<= 1;
  }
}
return crc;
}


bool readSerialValue(int desiredLength){
  char buffer[1024];
  int length = 0;
  while(1){
    if(Serial.available()){
      char c = Serial.read();
      bool inArray = false;
      for(int i = 0; i < 23; i++){
        if(c == acceptableHex[i]){
          inArray = true;
        }
      }
      if(c == '\r'){
        inArray = true;
      }


      if(inArray == false){
        length = 0;
        delay(50);
        flushInput();
        //Serial.println("false not in array");

        return false;
      }

      if (c == '\r'){
        if(length != desiredLength){
          length = 0;
          flushInput();
          //Serial.println("false wrong length");
          return false;
        }
        buffer[length] = '\0';
        int byte_count = length/2;
        hex2bin(serialBytes, buffer, &byte_count);
        length = 0;
        flushInput();
        return true;
      }
      else if (length < sizeof buffer - 1) {
        buffer[length++] = c;
      }
    }
  }
}


static void hex2bin(uint8_t *out, const char *in, size_t *size)
{
    size_t sz = 0;
    while (*in) {
        while (*in == ' ') in++;  // skip spaces
        if (!*in) break;
        uint8_t c = *in>='a' ? *in-'a'+10 : *in>='A' ? *in-'A'+10 : *in-'0';
        in++;
        c <<= 4;
        if (!*in) break;
        c |= *in>='a' ? *in-'a'+10 : *in>='A' ? *in-'A'+10 : *in-'0';
        in++;
        *out++ = c;
        sz++;
    }
    *size = sz;
}

void resetDrive(){
  setPRES();
  //delay(1);
  clearPRES();
  //delay(1);
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

/*void printRawParity(){
  Serial.println(F("Raw Parity Data:"));
  Serial.print(F("0000: "));
  for(int i = 0; i <= 531; i++){
    Serial.print(parity[i]>>4, HEX);
    Serial.print(parity[i]&0x0F, HEX);
    Serial.print(F(" "));
    if((i + 1) % 8 == 0 and (i + 1) % 16 != 0){
      Serial.print(F(" "));
    }
    if((i + 1) % 16 == 0){
      Serial.print(F("        "));
      for(int j = i - 15; j <= i; j++){
        if(parity[j] <= 0x1F){
          Serial.print(F("."));
        }
        else{
          Serial.write(parity[j]);
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
    if(parity[i] <= 0x1F){
      Serial.print(F("."));
    }
    else{
      Serial.write(parity[i]);
    }
  }
  Serial.println();
}*/


void printDataNoSpace(byte data){
  Serial.print(data>>4, HEX);
  Serial.print(data&0x0F, HEX);
}

void printDataSpace(byte data){
  Serial.print(data>>4, HEX);
  Serial.print(data&0x0F, HEX);
  Serial.print(F(" "));
}

/*PD 7  6  5  4  3 2 1 0
PB 13 12 11 10 9 8*/

//SETTING A BIT ALWAYS MEANS PUTTING IT INTO ITS ACTIVE STATE (MEANING LOW IF IT'S ACTIVE LOW). DO THIS IN THE CONTROL FUNCTIONS
//ADD ALL THE CONTROL LINES INSTEAD OF JUST CMD AND BSY
//READING FROM THE CONTROL BITS
//WHAT SHOULD A FUNCTION DO WHEN IT'S DONE? SHOULD IT LEAVE THE DIRECTION AS WHATEVER IT SET IT TO OR SHOULD IT REVERT IT TO WHAT IT WAS BEFORE?
//SET DIRECTIONS FOR CONTROL LINE R/W. SOME LINES NEED TO BE INPUTS WHILE OTHERS ARE OUTPUTS

void setParallelDir(char* dir){
  if(strcmp(dir, 'IN') == 0){
    DDRL = B00000000;
  }
  else if(strcmp(dir, 'OUT') == 0){
    DDRL = B11111111;
  }
}


void initPins(){
  clearCMD();
  clearRW();
  clearSTRB();
  clearPRES();
  clearPRES();
  setPRES();
  //delay(1);
  clearPRES();
  //delay(1);
}

void sendData(byte parallelBits){
  DDRL = B11111111;
  PORTL = parallelBits;
}

byte receiveData(){
  DDRL = B00000000;
  return PINL;
}


void halt(){
  Serial.println(F("Program halted!"));
  while(1);
}

void setCMD(){
  PORTC = PORTC & B11111110;
}

void clearCMD(){
  PORTC = PORTC | B00000001;
}

void setBSY(){
  DDRC = DDRC | B00000010;
  PORTC = PORTC & B11111101;
}

void clearBSY(){
  DDRC = DDRC | B00000010;
  PORTC = PORTC | B00000010;
}

void setRW(){
  PORTC = PORTC & B11111011;
}

void clearRW(){
  PORTC = PORTC | B00000100;
}

void setSTRB(){
  PORTC = PORTC & B11110111;
}

void clearSTRB(){
  PORTC = PORTC | B00001000;
}
void setPRES(){
  PORTC = PORTC & B11101111;
}

void clearPRES(){
  PORTC = PORTC | B00010000;
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

bool readBsy(){
  DDRC = DDRC & B11111101;
  return bitRead(PINC, 1);
}

bool readParity(){
  DDRC = DDRC & B11011111;
  return bitRead(PINC, 5);
}

bool checkParity(byte data, byte parity){
  byte B_cnt = 0;
  for (; data; data >>= 1) {
    if (data & 1) {
      B_cnt++;
    }
  }
  if(B_cnt % 2 == 1 and parity == 0x00){
    return true;
  }
  else if(B_cnt % 2 == 0 and parity == 0x01){
    return true;
  }
  else{
    return false;
  }
}

bool profileHandshake(){ //Returns true if the handshake succeeds, false if it fails
  bool success = 1;
  setCMD();
  long int startTime = millis();
  while(receiveData() != B00000001 or readBsy() != 0){
    if(millis() - startTime >= 5000){
      //Serial.println(F("Handshake Failed!!!")); //If more than 5 seconds pass and the drive hasn't responded with a $01, halt the program
      success = 0;
      break;
    }
  }
  setSTRB(); //Pulse strobe to tell the drive that we got its $01
  //delay(1);
  clearSTRB();
  //delay(1);
  sendData(0x55); //Respond to the drive's $01 with a $55
  //delay(1);
  setRW(); //Tell the drive that we're writing to it
  //delay(1);
  clearCMD(); //And raise CMD
  //delay(1);
  return success;
}

byte sendCommandBytes(byte byte0, byte byte1, byte byte2, byte byte3, byte byte4, byte byte5){ //Returns the response byte from the profile or 0xFF if the repsonse is invalid
  while(readBsy() != 1){

  }
  byte success;
  setSTRB(); //For some reason, STRB seems to be active high during the command byte phase of the transfer, so set it low to start this phase
  //delay(1);
  sendData(byte0); //Send command byte 0 (command type) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  sendData(byte1); //Send command byte 1 (byte 0 of block number) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  sendData(byte2); //Send command byte 2 (byte 1 of block number) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  sendData(byte3); //Send command byte 3 (byte 2 of block number) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  sendData(byte4); //Send command byte 4 (retry count) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  sendData(byte5); //Send command byte 5 (spare threshold) and pulse the strobe
  //delay(1);
  clearSTRB();
  //delay(1);
  setSTRB();
  //delay(1);
  clearSTRB(); //Now the strobe is back in active-low mode again, so clear it after the command bytes are sent
  //delay(1);
  clearRW(); //We want to read the status bytes from the drive now, so set R/W to read mode
  //delay(1);
  setCMD(); //Lower CMD to tell the drive that we're ready for its confirmation byte
  long int startTime = millis();
  while(1){
    success = receiveData();
    if((success == 0x02 or success == 0x03 or success == 0x04 or success == 0x05 or success == 0x06 or success == 0x07) and readBsy() == 0){
      break;
    }
    if(millis() - startTime >= 5000){
      //Serial.println(F("Command Confirmation Failed!!!")); //If more than 5 seconds pass and the drive hasn't responded with an $02, halt the program
      success = 0xFF;
      break;
    }
  }

  //delay(1);
  setSTRB(); //Acknowledge that we read the $02 confirmation by pulsing the strobe
  success = receiveData();
  //delay(1);
  clearSTRB();
  setRW(); //Tell the drive that we're writing to the bus and respond to its $02 with a $55
  sendData(0x55);
  //delay(1);
  clearCMD();
  return success;
}

void readStatusBytes(){ //Returns the byte array containing the status bytes with a fifth byte at the end that's 0xFF if the operation succeeds and 0x00 if the drive never finishes reading the block
  status[5] = 0xFF;
  clearCMD(); //Raise CMD to tell the drive that it's time to read the desired block
  long int startTime = millis();
  while(readBsy() != 1){ //Wait until the drive finishes reading the block and time out if it doesn't finish within 10 seconds
    if(millis() - startTime >= 10000){
      status[5] = 0x00;
      break;
    }
  }
  clearRW(); //Go into read mode to get the status bytes from the drive
  for(int i = 0; i <= 3; i++){ //Fill all 4 bytes of the status array with the status bytes from the drive, pulsing strobe each time we read a byte from the drive
    //delay(1);
    status[i] = receiveData();
    //delay(1);
    setSTRB();
    //delay(1);
    clearSTRB();
  }
  //delay(1);
}

void readData(){ //Returns the 532-byte array with the data that was read from the drive
  for(int i = 0; i <= 531; i++){ //Read all 532 bytes from the drive into the data array
    data[i] = receiveData();
    //parity[i] = readParity();
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearCMD();
  delayMicroseconds(readDelay);
}

void writeData(){
  setRW();
  //delay(1);
  clearCMD();
  //delay(1);
  while(readBsy() != 1){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      Serial.println(F("Drive Never Said It Was Ready To Receive Data!!!"));
      break;
    }
  }
  //delay(1);
  for(int i = 0; i < 532; i++){
    sendData(data[i]);
    delayMicroseconds(readDelay);
    setSTRB();
    delayMicroseconds(readDelay);
    clearSTRB();
    delayMicroseconds(readDelay);
  }
  clearRW();
  //delay(1);
  setCMD();
  //delay(1);
  //setSTRB();
  delayMicroseconds(readDelay);
  while(readBsy() != 0 and receiveData != 0x06){
    long int startTime = millis();
    if(millis() - startTime >= 10000){
      Serial.println(F("Drive never responded after write operation!"));
      break;
    }
  }
  setSTRB();
  //delay(1);
  clearSTRB();
  //delay(1);
  setRW();
  //delay(1);
  sendData(0x55);
  //delay(1);
  clearCMD();
}

/*ProFile PD0-PD7 are Arduino pins D2-D9
CMD is A0
BSY is A1
R/W is A2
STRB is A3
PRES is A4
Parity is A5
PCHK is A6
POCD is A7 (or you can just ground POCD with the jumper)

PD0-PD7 is Arduino Mega Port L 0-7
CMD, BSY, R/W, STRB, PRES, Parity, PCHK, and POCD are Arduino Mega Port C 0-7, respectively.*/
