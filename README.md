# ArduinoFile
An Arduino-based device for troubleshooting/backing up ProFile and Widget hard drives with a modern computer.\
\
![IMG_3571](https://user-images.githubusercontent.com/16897189/133942992-7602075a-a124-4176-a397-23a169452b84.jpeg)

## Warning
The ArduinoFile is a prototype and there are almost certainly bugs in the software, so don't use it in situations where data loss would be bad.\
\
If you find any bugs, please email me at alexelectronicsguy@gmail.com so that I can work on getting them fixed!

## Introduction
I recently built myself a USBWidEx to troubleshoot one of my ProFile hard drives and I'm really happy with it, but I found the surface-mount soldering to be really difficult (I fried three chips in the process) and the Renesas processors used in the device aren't made anymore, so I wanted a cheaper and more simplistic solution.

The result was the ArduinoFile, which uses an Arduino Mega and a simple PCB to interface with Apple parallel port hard drives. It doesn't have nearly as much functionality as the NeoWidEx yet (for instance, no Widget-specific commands are supported at the moment), but I'll keep adding more capabilities over time.

## Hardware
NOTE: The PCB shown in the picture above is a prototype and the final design that I'm working on now will be a lot simpler and more compact, so I've provided the Gerber files and EasyEDA schematic and PCB files for this current design but I'm not going to go into any detail about it.\
\
The final ArduinoFile PCB will be a shield with a 26-pin IDC connector for connecting to the drive and a male header that you can easily hook your logic analyzer to if you end up writing/troubleshooting your own code for the device. I tried to use the ArduinoFile to emulate a ProFile as well and it just ended up being way too slow, so the SD card header probably isn't necessary anymore, but I might still include it in the final design in case anyone finds a use for it. The shield will stack right on top of the Arduino, making things a lot more compact.\
\
The cable for interfacing with the Widget is just a ribbon cable with a 26-pin female IDC connector on each end. The ProFile interface cable has a 26-pin female IDC on one end and a male DB25 connector on the other.


## Software
The code is probably really sloppy since I wrote it all in a hurry in between my college classes, but it certainly gets the job done.\
\
Upload the ArduinoFile.ino Arduino sketch to the Arduino using the Arduino IDE and you should be good to go! Connect to the device using a terminal program with a baud rate of 115200, one stop bit, and no parity. Once connected, you should see a main menu screen with a variety of options.

## How to Use It

NOTE: All numbers entered by the user and displayed by the ArduinoFile are in hex
unless otherwise specified.\
\
NOTE 2: Whenever you enter a block number, you must add leading zeros to make the
entry three bytes long. For instance, you would enter block 1FFF as 001FFF.
\
Upon connecting to the ArduinoFile, you should see a screen that looks like this:\
\

<img width="409" alt="Screen Shot 2021-09-19 at 7 54 53 PM" src="https://user-images.githubusercontent.com/16897189/133947059-ad9e4e78-1bed-431b-8c66-626a50a0f993.png">
\
You can select an option from the menu by typing the corresponding number and pressing return.
### 1 - Reset Drive
This command does exactly what it says: it resets the drive by briefly lowering
/PRES and then raising it again.\
\
<img width="342" alt="Screen Shot 2021-09-19 at 12 58 34 PM" src="https://user-images.githubusercontent.com/16897189/133936104-fdad89b2-f6d8-4f5c-ab96-03e307639744.png">
### 2 - Read Spare Table
Reads the spare table (block FFFFFF) of the drive and displays the raw data in hex.
The drive status bytes and the contents of the spare table are interpreted for
the user as well.\
\
<img width="594" alt="Screen Shot 2021-09-19 at 12 57 01 PM" src="https://user-images.githubusercontent.com/16897189/133936050-80d8c9d8-d926-4033-9d46-1e46c3d5882a.png">
### 3 - Read Block
Reads the desired block into the buffer and displays the hex data that was read.
The status bytes are interpreted for the user. By pressing return at the
retry count and spare threshold prompts, the default values of 64 and 14 are used.
The user can change these values by answering "n" at these prompts.\
\
<img width="588" alt="Screen Shot 2021-09-19 at 2 34 52 PM" src="https://user-images.githubusercontent.com/16897189/133938805-65a25af7-5bb8-426b-8c6c-4ae5736a6400.png">
### 4 - Backup Entire Drive
Backs up the entire contents of the drive to the modern computer over XMODEM.
The ArduinoFile first reads the spare table to determine the size of the drive
that's connected. Press return if the detected drive type is correct or press "n"
to enter a custom drive size in blocks. After confirming drive size, start
the XMODEM receiver (with CRC, not checksum) on the modern computer and the drive will be backed up to a file. The file format is just the raw data from the drive with no headers or anything else weird. This process usually takes around 15 minutes for a 5MB ProFile or 30 minutes for a 10MB ProFile or Widget.\
\
<img width="641" alt="Screen Shot 2021-09-19 at 3 06 19 PM" src="https://user-images.githubusercontent.com/16897189/133939756-0a922a24-aa3b-4934-bf19-7f560a207882.png">
### 5 - Write Buffer to Block
Writes the current contents of the buffer (whatever you last read from the drive)
to a user-specified block on the disk.\
\
<img width="461" alt="Screen Shot 2021-09-19 at 3 16 30 PM" src="https://user-images.githubusercontent.com/16897189/133940042-461532f9-b027-41cf-8c2e-716536ef8aef.png">
### 6 - Restore Drive From Backup
Restores the drive from a backup made with the backup command. Note that this
currently does NOT work with images that have the BLU header or any other format
that's not just the raw data from the disk. Since this operation will destroy
all information currently on your disk, the ArduinoFile gives a warning about this
before beginning the restore operation. It then uses the spare table to determine
the drive's size, which you can change by pressing "n" and entering a custom size
in blocks. After confirming your drive size, start your XMODEM receiver with a
1K block size and wait for the transfer to complete. Just like the backup operation,
this usually takes around 15 minutes for a 5MB ProFile or 30 minutes for a 10MB ProFile or Widget.\
\
<img width="929" alt="Screen Shot 2021-09-19 at 3 27 56 PM" src="https://user-images.githubusercontent.com/16897189/133940394-55853b33-965a-4105-8567-f438e3104d6e.png">
### 7 - Write Zeros to Drive
This command does exactly what it says: it writes zeros to every block on the disk.
Since this destroys all data on the drive, the user gets a warning before the drive
is actually zeroed. The ArduinoFile also reads the spare table to get the size of the
drive so that it knows how many blocks to write zeros to. If you want to use a custom
drive size, you can press "n" when it asks if the detected size is correct and enter
your own size in blocks.\
\
<img width="921" alt="Screen Shot 2021-09-19 at 3 30 36 PM" src="https://user-images.githubusercontent.com/16897189/133940466-c8bd2ffd-c32d-4766-b623-d9703bea917a.png">
### 8 - Low-Level Format
NOTE: This command only works with 5MB ProFiles at the moment. 10MB ProFiles
and Widgets can't be low-level formatted yet, but I'm working on adding support
for them in the future.\
\
This command allows you to low-level format your 5MB ProFile, which can sometimes fix your drive if you're getting lots of read/write errors. Make sure to have the diagnostic ROM installed in the drive with a piggyback Z8 before running this command. Since a low-level format erases all data on the drive, the user gets a warning before proceeding. The ArduinoFile then reads the spare table to get the drive's size and will refuse to continue unless it's a 5MB ProFile. The device will then ask you to install the jumper on the ProFile's control board at P7 and press return to continue. The ArduinoFile will then proceed to format the drive and will ask you to remove the jumper once it's finished. It will then scan the drive for bad blocks and initialize an empty spare table before telling the user that the format is complete.\
\
<img width="930" alt="Screen Shot 2021-09-19 at 3 49 42 PM" src="https://user-images.githubusercontent.com/16897189/133941011-b655b146-1534-4bab-8a41-96e102a4636c.png">
### 9 - Drive Tests
Drive Tests is a submenu that contains a variety of read and write tests for troubleshooting your drive. Each one is described below.\
\
NOTE: You can choose to loop any of these tests forever, in which case the ArduinoFile will show how many passes of the test have been completed so far.
\
\
<img width="412" alt="Screen Shot 2021-09-19 at 7 55 37 PM" src="https://user-images.githubusercontent.com/16897189/133947083-3a99bc9a-6fdb-42dc-b23b-ec0884ab54cb.png">

#### 1 - Sequential Read
This test sequentially reads every block on the disk and reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.\
\
<img width="642" alt="Screen Shot 2021-09-19 at 4 36 14 PM" src="https://user-images.githubusercontent.com/16897189/133942234-fe4d7ece-a6aa-4482-977e-f4bb9991ccad.png">

#### 2 - Sequential Write
This test sequentially writes 0x55 and then 0xAA to every block on the disk and reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Since this will destroy all data on the disk, the user receives a warning before continuing. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.\
\
<img width="932" alt="Screen Shot 2021-09-19 at 4 35 45 PM" src="https://user-images.githubusercontent.com/16897189/133942224-f9160c3d-add1-48f6-b130-671c52dbeca5.png">

#### 3 - Random Read
This test reads a number of random blocks equal to the size of the disk. Note that it DOES NOT necessarily read every block on the disk; just a number of blocks equal to the disk's size. It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. This test puts more stress on the head positioning systems than the sequential test does. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.\
\
<img width="645" alt="Screen Shot 2021-09-19 at 4 35 06 PM" src="https://user-images.githubusercontent.com/16897189/133942206-f18be74e-38da-4c35-9b06-83f15e913a70.png">

#### 4 - Random Write
This test writes 0x55 and then 0xAA to a number of random blocks equal to the size of the disk. Note that it DOES NOT necessarily write to every block on the disk; just a number of blocks equal to the disk's size. It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. This test puts more stress on the head positioning systems than the sequential test does. Since this will destroy all data on the disk, the user receives a warning before continuing. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.\
\
<img width="930" alt="Screen Shot 2021-09-19 at 4 34 04 PM" src="https://user-images.githubusercontent.com/16897189/133942166-f69a39ea-694e-471d-9f02-49ae338615f6.png">

#### 5 - Butterfly Read
This test puts a lot of stress on the positioning system by sweeping the heads across the disk (reading block 0, then 25FE, then 2, then 25FD, etc). It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.\
\
<img width="651" alt="Screen Shot 2021-09-19 at 4 33 19 PM" src="https://user-images.githubusercontent.com/16897189/133942151-23f3ec14-7873-4bdc-938f-eea36029a04a.png">

#### 6 - Read-Write-Read
This test was inspired by the similar option on USBWidEx. Since this destroys all data on your drive, the ArduinoFile gives you a warning before proceeding. As with many of the other commands, the ArduinoFile automatically detects drive size from the spare table, but you can press "n" and enter your own drive size in blocks if you want. The test then starts with Phase 1 in which it reads every block on the disk. In Phase 2, the ArduinoFile writes a test pattern of 555AAA to every block on the disk, with a 16-bit checksum in the last two bytes of each block. Phase 3 consists of the ArduinoFile rereading every block  and checking to make sure that the data matches the pattern just written to the drive. Throughout all three phases of the test, the ArduinoFile will report any errors to the user.\
\
<img width="924" alt="Screen Shot 2021-09-19 at 4 23 27 PM" src="https://user-images.githubusercontent.com/16897189/133941902-68c4e55f-e12b-4c7e-b447-ef35e0dce616.png">

#### 7 - Return to Main Menu
It would be very concerning if you couldn't figure out what this option did.

## Wishlist/Areas For Improvement
- Add details, schematics, and links to the hardware needed for this project once the final design is ready!
- Add Widget-specific commands!
- Low-level format Widgets and 10MB ProFiles!
- Make it check the ROM revision to make sure that you have the diagnostic Z8 fitted before doing a low-level format.
- Read the RAM buffer after a low-level format to see which blocks are bad.
- Maybe add a status LED?
- Allow the user to manually enter custom commands.
- Add comments to the code.
- Read and write to the drive's RAM (requires diagnostic Z8).
- Add individual commands for format, scan, and init spare table (requires diagnostic Z8).
- Allow the user to disable the stepper in a ProFile (requires diagnostic Z8).
- Add support for BLU images and images with 5:1 interleave.
- Add write-verify command.


## Known Bugs
- Sometimes the first read or write to the disk will fail and you have to select "Reset Drive" to get things working properly. After that first read/write, everything works fine.
- Sometimes the terminal freaks out when the ArduinoFile tries to move the cursor back to the beginning of the line and prints the text shifted to the right. I'm not sure if my terminal software or the ArduinoFile itself is to blame for this.
