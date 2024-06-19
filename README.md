# ArduinoFile
An Arduino-based device for testing and emulating ProFile hard drives.<br>
<br>
![IMG_3865](https://user-images.githubusercontent.com/16897189/146275356-bb4e56cd-9843-4474-87db-e54e72f25cec.jpeg)

## An Important Note
ArduinoFile emulator mode support on the Lisa 2/10 is experimental at the moment. The emulator was previously nonfunctional on the 2/10 because of its faster transfer speeds that the Arduino couldn't keep up with, but it's now just barely fast enough after some optimizations if you use the code and PCB found in the 2/10-Testing branch. But this is still experimental (although it appears to be pretty solid), so just keep that in mind!ZzZZZZZzzzzzzzzzz

## Introduction
I recently built myself a USBWidEx to troubleshoot one of my ProFile hard drives and I'm really happy with it, but I found the surface-mount soldering to be really difficult (I fried three chips in the process) and the Renesas processors used in the device aren't made anymore, so I wanted a cheaper and more simplistic solution.

The result was the ArduinoFile, which uses an Arduino Mega and a simple PCB to interface with Apple parallel port hard drives. It doesn't have nearly as much functionality as the NeoWidEx yet (for instance, no Widget-specific commands are supported at the moment), but I'll keep adding more capabilities over time.

The device is also capable of emulating a ProFile hard drive, using an SD card to store disk images, and has near-complete compatibility with Tom Stepleton's awesome [Cameo/Aphid Selector](https://github.com/stepleton/cameo/tree/master/aphid/selector) program. It's pretty fast too; it takes around 40 seconds to boot into LOS 3.0, which is a full 20 seconds faster than my actual ProFiles, and the emulator only needs two seconds or so to initialize itself after you power it up!

## Hardware
You can find EasyEDA project files for the ArduinoFile schematic and PCB in the the PCB folder as well as a PDF version of the schematic and Gerber files for fabricating your own boards. I use [JLCPCB](https://jlcpcb.com/) for all of my boards because of the low cost, but of course you can use any PCB fabrication service that you want.<br>
<br>
The ArduinoFile PCB is an Arduino Mega shield with a 26-pin IDC connector for connecting to the drive and a male header that you can easily connect to your logic analyzer if you end up writing/troubleshooting your own code for the device. The shield stacks right on top of the Arduino, making things nice and compact. It also contains a reset button for quickly aborting an operation and returning to the main menu when in tester mode and for quickly returning to the Selector disk image when in emulator mode. The board also has an RGB status LED that is described in the emulator mode and diagnostic mode sections later in this document.<br>
<br>
The links in the list below will take you to Mouser product pages for each of the parts needed in this project.
<br>
- 1x: [Arduino Mega microcontroller](https://www.ebay.com/itm/203648886575?_trkparms=ispr%3D1&hash=item2f6a6b672f:g:q9sAAOSwQYJhZh6q&amdata=enc%3AAQAGAAAA4LIn7qRqelfTlmbmnI9oOvzMVWy8U%2FMiHdn9%2BWBISCraykUuABsB7Slvn0zLzFILrQWIzl0WpW1cx0X3taaQwuUjAKJi5oYGOefIkmSc4FPSLr8kPkBOvSk6AhKYlIC4Vd459cDaCMeYuG%2FGeXBrDFQDDg6dKQmXUd7QEUIW%2BvimVexYcYxdNO2yoOZNpuapadbgRgsb8txWJg2rJyfjQ7me8p1jLMFmeKdbdARdez96YPw2wp9O17UvtAjyCr%2BgxSIrRotJVbrT4EknhFMbNMvyg1QmhjJn%2BWPU4EnjidvO%7Ctkp%3ABFBM1uTDh7hf).
- 1x: [100 Ohm resistor](https://www.mouser.com/ProductDetail/603-CFR-12JT-52-100R) for R1.
- 2x: [180 Ohm resistor](https://www.mouser.com/ProductDetail/603-CFR-25JR-52-180R) for R2 and R3.
- 1x: [Common cathode RGB LED](https://www.mouser.com/ProductDetail/604-WP154A4SUREQBFZW) for LED1.
- 1x: [6mm x 5mm tactile switch](https://www.mouser.com/ProductDetail/612-TL1105A) for S1.
- 1x: [10K 10-pin bussed resistor network](https://www.mouser.com/ProductDetail/652-4610X-101-103FLF) for RP1.
- 1x: [47K 10-pin bussed resistor network](https://www.mouser.com/ProductDetail/652-4610X-1LF-47K) for RP2.
- 1x: [74LS280 Parity generator](https://www.mouser.com/ProductDetail/595-SN74LS280N) for U2 and an optional (but recommended) [14-pin socket](https://www.mouser.com/ProductDetail/575-1104731441001000) for it.
- 7x: [20-pin breakaway male headers](https://www.mouser.com/ProductDetail/538-22-28-4200) for J2, J3, and all of the Arduino pins around the edge of the shield.
- 1x: [Right angle 26-pin (2 x 13) connector](https://www.mouser.com/ProductDetail/798-HIF3FC26PA254DS1) for J1.
- 1x: [SD card breakout board from eBay.](https://www.ebay.com/itm/234062594843?_trkparms=ispr%3D1&hash=item367f37c31b:g:mK8AAOSwH8VgNZVx&amdata=enc%3AAQAGAAAA4ORZR7Yd4l6ArFQiZiIk3%2FyGJhLpKxK3SLa0%2Fi1LBqRWuCFXp7hqVWOXCno8xNnjBRqfldA2iaf6Q32doBTPlb1WHn9Gp6ftLFAb%2FXrCKf9krHnGv6JRjlLqRwzdWU6p2XKnCuU78pGbvgVRCmOd4XR9H6KsFnP%2BUDONNDjTlKlKlIiRLIhjIjyq4RL1gXs5%2Bj12YX3dFb6dngFGK1j%2B8DI2GJIw3wXgKQDcjbOS4bz1YG4LNAxjK%2BO%2FrmlrCGgxo1UM1wpks%2BMggcK%2FKZHXeihhzHH47%2FKW%2B3eTTSNQ27uH%7Ctkp%3ABFBMzP2jhrhf)

The cable for interfacing with a Widget is just a 26-pin [ribbon cable](https://www.mouser.com/ProductDetail/517-3365-26FT) with two [26-pin female IDC connectors](https://www.mouser.com/ProductDetail/710-61202623021), one on each end. The ProFile interface cable has a [26-pin female IDC](https://www.mouser.com/ProductDetail/710-61202623021) on one end of the [ribbon cable](https://www.mouser.com/ProductDetail/517-3365-26FT) and a [male DB25 connector](https://www.mouser.com/ProductDetail/523-L117DBFRA25P) on the other. These are the same cables that are used with the Cameo/Aphid, so you can reuse your existing cables if you already have one of those.

Assembly is pretty straightforward: just put each part in the location indicated above and solder it in. See the picture at the start of this document if you're unsure about anything. Make sure to get the polarity of the LED right based on the markings on the silkscreen and ensure that the dot on one end of each resistor pack aligns with the square pad on the board. Also, make sure that you solder in the headers that allow the shield to stack on top of the Arduino before you install the right angle connector since the connector will cover up some of the holes for the Arduino-stacking pins.

Now that your ArduinoFile is all put together, we can talk about how to use it!
## ProFile Emulator Mode

### Software
The code is probably really sloppy since I wrote it all in a hurry in between my classes, but it certainly gets the job done.<br>
<br>
The ArduinoFile emulator software requires the SDFat library for SD card accesses, which can be installed by selecting the Library Manager from the Arduino IDE's tools menu and searching for "SDFat".

<img width="795" alt="Screen Shot 2021-10-17 at 11 17 38 AM" src="https://user-images.githubusercontent.com/16897189/137633654-29ea1a80-f1df-46e8-bd5e-78b22da1c623.png">
<br>Next, use the Arduino IDE to upload the proFileEmulator.ino sketch to the Arduino and use a terminal program to connect to the board with a baud rate of 115200. You should see a message that says "Default drive file profile.image not found! Halting...". This means that everything looks good so far and that we just need to prepare the SD card.
<br><br>
If you plan on using the ArduinoFile without the Cameo/Aphid Selector, just format your microSD card as FAT32 and copy your desired raw disk image (images from the Cameo/Aphid work fine) over to the root directory of the card. Change the file's name to "profile.image", plug the SD card into the Arduino, press the reset button, and you should be up and running!
<br><br>
If you want to use the Selector (HIGHLY recommended), format the card as FAT32 and copy the contents of the SDTemplate folder into the root of the SD card. This will set up the rescue folder with backups of the Selector in ProFile, 3.5", and Twiggy formats and will place an image of the selector called "profile.image" in the root of the card. You can now plug the SD card into the ArduinoFile and press the reset button to initialize the device.

### Status LED
When the ArduinoFile is first initializing the SD card and switching to the default drive image, the status LED will illuminate red. The LED will turn green after these operations have completed (usually around two or three seconds), indicating that the emulator is ready for use. The LED will go dark during a standard read or write operation and will remain green whenever the ArduinoFile is not communicating with the host, just like the "Ready" LED on an actual ProFile. During a Selector "magic block" read or write operation (take a look at the Selector manual for more info about these special commands), the LED will turn blue while the operation occurs instead of just going dark. There isn't really a good reason for this other than that it's cool to be able to see when the ArduinoFile is executing a Selector command instead of a regular read or write! Finally, the LED will turn white if the ArduinoFile receives a "halt emulator" command from the Selector and the reset button will need to be pressed to get the emulator running again.

### How to Use It

To be honest, there's really not a lot to say here. Once you follow the software steps mentioned above, just plug the ArduinoFile into your Lisa and it should work just like a normal ProFile! Note that the ArduinoFile needs to be connected to power (the Lisa doesn't supply power over the parallel port), so keep the USB cable connected while in use. If you don't know how to use the Selector yet, read [this incredibly detailed manual](https://github.com/stepleton/cameo/blob/master/aphid/selector/MANUAL.md) to figure things out. The ArduinoFile is pretty much completely compatible with the Selector, with the exception of a few features that aren't implemented yet (read the note below).

IMPORTANT NOTE: There are two main parts of the Selector protocol that aren't fully functional on the ArduinoFile at the moment. First, I haven't fully implemented the key-value store because it takes the Arduino way too long to look through a database file for the appropriate key-value pair. However, I have made special provisions for the Autoboot and Moniker functions in the Selector, so these work great even though the entire key-value store doesn't. Since the Selector doesn't seem to use the key-value store for anything else, it's unlikely that I'll ever fully implement it unless someone writes a program that needs it. Second, I can't get the SDFat library to retrieve the modification date and time for the files on the SD card for some reason, so the ArduinoFile isn't able to send modification dates when the Selector requests information about a file. Fortunately, the Selector doesn't seem to use this information for anything, so it's not a big deal.

Due to the limitations of the Arduino, duplicating a file on the SD card (the "C" command in the Selector) takes around 18 seconds for a 5MB disk image and longer for larger images. This is so long that the Selector will time out while waiting for the ArduinoFile to duplicate a disk image and will say that the operation might have failed. Don't worry, it probably didn't! Just give it some time to finish up (you can see when it's done if you're connected to the ArduinoFile over serial or by waiting for the status LED to turn from blue to green at the end of the operation) and you should be good to go!

Once you're done with a particular disk image and want to get back to the Selector, you can do this either by unplugging the ArduinoFile and then plugging it back in or by pressing the reset button on the ArduinoFile shield. There isn't an OS or anything running on the Arduino, so the emulator is ready to go within two or three seconds of rebooting it!

The ArduinoFile supports disk images of pretty much any size you want and will automatically adjust the spare table accordingly, so feel free to use 5MB, 10MB, or even larger images (for use with MacWorks only!) and they should all work just fine!

If you connect to the ArduinoFile over serial with a baud rate of 115200, you can see a lot of really interesting information about what it's doing. There are status messages for the [Selector magic block commands](https://github.com/stepleton/cameo/blob/master/aphid/selector/PROTOCOL.md) (the status for duplicating a file is especially useful, as discussed above), for when the drive was reset, and for any errors during communication between the Lisa and the ArduinoFile. Perhaps most interesting of all, the ArduinoFile will print out the command bytes for each command it receives and it's oddly mesmerizing to see them scroll by when you boot an operating system! For the sake of making the emulator faster, I was going to remove these status/debugging messages after I worked out all the bugs, but removing them didn't improve performance by any noticable amount, so I decided to leave them in since they look so cool!
<br><br>
<img width="968" alt="Screen Shot 2021-10-17 at 3 56 57 PM" src="https://user-images.githubusercontent.com/16897189/137642944-60545be0-9b87-497d-a2fc-8992d2ff6bca.png">


## ProFile/Widget Diagnostic Mode

In this mode, the ArduinoFile provides functionality similar to that of USBWidEx and has proven to be very helpful in troubleshooting and backing up ProFiles.

### Software

Upload the proFileTester.ino Arduino sketch to the Arduino using the Arduino IDE and you should be good to go! Connect to the device using a terminal program with a baud rate of 115200. Once connected, you should see a main menu screen with a variety of options.

### How to Use It

NOTE: All numbers entered by the user and displayed by the ArduinoFile are in hex
unless otherwise specified.<br>
<br>
NOTE 2: Whenever you enter a block number, you must add leading zeros to make the
entry three bytes long. For instance, you would enter block 1FFF as 001FFF.
<br>
#### Status LED
In diagnostic mode, the status LED will be green whenever the ArduinoFile is ready to accept a command from the user or whenever an operation completes successfully. The LED will illuminate blue whenever user input is expected (such as confirming the drive type or entering a block number) and will be red whenever an error occurs. The LED will go dark during an actual read or write operation and will be yellow in between reads and writes.
#### Main Menu
Upon connecting to the ArduinoFile, you should see a screen that looks like this:<br>
<br>
<img width="409" alt="Screen Shot 2021-09-19 at 7 54 53 PM" src="https://user-images.githubusercontent.com/16897189/133947059-ad9e4e78-1bed-431b-8c66-626a50a0f993.png">
<br>
You can select an option from the menu by typing the corresponding number and pressing return. If you ever want to abort a long operation like an XMODEM transfer or read test, just disconnect and reconnect the serial connection to the ArduinoFile or press the reset button on the ArduinoFile shield.
<br>
#### 1 - Reset Drive
This command does exactly what it says: it resets the drive by briefly lowering /PRES and then raising it again.<br>
<br>
<img width="342" alt="Screen Shot 2021-09-19 at 12 58 34 PM" src="https://user-images.githubusercontent.com/16897189/133936104-fdad89b2-f6d8-4f5c-ab96-03e307639744.png">
#### 2 - Read Spare Table
Reads the spare table (block FFFFFF) of the drive and displays the raw data in hex.
The drive status bytes and the contents of the spare table are interpreted for
the user as well.<br>
<br>
<img width="594" alt="Screen Shot 2021-09-19 at 12 57 01 PM" src="https://user-images.githubusercontent.com/16897189/133936050-80d8c9d8-d926-4033-9d46-1e46c3d5882a.png">
#### 3 - Read Block
Reads the desired block into the buffer and displays the hex data that was read.
The status bytes are interpreted for the user. By pressing return at the
retry count and spare threshold prompts, the default values of 64 and 14 are used.
The user can change these values by answering "n" at these prompts.<br>
<br>
<img width="588" alt="Screen Shot 2021-09-19 at 2 34 52 PM" src="https://user-images.githubusercontent.com/16897189/133938805-65a25af7-5bb8-426b-8c6c-4ae5736a6400.png">
#### 4 - Backup Entire Drive
Backs up the entire contents of the drive to the modern computer over XMODEM.
The ArduinoFile first reads the spare table to determine the size of the drive
that's connected. Press return if the detected drive type is correct or press "n"
to enter a custom drive size in blocks. After confirming drive size, start
the XMODEM receiver (with CRC, not checksum) on the modern computer and the drive will be backed up to a file. The file format is just the raw data from the drive with no headers or anything else weird. This process usually takes around 15 minutes for a 5MB ProFile or 30 minutes for a 10MB ProFile or Widget.<br>
<br>
<img width="641" alt="Screen Shot 2021-09-19 at 3 06 19 PM" src="https://user-images.githubusercontent.com/16897189/133939756-0a922a24-aa3b-4934-bf19-7f560a207882.png">
#### 5 - Write Buffer to Block
Writes the current contents of the buffer (whatever you last read from the drive)
to a user-specified block on the disk.<br>
<br>
<img width="461" alt="Screen Shot 2021-09-19 at 3 16 30 PM" src="https://user-images.githubusercontent.com/16897189/133940042-461532f9-b027-41cf-8c2e-716536ef8aef.png">
#### 6 - Restore Drive From Backup
Restores the drive from a backup made with the backup command. Note that this
currently does NOT work with images that have the BLU header or any other format
that's not just the raw data from the disk. Since this operation will destroy
all information currently on your disk, the ArduinoFile gives a warning about this
before beginning the restore operation. It then uses the spare table to determine
the drive's size, which you can change by pressing "n" and entering a custom size
in blocks. After confirming your drive size, start your XMODEM sender with a
1K block size and wait for the transfer to complete. Just like the backup operation,
this usually takes around 15 minutes for a 5MB ProFile or 30 minutes for a 10MB ProFile or Widget.<br>
<br>
<img width="929" alt="Screen Shot 2021-09-19 at 3 27 56 PM" src="https://user-images.githubusercontent.com/16897189/133940394-55853b33-965a-4105-8567-f438e3104d6e.png">
#### 7 - Write Zeros to Drive
This command does exactly what it says: it writes zeros to every block on the disk.
Since this destroys all data on the drive, the user gets a warning before the drive
is actually zeroed. The ArduinoFile also reads the spare table to get the size of the
drive so that it knows how many blocks to write zeros to. If you want to use a custom
drive size, you can press "n" when it asks if the detected size is correct and enter
your own size in blocks.<br>
<br>
<img width="921" alt="Screen Shot 2021-09-19 at 3 30 36 PM" src="https://user-images.githubusercontent.com/16897189/133940466-c8bd2ffd-c32d-4766-b623-d9703bea917a.png">
#### 8 - Low-Level Format
NOTE: This command only works with 5MB ProFiles at the moment. 10MB ProFiles
and Widgets can't be low-level formatted yet, but I'm working on adding support
for them in the future.<br>
<br>
This command allows you to low-level format your 5MB ProFile, which can sometimes fix your drive if you're getting lots of read/write errors. Make sure to have the diagnostic ROM installed in the drive with a piggyback Z8 before running this command. Since a low-level format erases all data on the drive, the user gets a warning before proceeding. The ArduinoFile then reads the spare table to get the drive's size and will refuse to continue unless it's a 5MB ProFile. The device will then ask you to install the jumper on the ProFile's control board at P7 and press return to continue. The ArduinoFile will then proceed to format the drive and will ask you to remove the jumper once it's finished. It will then scan the drive for bad blocks and initialize an empty spare table before telling the user that the format is complete.<br>
<br>
<img width="930" alt="Screen Shot 2021-09-19 at 3 49 42 PM" src="https://user-images.githubusercontent.com/16897189/133941011-b655b146-1534-4bab-8a41-96e102a4636c.png">
#### 9 - Drive Tests
Drive Tests is a submenu that contains a variety of read and write tests for troubleshooting your drive. Each one is described below.<br>
<br>
NOTE: You can choose to loop any of these tests forever, in which case the ArduinoFile will show how many passes of the test have been completed so far.
<br>
<br>
<img width="412" alt="Screen Shot 2021-09-19 at 7 55 37 PM" src="https://user-images.githubusercontent.com/16897189/133947083-3a99bc9a-6fdb-42dc-b23b-ec0884ab54cb.png">

##### 1 - Sequential Read
This test sequentially reads every block on the disk and reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.<br>
<br>
<img width="642" alt="Screen Shot 2021-09-19 at 4 36 14 PM" src="https://user-images.githubusercontent.com/16897189/133942234-fe4d7ece-a6aa-4482-977e-f4bb9991ccad.png">

##### 2 - Sequential Write
This test sequentially writes 0x55 and then 0xAA to every block on the disk and reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Since this will destroy all data on the disk, the user receives a warning before continuing. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.<br>
<br>
<img width="932" alt="Screen Shot 2021-09-19 at 4 35 45 PM" src="https://user-images.githubusercontent.com/16897189/133942224-f9160c3d-add1-48f6-b130-671c52dbeca5.png">

##### 3 - Random Read
This test reads a number of random blocks equal to the size of the disk. Note that it DOES NOT necessarily read every block on the disk; just a number of blocks equal to the disk's size. It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. This test puts more stress on the head positioning systems than the sequential test does. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.<br>
<br>
<img width="645" alt="Screen Shot 2021-09-19 at 4 35 06 PM" src="https://user-images.githubusercontent.com/16897189/133942206-f18be74e-38da-4c35-9b06-83f15e913a70.png">

##### 4 - Random Write
This test writes 0x55 and then 0xAA to a number of random blocks equal to the size of the disk. Note that it DOES NOT necessarily write to every block on the disk; just a number of blocks equal to the disk's size. It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. This test puts more stress on the head positioning systems than the sequential test does. Since this will destroy all data on the disk, the user receives a warning before continuing. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.<br>
<br>
<img width="930" alt="Screen Shot 2021-09-19 at 4 34 04 PM" src="https://user-images.githubusercontent.com/16897189/133942166-f69a39ea-694e-471d-9f02-49ae338615f6.png">

##### 5 - Butterfly Read
This test puts a lot of stress on the positioning system by sweeping the heads across the disk (reading block 0, then 25FE, then 2, then 25FD, etc). It also reports any errors to the user, along with an interpretation of the status bytes when the error occurred. Although the ArduinoFile reads the spare table to determine drive size automatically, you can press "n" and enter a custom drive size in blocks if you want.<br>
<br>
<img width="651" alt="Screen Shot 2021-09-19 at 4 33 19 PM" src="https://user-images.githubusercontent.com/16897189/133942151-23f3ec14-7873-4bdc-938f-eea36029a04a.png">

##### 6 - Read-Write-Read
This test was inspired by the similar option on USBWidEx. Since this destroys all data on your drive, the ArduinoFile gives you a warning before proceeding. As with many of the other commands, the ArduinoFile automatically detects drive size from the spare table, but you can press "n" and enter your own drive size in blocks if you want. The test then starts with Phase 1 in which it reads every block on the disk. In Phase 2, the ArduinoFile writes a test pattern of 555AAA to every block on the disk, with a 16-bit checksum in the last two bytes of each block. Phase 3 consists of the ArduinoFile rereading every block  and checking to make sure that the data matches the pattern just written to the drive. Throughout all three phases of the test, the ArduinoFile will report any errors to the user.<br>
<br>
<img width="924" alt="Screen Shot 2021-09-19 at 4 23 27 PM" src="https://user-images.githubusercontent.com/16897189/133941902-68c4e55f-e12b-4c7e-b447-ef35e0dce616.png">

##### 7 - Return to Main Menu
It would be very concerning if you couldn't figure out what this option did.

## Wishlist/Areas For Improvement
The items on the wishlist are listed in order of priority. The ones that I deem most important are closer to the top of the lists.
If you find any bugs, please email me at alexelectronicsguy@gmail.com so that I can work on getting them fixed!
### Tester Mode
- Allow the user to manually enter custom commands.
- Add Widget-specific commands!
- Low-level format Widgets and 10MB ProFiles!
- Make it check the ROM revision to make sure that you have the diagnostic Z8 fitted before doing a low-level format.
- Read the RAM buffer after a low-level format to see which blocks are bad.
- Read and write to the drive's RAM (requires diagnostic Z8).
- Add individual commands for format, scan, and init spare table (requires diagnostic Z8).
- Allow the user to disable the stepper in a ProFile (requires diagnostic Z8).

### Emulator Mode
- Utilize the status bytes if an SD card read/write error occurs (currently, the status bytes are just always zero).
- Get the ArduinoFile to send modification dates and times to the Selector upon request.
- Make the Selector uptime counter do something more interesting since it can't be used for keeping time in this situation.

### Both
- Maybe add a switch that the user can toggle to switch between emulator and tester modes on the fly without having to upload new code?
- Add comments to the code.
- Add support for BLU images and images with 5:1 interleave.

## Known Bugs
### Tester Mode
- Sometimes the first read or write to the disk will fail and you have to select "Reset Drive" to get things working properly. After that first read/write, everything works fine.
- Sometimes the terminal freaks out when the ArduinoFile tries to move the cursor back to the beginning of the line and prints the text shifted to the right. I'm not sure if my terminal software or the ArduinoFile itself is to blame for this.

### Emulator Mode
- None!

## Changelog
- 1.0 - Initial Release
- 1.1 - Fixed a timing issue where the ArduinoFile doesn't wait long enough for the Lisa to put an 0x55 on the bus, causing timeouts on some Lisas.
- 1.2 - Fixed an issue where the status LED doesn't light back up after a read or write, added blue and white LED indications for magic block and halt commands, and implemented the Autoboot and Moniker portions of the key-value store.

## Acknowledgements
- This project would not have been possible without the help of the LisaList community, especially Tom Stepleton and his awesome [Cameo/Aphid project](https://github.com/stepleton/cameo/tree/master/aphid).
- I would also like to thank James Denton from [ArcaneByte](http://www.arcanebyte.com/) for helping me find and fix bugs in the ProFile emulator software.
- Dr. Patrick Schäfer's [reverse-engineering of the ProFile communications protocol](http://john.ccac.rwth-aachen.de:8000/patrick/idefile.htm) for his IDEFile project was really invaluable too!
