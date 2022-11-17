/*
  LoRa Duplex communication
  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.
  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback. For an
  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
const int buttonPin = 12;  // the number of the pushbutton pin
const int ledPin = 13;  
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;       // LoRa radio reset
const int irqPin =2;         // change for your board; must be a hardware interrupt pin
const int status_led=0;          //status led

String outgoing;              // outgoing message
String currentStatus;
String currentAnalogvalue;    
int buttonState = 0;
int Analogvalue;
int status=0;               //output value for send
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 1000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex");
  pinMode(status_led, OUTPUT);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String message = String(buttonState);   // send a message
    sendMessage(message);
    Serial.println("Sending value " + message);
     Serial.println();
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 15000;    // 2-3 seconds
    
  }
        // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  //byte incomingLength = LoRa.read();    // incoming msg length
 // String incoming = "";

  while (LoRa.available()) {
   // incoming = LoRa.readString();
   // status = incoming.toInt();
    currentStatus=LoRa.readStringUntil('&');
    currentAnalogvalue=LoRa.readStringUntil('&');

    status=currentStatus.toInt();
    Analogvalue=currentAnalogvalue.toInt();

     
    if(status_led==1)
    {
      digitalWrite(status_led, status);
    }
    else if(status_led==0)
    {
      digitalWrite(status_led, status);
    }
  }

  //if (incomingLength != incoming.length()) {   // check length for error
  //  Serial.println("error: message length does not match length");
  //  return;                             // skip rest of function
  //}

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }
  
  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
//  Serial.println("Message length: " + String(incomingLength));
//  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println("RELAY STATUS: " + String(status));
   Serial.println("ANALOGVALUE: " + String(Analogvalue));
  Serial.println();
}
