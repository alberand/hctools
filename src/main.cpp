#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SimpleCLI.h>

#define STR(x) #x
#define S(A) A STR(\r\n)

#define ATMODEPIN 1
#define HC_STATUS 4
#define HC_RESET 3
#define BAUDRATE 38400

// Pin 10 of Arduino goes to TX (pin 0) of HC-05
// Pin 11 of Arduino goes to RX (pin 1) of HC-05
SoftwareSerial Peripheral(10, 11); // RX, TX

SimpleCLI cli;

Command help;
Command pass;
Command echo;
Command master;
Command slave;
Command statuspin;
Command name;
Command baudrate;

void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        Serial.print(cmdError.getCommand().toString());
        Serial.println("\"?");
    }
}

void echoCallback(cmd* c){
	Serial.write(S("echoCallback\n"));

    while(true){
        if (Peripheral.available()) {
            Peripheral.write("R: ");
            Peripheral.write(Peripheral.read());
        }

        if (Serial.available()) {
            char c = Serial.read();
            if(c == 0x03){
                break;
            }
            Peripheral.write(c);
        }
    }
}

void passCallback(cmd* c){
	Serial.write(S("passCallback\n"));

    while(true){
        if (Peripheral.available()) {
            Serial.write(Peripheral.read());
        }

        if (Serial.available()) {
            char c = Serial.read();
            if(c == 0x03){
                break;
            }
            Peripheral.write(c);
        }
    }
}

void masterCallback(cmd* c){
	Serial.write(S("nameCallback\n"));
}

void slaveCallback(cmd* c){
	Serial.write(S("slaveCallback\n"));
}

void nameCallback(cmd* c){
	Serial.write(S("nameCallback\n"));
}

void baudrateCallback(cmd* c){
	Serial.write(S("baudrateCallback\n"));
}

void statuspinCallback(cmd* c){
	Serial.write(S("statuspinCallback\n"));
    // int status = digitalRead(HC_STATUS);
    // Serial.write("STATUS of the HC is ");
    // Serial.write(status + 0x30);
    // Serial.write("\r\n");
}

void helpCallback(cmd* c){
    Serial.write(S("Help:"));
    Serial.write(S("\thelp: This help"));
    Serial.write(S("\tpass: Passtrhough mode between PC and RX 10 & TX 11"));
    Serial.write(S("\techo: Echo server on the HC-05"));
    Serial.write(S("\tmaster: Configure connected HC-05 as master"));
    Serial.write(S("\tslave: Configure connected HC-05 as slave"));
    Serial.write(S("\tstatuspin: Print value of the pin connected to HC-STATUS"));
    Serial.write(S("\tname: Change name of the HC-05"));
    Serial.write(S("\tbaudrate: Change baudrate of HC-05"));
}

void setup() {
  	Serial.begin(BAUDRATE);
  	Peripheral.begin(BAUDRATE);
  	pinMode(LED_BUILTIN, OUTPUT);
  	digitalWrite(LED_BUILTIN, LOW);

  	pinMode(HC_STATUS, INPUT);
  	pinMode(HC_RESET, OUTPUT);

  	Serial.write("HCTOOLS. Version: ");
  	Serial.write(VERSION);
  	Serial.write("\r\n");

    cli.setOnError(errorCallback); // Set error Callback
	// Add commands
	help = cli.addCommand("help", helpCallback);
	pass = cli.addCommand("pass", passCallback);
	echo = cli.addCommand("echo", echoCallback);
	master = cli.addCommand("master", masterCallback);
	slave = cli.addCommand("slave", slaveCallback);
	statuspin = cli.addCommand("statuspin", statuspinCallback);
	name = cli.addCommand("name", nameCallback);
	baudrate = cli.addCommand("baudrate", baudrateCallback);

    // ping.addArgument("number");
    // ping.addPositionalArgument("str", "pong");
    // ping.addFlagArgument("c");
}

void loop() {
	// Check if user typed something into the serial monitor
    if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

        // Echo the user input
        Serial.print("# ");
        Serial.println(input);

        // Parse the user input into the CLI
        cli.parse(input);
    }

    if (cli.errored()) {
        CommandError cmdError = cli.getError();

        Serial.print("ERROR: ");
        Serial.println(cmdError.toString());

        if (cmdError.hasCommand()) {
            Serial.print("Did you mean \"");
            Serial.print(cmdError.getCommand().toString());
            Serial.println("\"?");
        }
    }
}
