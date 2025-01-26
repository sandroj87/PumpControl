/*
 Example : Peristaltic pumps controlled through relay module
 Author  : Sandro Jurinovich
 Date    : 2025-01-26
 Version : 1.0
*/
#include <PumpControl.h>

bool VERBOSE = true;          // Verbosity of the output
const int MAX_PARSE = 10;     // Number of elements in input command string
String input;                 // Buffer string for input

// Create pumpG1 and pumpG2 objects [obj(pin,logic,flow)]
// pin   -> Arduino output pin to control the relay
// logic -> Relate the pin state (LOW/HIGH) to the real on/off state of the pump (depending con the relay configuration)
// flow  -> Pump flow (mL/min)
PumpControl pumpG1(2,true,1.0);
PumpControl pumpG2(3,true,1.0);


/* ******************************************************************
 * setup()
 * This code will be run once
 */
void setup(){

  // Initialize serial comunication
  Serial.begin(9600);
  Serial.println("> Example for pump_control library");

}


/* ******************************************************************
 * loop()
 */
void loop(){

  // Read input from serial or keypad
  input = readInput();

  // Parse input
  parseInput(input);

  // Update G1 status and check if status is changed
  pumpG1.update();
  if(pumpG1.stateChanged()){
    Serial.print("G1: "); Serial.println(pumpG1.getState());
  }

  // Update G2 status and check if status is changed
  pumpG2.update();
  if(pumpG2.stateChanged()){
    Serial.print("G2: "); Serial.println(pumpG2.getState());
  }

}


/* ******************************************************************
 * readInput()
 * Call functions to read commands from different input
 * OUT -> string
 */
String readInput(){
  String inStr = "";        // Buffer string variable
  String inpSource = "";    // Type of source
  inStr = readFromSerial();
  if(inStr != ""){
    inpSource = "serial";
  }
  // Print the input
  if(VERBOSE && inStr != ""){
    Serial.print("Input command from ");
    Serial.print(inpSource);
    Serial.print(": ");
    Serial.println(inStr);
  }
  return inStr;
}


/* ******************************************************************
 * readFromSerial()
 * Read input from serial port
 * OUT -> string : input string from serial
 */
String readFromSerial(){
  // Create buffer string variable
  String inStr = "";
  // Create buffer char variable
  char inChar = "";
  while (Serial.available() > 0) {
    inChar = Serial.read();
    // If <CR> or <NL> char is found in input then stops reaading
    if (inChar == '\r' or inChar == '\n'){
      break;
    }
    // Add the input char to the buffer string
    inStr += inChar;
    delay(10);
  }
  return inStr;
}


/* ******************************************************************
 *  Parse input command from input string
 *  parseInput
 *  IN -> inString str Input string
*/
void parseInput(String inString){
  String elem[MAX_PARSE];
  // If inString is not empty...
  if (inString != ""){
    char *token;  // Buffer array for token
    int i = 0;    // Counter
    // Split the string using "," as delimiter and get the first token
    token = strtok(inString.c_str(), ",");
    elem[0] = token; i++;
    while (token != NULL) {
      // Extract the next token
      token = strtok(NULL, ",");
      elem[i] = token;
      i++;
    }

    // Verbosity of the output
    if(elem[0]=="v"){
       if(VERBOSE){
        VERBOSE = false;
        Serial.println(F("Verbosity: OFF"));
      }else{
        VERBOSE = true;
        Serial.println(F("Verbosity: ON"));
      }
    }

    // Parsing <pump> command
    else if(elem[0]=="pump"){
      String pumpName = elem[1];
      // Pump G1 control
      if(pumpName == "G1"){
        parseDosingPumps(pumpG1,elem);
      }
      // Pump G2 control
      else if(pumpName == "G2"){
        parseDosingPumps(pumpG2,elem);
      }
      // Stops all pumps
      else if(pumpName == "stop"){
        pumpG1.off();
        pumpG2.off();
      }
      else{
        Serial.print(" > Error in <pump> commmand!");
        Serial.println("   Syntax: pump, <pump name>, <command>");
      }
    }

    // Generic error (command is not understood)
    else{
      Serial.print("Command <");Serial.print(elem[0]);Serial.println("> was not understood!");
    }
  }

}


/* ******************************************************************
 *  Parse input for dosing pumps
 *
 */
void parseDosingPumps(PumpControl &pump,String (&elem)[MAX_PARSE]){
  String pumpName = elem[1];
  // Pump on
  if(elem[2] == "on"){
    Serial.print("> Pump: ");Serial.print(pumpName);Serial.println(" ON");
    pump.on();
  }
  // Pump off
  else if(elem[2] == "off"){
    Serial.print("> Pump: ");Serial.print(pumpName);Serial.println(" OFF");
    pump.stop();
  }
  // Pump drop
  else if(elem[2] == "drop"){
    unsigned long nDrops = elem[3].toInt();
    if(nDrops <= 0){nDrops = 1;}
    Serial.print("> Pump: ");Serial.print(pumpName);
    Serial.print(" DROP: ");Serial.println(nDrops);
    pump.drop(nDrops);   
  }
  // Add volume (continuos flow)
  else if(elem[2] == "addvolume"){
    int volume = elem[3].toInt();
    Serial.print("> Pump: ");Serial.print(pumpName);
    Serial.print(" ADD VOLUME: ");Serial.println(volume);
    pump.addVolume(volume);
  }
  // Set flow in mL/min
  else if(elem[2] == "setflow"){
    float flow = elem[3].toFloat();
    Serial.print("> Pump: ");Serial.print(pumpName);
    Serial.print(" SET FLOW: ");Serial.println(flow);
    pump.setFlow(flow);
  }
  // Set drop times
  else if(elem[2] == "setdroptime"){
    unsigned long dropTimeOn  = elem[3].toInt();
    unsigned long dropTimeOff = elem[4].toInt();
    if(dropTimeOn <= 0){dropTimeOn = 250;}
    if(dropTimeOff <= 0){dropTimeOff = 250;}
    Serial.print("> Pump: ");Serial.print(pumpName);
    Serial.print(" TIME DROP ON: ");Serial.print(dropTimeOn);
    Serial.print(" TIME DROP OFF: ");Serial.println(dropTimeOff);
    pump.setDropTimes(dropTimeOn,dropTimeOff);
  }
  else{
    Serial.print(" > Error in <pump,"); Serial.print(pumpName); Serial.println("> commmand!");
    Serial.println("   Available keywords: on, off, drop, addvolume, setflow, setdroptime");
  }
}