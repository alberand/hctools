### HC-05 tool

This is tool for configuring and communicationg with cheap Bluetooth modules
HC-05. This firmware was mainly tested on Arduino Nano but should work on any
Arduino.

<img alt="Schematics" style="width: 600px: margin: 30px 0 30px 173px;" src="./schematics.png">

### Interface

Commands:

* **help** - shows list of commands
* **echo** - Run simple echo server. It is useful to check if your module is
  working. You run this command, connect to the module with your phone or PC and
  start sending some text to it. The module will receive the text and send it back
  to you.                                                                                                                             
* **atmode** - This command will switch HC-05 module into AT command mode and
  then turn Arduino into serial passtrhogh device between your PC and HC-05.
  This will allow you to communicate with Bluetooth module without any
  additional hardware (with the exception of Arduino of course)                                                                                
* **master** - configure connected HC-05 module as Bluetooth master.
* **slave** - configure connected HC-05 module as Bluetooth slave. 
* **name** - change Bluetooth name of the module.                  
* **baudrate** - change baudrate of the module. This baudrate is used whe
  module communicates with Arduino.

### References:

* [Platformio](https://platformio.org/)
* [HC-05](https://www.aliexpress.com/wholesale?catId=0&SearchText=HC-05+Bluetooth)
