#include <SoftwareSerial.h>

// Define the RX and TX pins connected to the HC-05
#define HC05_RX 10
#define HC05_TX 11
// Define the EN pin for HC-05
#define HC05_EN 12
// Define the pin to control the HC-05 power
#define HC05_ONOFF 13
// Slave device address
#define SLAVE_ADDR "98D3,02,965455"

// Create a SoftwareSerial object for the HC-05
SoftwareSerial bluetooth(HC05_RX, HC05_TX);

// AT_MODE macro to enable or disable AT mode easily
// Set this to true for AT mode, false for normal operation (pairing mode)
#define AT_MODE false
// #define INIT_CONNECT false

void config(){
  // starting in AT MODE

  int pair_timeout = 1000; // 10 second timeout
  char buf[50];

  // device off
  digitalWrite(HC05_ONOFF, LOW);
  delay(2000);

  // set EN High
  digitalWrite(HC05_EN, HIGH);
  Serial.println("Starting in AT MODE");
  delay(1000);

  // device on
  digitalWrite(HC05_ONOFF, HIGH);
  delay(5000);

  sendATCommand("AT");
  sendATCommand("AT+RMAAD");
  sendATCommand("AT+CMODE=0");
  sendATCommand("AT+NAME=HC05_UNO_MASTER");
  sendATCommand("AT+UART=9600,0,0");
  sendATCommand("AT+PSWD=0000");
  sendATCommand("AT+ROLE=1");

  // sendATCommand("AT+INQM=0,5,9");
  // sendATCommand("AT+INIT");
  // sendATCommand("AT+INQ");

  // formulation of pair command
  snprintf(buf, sizeof(buf), "AT+BIND=%s", SLAVE_ADDR);
  sendATCommand(buf);
  Serial.println("AT CONNECTION");
  // snprintf(buf, sizeof(buf), "AT+PAIR=%s, 9", SLAVE_ADDR);
  // sendATCommand(buf);

  // snprintf(buf, sizeof(buf), "AT+LINK=%s", SLAVE_ADDR);
  // sendATCommand(buf);
  
  if(!AT_MODE){
    digitalWrite(HC05_ONOFF, LOW);
    digitalWrite(HC05_EN, LOW);
    delay(2000);
    digitalWrite(HC05_ONOFF, HIGH);
    Serial.println("In pairing mode now");
  }
}

void setup() {
  // Set pins as output
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(HC05_EN, OUTPUT);
  pinMode(HC05_ONOFF, OUTPUT);

  bluetooth.begin(9600); // Adjust this baud rate depending on your HC-05's AT mode baud rate
  delay(2000);
  config();

  delay(1000);

  //sending messsage
  if(!AT_MODE)
    delay(10000);
    Serial.println("sending message");
    bluetooth.println("Hello from master");
}

void sendATCommand(const char* command) {
  Serial.print("Sending: ");
  Serial.println(command);
  bluetooth.println(command);

  // wait for and display response
  long startTime = millis();
  while(!bluetooth.available()){
    if(millis() - startTime > 10000){
      Serial.println("Response Took More Than 10 Seconds");
      return;
    }
  }

  Serial.print("HC05 Response: ");
  while(bluetooth.available()){
    Serial.write(bluetooth.read());
  }
  
  Serial.println();
}

void loop() {
  // Read from the HC-05 and write to the Serial Monitor
  // if (bluetooth.available()) {
  //   Serial.write(bluetooth.read());
  // }

  // Read from the Serial Monitor and write to the HC-05
  // if (Serial.available()) {
  //   // bluetooth.write(Serial.read());
  // }

  int slidePot = analogRead(A2);
  // float voltage = slidePot * (3.03/1023.0);
  float voltage = slidePot * (3.3/675);
  
  Serial.print("Sensor Value: ");
  Serial.print(slidePot);
  Serial.println();
  Serial.print(voltage);
  Serial.println();
  float maxV = 3.3;
  float percentage = (voltage/maxV) * 100;
  percentage = constrain(percentage, 0, 100);
  delay(100);

  if(percentage < 30){
    Serial.println("LEFT");
    bluetooth.println("LEFT");
    // delay(500);
  } else if(percentage > 70){
    Serial.println("RIGHT");
    bluetooth.println("RIGHT");
  } else {
    Serial.println("NEITHER");
    bluetooth.println("NEITHER");
  }
}

