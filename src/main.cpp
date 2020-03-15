#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SimpleCLI.h>

#define STR(x) #x
#define S(A) A STR(\r\n)

#define HC_ATMODE 3
#define HC_STATUS 5
#define HC_POWER 4
#define BAUDRATE 115200

// Pin 10 of Arduino goes to TX (pin 0) of HC-05
// Pin 11 of Arduino goes to RX (pin 1) of HC-05
SoftwareSerial Peripheral(10, 11); // RX, TX

SimpleCLI cli;

Command help;
Command pass;
Command echo;
Command atmode;
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
	Serial.write(S("Echoing every received character. CTRL-D to stop it."));
    Peripheral.write(S("I will resend you everything"));

    while(true){
        if (Peripheral.available()) {
            Peripheral.write(Peripheral.readStringUntil('\n').c_str());
        }

        if (Serial.available()) {
            char c = Serial.read();
            if(c == 0x04){
                break;
            }
            Peripheral.write(c);
        }
    }
    
    // Reset line
    Serial.write('\r');
}

void passCallback(cmd* c){
	Serial.write(S("Passing input stream to the HC-05. CTRL-D to stop it."));
	Serial.write(S("Try 'AT' command to see if HC-05 will respond with 'OK'"));
    while(true){
        if (Peripheral.available()) {
            Serial.write(Peripheral.read());
        }

        if (Serial.available()) {
            char c = Serial.read();
            if(c == 0x04){
                break;
            }
            Peripheral.write(c);
        }
    }

    // Reset line
    Serial.write('\r');
}

void switchToAT(){
  	digitalWrite(HC_POWER, LOW);
    delay(1000);
  	digitalWrite(HC_ATMODE, HIGH);
  	digitalWrite(HC_POWER, HIGH);
}

void switchToNormal(){
  	digitalWrite(HC_ATMODE, LOW);
  	digitalWrite(HC_POWER, HIGH);
    delay(1000);
  	digitalWrite(HC_POWER, LOW);
}

void atmodeCallback(cmd* c){
	Serial.write(S("Switching HC-05 into AT mode"));

    switchToAT();
    passCallback(c);
    switchToNormal();
}

void masterCallback(cmd* c){
    Command cmd(c);

	Serial.write(S("Configuring HC-05 module as master"));

    // Process arguments
    Argument nameArg = cmd.getArgument("name");
    String name = nameArg.getValue();
    String setNameCmd = "AT+NAME=" + name + "\r\n";

    Argument slaveMacArg = cmd.getArgument("slavemac");
    String slaveMac = slaveMacArg.getValue();
    String setSlaveMacCmd = "AT+BIND=" + slaveMac + "\r\n";
    // "AT+BIND=0013,EF,000304"

    switchToAT();

    Peripheral.write(S("AT+ROLE=1"));
    delay(1000);
    Peripheral.write(S("AT+UART=115200,1,0"));
    delay(1000);
    Peripheral.write(setNameCmd.c_str());
    delay(1000);
    Peripheral.write(S("AT+CMODE=0"));
    delay(1000);
    Peripheral.write(setSlaveMacCmd.c_str());
    delay(1000);
    Peripheral.write(S("AT+CLASS=1F00"));
    delay(1000);
    Peripheral.write(S("AT+INQM=1,2,48"));
    delay(1000);
    switchToNormal();
}

void slaveCallback(cmd* c){
    Command cmd(c);

	Serial.write(S("Configuring HC-05 module as slave"));

    // Process arguments
    Argument nameArg = cmd.getArgument("name");
    String name = nameArg.getValue();

    String setNameCmd = "AT+NAME=" + name + "\r\n";

    switchToAT();

    Peripheral.write(S("AT"));
    delay(1000);
    Peripheral.write(S("AT+ROLE=0"));
    delay(1000);
    Peripheral.write(S("AT+UART=115200,1,0"));
    delay(1000);
    Peripheral.write(setNameCmd.c_str());
    delay(1000);
    switchToNormal();
}

void nameCallback(cmd* c){
	Serial.write(S("Set HC-05 name to ''"));
    atmodeCallback(c);

    Peripheral.write(S("AT+NAME=Slavy"));
}

void baudrateCallback(cmd* c){
	Serial.write(S("Set HC-05 baudrate to ''"));
    atmodeCallback(c);

    Peripheral.write(S("AT+UART=115200,1,0"));
}

void statuspinCallback(cmd* c){
	Serial.write(S("Reading STATUS pin of the HC-05 moduel"));
    int status = digitalRead(HC_STATUS);
    Serial.write("STATUS pin of the HC is ");
    Serial.write(status + 0x30);
    Serial.write("\n");
}

void helpCallback(cmd* c){
    Serial.write(S("Help:"));
    Serial.write(S("\thelp: This help"));
    Serial.write(S("\tpass: Passtrhough mode between PC and RX 10 & TX 11"));
    Serial.write(S("\techo: Echo server on the HC-05"));
    Serial.write(S("\tatmode: switch HC-05 into AT mode and run passthrough"));
    Serial.write(S("\tmaster: Configure connected HC-05 as master"));
    Serial.write(S("\tslave: Configure connected HC-05 as slave"));
    Serial.write(S("\tstatuspin: Print value of the pin connected to HC-STATUS"));
    Serial.write(S("\tname: Change name of the HC-05"));
    Serial.write(S("\tbaudrate: Change baudrate of HC-05"));
}

void setup() {
  	Serial.begin(BAUDRATE);
    // give user 20 seconds to insert command
    Serial.setTimeout(20000);
  	Peripheral.begin(BAUDRATE);
    Peripheral.setTimeout(20000);

  	pinMode(LED_BUILTIN, OUTPUT);
  	pinMode(HC_ATMODE, OUTPUT);
  	pinMode(HC_STATUS, INPUT);
  	pinMode(HC_POWER, OUTPUT);

  	digitalWrite(LED_BUILTIN, LOW);
  	digitalWrite(HC_POWER, HIGH);
  	digitalWrite(HC_ATMODE, LOW);

  	Serial.write("HCTOOLS. Version: ");
  	Serial.write(VERSION);
  	Serial.write(" (");
  	Serial.write(COMMIT);
  	Serial.write(")\r\n");

    cli.setOnError(errorCallback); // Set error Callback
	// Add commands
	help = cli.addCommand("help", helpCallback);
	pass = cli.addCommand("pass", passCallback);
	echo = cli.addCommand("echo", echoCallback);
	atmode = cli.addCommand("atmode", atmodeCallback);
	master = cli.addCommand("master", masterCallback);
	slave = cli.addCommand("slave", slaveCallback);
	statuspin = cli.addCommand("statuspin", statuspinCallback);
	name = cli.addCommand("name", nameCallback);
	baudrate = cli.addCommand("baudrate", baudrateCallback);

    name.addPositionalArgument("name");
    baudrate.addPositionalArgument("baudrate");
    master.addPositionalArgument("slavemac");
    master.addPositionalArgument("name");
    slave.addPositionalArgument("name");

    Serial.print("# ");
}

void loop() {
	// Check if user typed something into the serial monitor
    if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

        // Parse the user input into the CLI
        cli.parse(input);
        Serial.print("# ");
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
        Serial.print("# ");
    }
}
