// Modified version of Robin2's One Way Transmitter code.
// Source: https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123
//Note: Can comment out Serial.print. Uncomment when using the Serial Monitor

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define the CE and CSN Pins for the nRF24 object.
#define CE_PIN   9
#define CSN_PIN 10

// Address for the two NRF24L01 modules. Should match the Receiver
const byte slaveAddress[5] = {'E','E','C','D','B'};

// Creating the RF24 Object with defined CE CSN pins.
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

// Data that is sent to the receiver from the beginning of successful connection with receiver.

//char dataToSend[32] = "Message 0 _";
char dataToSend[32] = "[Message: 0] [Button Status:  ]";
// txNum iterates from 0 to 9. Used with Serial Monitor to verify that data transfer is successful.
char txNum = '0';
// Character that changes to 'c' if Stop button is pressed. Changes to 'n' if Reset button is pressed.
char txButton = '~';


// Sets the transmission interval.
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 100; // Decrease to increase rate of transmission.

// Pins for the Reset, E-stop, and LED indicator.
int resetPin = 2; 
int estopPin = 1; 
int ledPinTX = 3; 
int warningLED = 5;

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleTx Starting");

    // Setup for Button and LED pins.
    pinMode(estopPin, INPUT);
    pinMode(ledPinTX, OUTPUT);
    pinMode(warningLED, OUTPUT);
    pinMode(resetPin, INPUT); 

    // N
    radio.begin();
    radio.setDataRate( RF24_2MBPS );
     // Set number and delay of retries upon failing to send data successfully.
    radio.setRetries(3,5); // delay, count
    // Set the writing address as the A B C D E from slaveAddress array.
    radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {

      // Send data, then reset timer.
      send();
      prevMillis = millis();

    }
}

//====================

void send() {

    bool rslt;

    // Sends data and its size information to receiver.
    rslt = radio.write( &dataToSend, sizeof(dataToSend)); // sizeof() gives the size (as bytes) of the data being sent.

    Serial.print("[Data Sent] ");
    Serial.print(dataToSend);
 

    // Updates message if only the previous data was sent successfully.
    // If not, it resends the same data before updating the 8th character of the dataToSend character array.
    if (rslt) {
        Serial.println("\t[Acknowledge received]");
            
         updateMessage();
         
    }
    else {
        Serial.println("\t[Tx failed]");
        digitalWrite(warningLED, HIGH);
        delay(100);
        digitalWrite(warningLED, LOW);
        delay(100);
        
    }  
}

//================

// The way the receiver is updating the pin is by checking the dataToSend array's 10th character.
void updateMessage() {
        // so you can see that new data is being sent
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }

    if (digitalRead(estopPin) == HIGH)
    {
      txButton = 'C';
      digitalWrite(warningLED, LOW);
      digitalWrite(ledPinTX, HIGH);
     // Serial.println(dataToSend[10]);
    }
    else if (digitalRead(resetPin) == HIGH)
    {
      txButton = 'N';
      digitalWrite(warningLED, LOW);
      digitalWrite(ledPinTX, LOW);
    }
    else {}
    
    // Updates the 8th character, which is used to verify that the transmitter is resending data that failed to be transmitted previously.
    dataToSend[10] = txNum; //10
    // Updates the 10th character, which updates the Button State; C if E-stop button was pushed, N if Reset. Originally "~" if neither were pushed.
    dataToSend[29] = txButton; //29
}
