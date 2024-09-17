#include <SoftwareSerial.h>
#define RIGHT_LED 8
#define LEFT_LED 9

// Define the RX and TX pins connected to the HC-05
#define HC05_RX 10
#define HC05_TX 11
// Define the EN pin for HC-05
#define HC05_EN 12
// Define the pin to control the HC-05 power
#define HC05_ONOFF 13

// Create a SoftwareSerial object for the HC-05
SoftwareSerial bluetooth(HC05_RX, HC05_TX);

// AT_MODE macro to enable or disable AT mode easily
// Set this to true for AT mode, false for normal operation (pairing mode)
#define AT_MODE false

void setup() {
  // Set pins as output
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(HC05_EN, OUTPUT);
  pinMode(HC05_ONOFF, OUTPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  // Initialize the HC-05 power control pin to OFF (assuming LOW is off)
  digitalWrite(HC05_ONOFF, LOW);
  delay(2000); // Wait a bit for the device to fully turn off


  // Depending on the AT_MODE, set the EN pin accordingly
  if (AT_MODE) {
    // Enable AT mode by setting the EN pin HIGH
    digitalWrite(HC05_EN, HIGH);
    Serial.println("HC05 is in AT Mode");
  } else {
    // Disable AT mode by setting the EN pin LOW
    digitalWrite(HC05_EN, LOW);
    Serial.println("HC05 is in Pairing Mode");
  }
  delay(1000);
  // Open the serial communications and wait for the port to open
  
   
  // Now, turn on the HC-05 by setting HC05_ONOFF high, MAKE SURE TO TURN ON ONLY AFTER THE HC05_EN PIN IS SET
  digitalWrite(HC05_ONOFF, HIGH);
  delay(2000); // Wait a bit for the device to power up and stabilize


  // Set the data rate for the SoftwareSerial port
  bluetooth.begin(9600); // Adjust this baud rate depending on your HC-05's AT mode baud rate

  // Serial.println(AT_MODE ? "AT mode ready" : "Pairing mode ready");
  // Serial.println("");
  delay(1000);
  if(AT_MODE){
    sendATCommand("AT"); 
    sendATCommand("AT+ADDR?");
    sendATCommand("AT+NAME=HC05_MEGA_MASTER");
    sendATCommand("AT+UART=9600,0,0");
    sendATCommand("AT+PSWD=0000");
    sendATCommand("AT+ROLE=0");
  }
}

void sendATCommand(const char* command) {
  Serial.print("Sending: ");
  Serial.println(command);
  bluetooth.println(command);

  // wait for and display response
  long startTime = millis();
  while(!bluetooth.available()){
    if(millis() - startTime > 5000){
      Serial.println("Response Took More Than 5 Seconds");
      return;
    }
  }

  Serial.print("HC05 Response: ");
  while(bluetooth.available()){
    Serial.write(bluetooth.read());
  }
  
  Serial.println();
}

String msg;

void loop() {
  // Read from the HC-05 and write to the Serial Monitor
  if (bluetooth.available()) {
    // Serial.write(bluetooth.read());
    Serial.print("Message received from Master: ");
    msg =  bluetooth.readStringUntil('\n');
    msg.trim();
    Serial.println(msg);
  }
  

  if(msg.length() >0 && strcmp(msg.c_str(), "LEFT") == 0){
    Serial.println("Turning led on");
    digitalWrite(LEFT_LED, HIGH);
    digitalWrite(RIGHT_LED, LOW);
  } else if(msg.length() > 0 && strcmp(msg.c_str(), "RIGHT") == 0){
    digitalWrite(LEFT_LED, LOW);
    digitalWrite(RIGHT_LED, HIGH);
  } else {
    digitalWrite(LEFT_LED, LOW);
    digitalWrite(RIGHT_LED, LOW);
  }
  // Read from the Serial Monitor and write to the HC-05
  // if (Serial.available()) {
  //   // bluetooth.write(Serial.read());
  // }
}
