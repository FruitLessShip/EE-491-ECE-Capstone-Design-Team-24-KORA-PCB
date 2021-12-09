#include <SPI.h>                // Serial monitor library
#include <Wire.h>               // I2C library
#include <OneWire.h>            // library to communicate with arduino
#include <DallasTemperature.h>  // library for temp sensor
#include <Adafruit_SSD1306.h>   // display library
#include <Adafruit_GFX.h>       // display library
#include <FastLED.h>            // LED library
#define LED_PIN   32            // Declare LED_PIN as pin 32
#define num_LED   14            // Declare the number of LEDs as 14
#define COIL 30                 // Declare COIL as pin 30
#define RESET 0                  // Declare RESET as pin 0
#define COIL2 31                // Declare COIL2 as pin 31
#define RESET2 1               // Declare RESET as pin 1
#define WIRELESS 4
#define TCAADDR 0x70            // Address for I2C multiplexor
#define SCREEN_WIDTH 128        // Width of OLED display
#define SCREEN_HEIGHT 64        // Height of OLED display
#define DS1621_sensor 0X48      // Address of Temp sensor
#define ONE_WIRE 9              // DQ Wire connected to digital pin 9

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Setup for Display 1
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Setup for Display 2
Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Setup for Display 3

OneWire oneWire(ONE_WIRE); // Setup for Temp sensor
DallasTemperature DS18B20_Sensor(&oneWire); // Setup for Temp sensor

/*
Function for change between the different ports of the multiplexor. 
Ex. TCA9548A(0) will let you talk to port 0 of the multiplexor.
*/
void TCA9548A(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

CRGB leds[num_LED]; // Define array for LEDs in strip

  float v_Resistance1[] = {990.00, 99.00, 990.00, 99.00}; // Use measured value of resistor in ohms. 1st and 2nd 3.3 [V] rail.
  float v_Resistance2[] = {1500.00, 1000.00, 1500.00, 1000.00, 1500.00, 1000.00}; // Use measured value of resistor in ohms. 1st, 2nd and 3rd 5 [V] rail.
  float v_Resistance3[] = {10800.00, 3600.00, 10800.00, 3600.00}; // Use measured value of resistor in ohms. 1st and 2nd 0-12 [V] rail.

  float c_Resistance1[] = {0.04, 100.00, 14900.00, 0.04, 100.00, 14900.00}; // Use measured value of resistor in ohms. 1st and 2nd 3.3 [V] rail.  
  float c_Resistance2[] = {0.002, 100.00, 29900.00, 0.002, 100.00, 29900.00, 0.002, 100.00, 29900.00}; // Use measured value of resistor in ohms. 1st, 2nd and 3rd 5 [V] rail.
  float c_Resistance3[] = {0.05, 100.00, 5900.00, 0.05, 100.00, 5900.00}; // Use measured value of resistor in ohms. 1st and 2nd 0-12 [V] rail.

  float v_Source1[] = {0.00, 0.00}; // Contains voltage for both 3.3 [V] rails
  float v_Source2[] = {0.00, 0.00, 0.00}; // Contains voltage for all three 5 [V] rails
  float v_Source3[] = {0.00, 0.00}; // Contains voltage for both 0-12 [V] rails
  float shunt_Current1[] = {0.00, 0.00}; // Contains current for both 3.3 [V] rails
  float shunt_Current2[] = {0.00, 0.00, 0.00}; // Contains current for all three 5 [V] rails 
  float shunt_Current3[] = {0.00, 0.00}; // Contains current for both 0-12 [V] rails

  const int buzzer = 2; //buzzer on digital pin 10
  int WarnOne_A = 0; //Used to stop warning beep one from repeating with Ambient sensor
  int WarnOne_C = 0; //Used to stop warning beep one from repeating with Onboard sensor
  int WarnTwo_A = 0; //Used to stop warning beep two from repeating with Ambient sensor
  int WarnTwo_C = 0; //Used to stop warning beep two from repeating with Onboard sensor

  int Set = 0; // Needed for the latch functions
  int Reset = 0; // Needed for the latch functions

/*
Setup function that initilize all pins and displays
*/
void setup() {
    
    Wire.begin(); // Begin I2C communications
    Serial.begin(115200);

    analogReadResolution(12);
    
    pinMode(35, OUTPUT);
    digitalWrite(35, HIGH);
    pinMode(34, OUTPUT);
    digitalWrite(34, HIGH);
    pinMode(33, OUTPUT);
    digitalWrite(33, HIGH);
    
    pinMode(WIRELESS, OUTPUT);
    pinMode(RESET, INPUT_PULLDOWN);  // Set RESET pin as an input
    pinMode(COIL, OUTPUT);  // Set COIL pin as an output
    pinMode(RESET2, INPUT_PULLDOWN); // Set RESET2 pin as an input
    pinMode(COIL2, OUTPUT); // Set COIL2 as an output
    
    pinMode(buzzer, OUTPUT);    // set up pin as output for beeper

    DS18B20_Sensor.begin(); //starts Dallas library

 // Init OLED display on bus number 0 (display 1)
  TCA9548A(0);
  if(!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display1.clearDisplay();

  // Init OLED display on bus number 1
  TCA9548A(1);
  if(!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display2.clearDisplay();

 // Init OLED display on bus number 1
  TCA9548A(7);
  if(!display3.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display3.clearDisplay();

  TCA9548A(2); // Switch to temp sensor
  Wire.begin(); // begin I2C commuication
  Wire.beginTransmission(DS1621_sensor);// connect to sensor
  Wire.write(0xAC); // Set access configuration register
  Wire.write(0);    // set for continuous conversion 
  Wire.beginTransmission(DS1621_sensor); // restart 
  Wire.write(0xEE);  // set for temperature conversion  
  Wire.endTransmission(DS1621_sensor); // end transmission
  delay(2000); // wait for sensor to stablize 
    
    //LED set-up code
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, num_LED); // Define LEDS and set the digital pin to 0 and number of LEDS to 6
    
    // Set analog pins analog 
    pinMode(A0, INPUT); // Set pin 14 as an analog input 
    pinMode(A6, INPUT); // Set pin 20 as an analog input
    pinMode(A8, INPUT); // Set pin 22 as an analog input
    pinMode(A10, INPUT); // Set pin 24 as an analog input
    pinMode(A16, INPUT); // Set pin 40 as an analog input
    pinMode(A14, INPUT); // Set pin 38 as an analog input
    pinMode(A2, INPUT); // Set pin 16 as an analog input
    pinMode(A1, INPUT); // Set pin 15 as an analog input
    pinMode(A7, INPUT); // Set pin 21 as an analog input
    pinMode(A9, INPUT); // Set pin 23 as an analog input
    pinMode(A11, INPUT); // Set pin 25 as an analog input
    pinMode(A17, INPUT); // Set pin 41 as an analog input
    pinMode(A15, INPUT); // Set pin 39 as an analog input
    pinMode(A3, INPUT); // Set pin 17 as an analog input
    
    digitalWrite(COIL, HIGH); // Set COIL pin high
    digitalWrite(COIL2, HIGH); // Set COIL2 pin high
}
/*
Main function that calls all other functions and loops until the microcontrollers reset button
is pressed or power to the microcontroller is turned off
*/
void loop() {
    v_Source1[0] = readVoltage(A0, 1000.00, 100.00); // Call readVoltage function with parameters for the 3.3 [V] rail and set equal to v_Source1[0]
    v_Source2[0] = readVoltage(A7, 1500.00, 1000.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[0]
    v_Source3[0] = readVoltage(A12, 1483.00, 4590.00); // Call readVoltage function with parameters for the 0-12 [V] rail and set equal to v_Source3[0]
    
    v_Source1[1] = readVoltage(A2, 1000.00, 100.00); // Call readVoltage function with parameters for the 3.3 [V] rail and set equal to v_Source1[1]
    v_Source2[1] = readVoltage(A14, 1500.00, 1000.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[1]
    v_Source3[1] = readVoltage(A10, 3600.00, 10800.00); // Call readVoltage function with parameters for the 0-12 [V] rail and set equal to v_Source3[1]
    
    v_Source2[2] = readVoltage(A16, 1500.00, 1000.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[2]
    
    shunt_Current1[0] = readCurrent(A1, 0.040, 100.00, 14900.00, v_Source1[0]); // Call readCurrent function with parameters for the 3.3 [V] rail and set equal to shunt_Current1_1 
    shunt_Current2[0] = readCurrent(A8, 0.002, 100.00, 30000.00, v_Source2[0]); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_1
    shunt_Current3[0] = readCurrent(A13, 0.050, 100.00, 5900.00, v_Source3[0]); // Call readCurrent function with parameters for the 0-12 [V] rail and set equal to shunt_Current3_1
    
    shunt_Current1[1] = readCurrent(A3, 0.040, 100.00, 14900.00, v_Source1[1]); // Call readCurrent function with parameters for the 3.3 [V] rail and set equal to shunt_Current1_2 
    shunt_Current2[1] = readCurrent(A15, 0.002, 100.00, 30000.00, v_Source2[1]); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_2
    shunt_Current3[1] = readCurrent(A11, 0.050, 100.00, 5900.00, v_Source3[1]); // Call readCurrent function with parameters for the 0-12 [V] rail and set equal to shunt_Current3_2
    
    shunt_Current2[2] = readCurrent(A17, 0.002, 100.00, 30000.00, v_Source2[2]); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_3

    // Correction factor to the voltage of the load based on the shunt resistors voltage
    v_Source1[0] = v_Source1[0] - (shunt_Current1[0] * (0.040)); 
    v_Source2[0] = v_Source2[0] - (shunt_Current2[0] * (0.002));
    v_Source3[0] = v_Source3[0] - (shunt_Current3[0] * (0.050));
    v_Source1[1] = v_Source1[1] - (shunt_Current1[1] * (0.040));
    v_Source2[1] = v_Source2[1] - (shunt_Current2[1] * (0.002));
    v_Source3[1] = v_Source3[1] - (shunt_Current3[1] * (0.040));
    v_Source2[2] = v_Source2[2] - (shunt_Current2[2] * (0.002));
    
    // LEDs function that changes the RGB LEDs based on voltage and current of a specific rail
    LEDs(v_Source1[0], shunt_Current1[0], v_Source2[0], shunt_Current2[0], v_Source3[0], shunt_Current3[0], v_Source1[1], shunt_Current1[1], v_Source2[1], shunt_Current2[1]);
    LEDs2(v_Source3[1], shunt_Current3[1], v_Source2[2], shunt_Current2[2]); 

    // if current is less than 1 amp then change units to milliamps
    if(shunt_Current1[0] < 1.00){
        shunt_Current1[0] = shunt_Current1[0] * 1000.00;
      }
    if(shunt_Current2[0] < 1.00){
        shunt_Current2[0] = shunt_Current2[0] * 1000.00;
      }
    if(shunt_Current3[0] < 1.00){
        shunt_Current3[0] = shunt_Current3[0] * 1000.00;
      }
    if(shunt_Current1[1] < 1.00){
        shunt_Current1[1] = shunt_Current1[1] * 1000.00;
      }
    if(shunt_Current2[1] < 1.00){
        shunt_Current2[1] = shunt_Current2[1] * 1000.00;
      }
    if(shunt_Current3[1] < 1.00){
        shunt_Current3[1] = shunt_Current3[1] * 1000.00;
      }
    if(shunt_Current2[2] < 1.00){
        shunt_Current2[2] = shunt_Current2[2] * 1000.00;
        }

    // Call display functions to display on the OLEDs
    display_1(v_Source1[0], v_Source2[0], v_Source3[0], shunt_Current1[0], shunt_Current2[0], shunt_Current3[0]); 
    display_2(v_Source1[1], v_Source2[1], v_Source3[1], v_Source2[2], shunt_Current1[1], shunt_Current2[1], shunt_Current3[1], shunt_Current2[2]);
    
    
    
    // Ambient Sensor Loop
    float temp1 = sensor1();  // requests and returns ambient temp 
    // Onboard Sensor Loop
    float temp2 = sensor2();  // requests and returns onboard temp
    // Checks temp1, temp2 for first warning beep
    if((temp1 > 70.0) || (temp2 > 70.0)){
    warning_One(temp1, temp2);
    }
    // Checks temp1, temp2 for second warning beep
    if((temp1 > 75.0) || (temp2 > 75.0)){
    warning_Two(temp1, temp2);
    }
    // Checks temp1, temp2 for continuous beep
    if((temp1 > 80.0) || (temp2 > 80.0)){
    continuous(temp1, temp2);
    }
   // Display the temperature on the third OLED
   display_3Temp(temp1, temp2);

   // Functions to turn the relay on and off for the 0-12 [V] rails 
   latch(shunt_Current3[0]);
   latch2(shunt_Current3[1]);
}
/*
Function that takes a pin number and two resistor values as inputs. Reads the voltage of that pin and calculates 
the source voltage based on the voltage divider equation. Outputs the source voltage as a float.
*/
float readVoltage(int pinNum, float R1, float R2){     
     float V_temp = 0.00; //Temp variable
     float V_source = 0.00; // return variable

     // read the voltage at the analog pin 50 times and average the value
     for(int i = 0; i < 50; i++){
      
        int R_digital = analogRead(pinNum); // Read digital value at that pin
        float R_voltage = (R_digital * 3.3)/4095.0000; // Find the voltage at that pin based on read value
        float divider = (R2 + R1)/R1; // Calculate the multiplier for the voltage divider equation
        V_source = (R_voltage)*(divider); // Calculate the source voltage based on the divider and the read voltage
        V_temp = V_temp + V_source;   
      }

        V_source = V_temp/50; // Average value

    if(V_source < 0.00){ // If V_source is less than 0 then set it equal to zero
        V_source = 0.00; 
     }

    return V_source; // return V_source
  
  }
/*
Function that takes a pin number and two resistor values as inputs. Reads the voltage of that pin and calculates 
the source voltage based on the voltage divider equation. Outputs the source voltage as a float.
*/
float readCurrent(int pinNum, float R_shunt, float R1, float R2, float voltage){

     float I_temp = 0.00; // Temp variable 
     float shunt_Current = 0.00; // Return Variable
     for(int i = 0; i < 50; i++){
      
        int R_digital = analogRead(pinNum); // Read digital value at that pin
        float R_voltage = (R_digital * 3.3)/4095.0000; // Find the voltage at that pin based on read value
        float V_shunt = R_voltage/(1+R2/R1); // Op amp non-inverting gain equation
        float shunt_Current = V_shunt/R_shunt ; // Calculate current based on Ohm's law
        I_temp = I_temp + shunt_Current;   
      }

        shunt_Current = I_temp/50; // Average Value

    if((shunt_Current < 0.00005) || (voltage == 0.00)){ // If shunt_Current is less than 0 then set it equal to zero
       shunt_Current = 0.00; 
     }
   
    return shunt_Current; // return shunt_Current
  
  }
/*
Function that takes six float values and displays them on OLED display 1
*/
void display_1(float voltage1, float voltage2, float voltage3, float current1, float current2, float current3){
    TCA9548A(0);
    display1.clearDisplay();
    display1.setTextSize(1); // set text size 1 to 8
    display1.setTextColor(WHITE); // set text color to white
    display1.setCursor(0,0); // set cursor postion 
    display1.println("Voltage:"); // print on screen
    display1.setCursor(0,8); // set cursor postion
    display1.println("1:"); // print on screen
    display1.setCursor(0,16); // set cursor postion
    display1.println("2:"); // print on screen
    display1.setCursor(0,24); // set cursor postion
    display1.println("3:");  // print on screen
    display1.setCursor(12,8); // set cursor postion
    display1.println(voltage1); // print voltage 1 on screen
    display1.setCursor(12,16); // set cursor postion
    display1.println(voltage2); // print voltage 2 on screen
    display1.setCursor(12,24); // set cursor postion
    display1.println(voltage3); // print voltage 3 on screen
    display1.setCursor(64,0); // set cursor postion 
    display1.println("Current:"); // print on screen
    display1.setCursor(64,8); // set cursor postion
    display1.println("1:"); // print on screen
    display1.setCursor(64,16); // set cursor postion
    display1.println("2:"); // print on screen
    display1.setCursor(64,24); // set cursor postion
    display1.println("3:");  // print on screen
    display1.setCursor(76,8); // set cursor postion
    display1.println(current1); // print voltage 1 on screen
    display1.setCursor(76,16); // set cursor postion
    display1.println(current2); // print voltage 2 on screen
    display1.setCursor(76,24); // set cursor postion
    display1.println(current3); // print voltage 3 on screen
    display1.display();
  }
/*
Function that takes six float values and displays them on OLED display 2
*/
void display_2(float voltage1, float voltage2, float voltage3, float voltage4, float current1, float current2, float current3, float current4){
    TCA9548A(1);
    display2.clearDisplay();
    display2.setTextSize(1); // set text size 1 to 8
    display2.setTextColor(WHITE); // set text color to white
    display2.setCursor(0,0); // set cursor postion 
    display2.println("Voltage:"); // print on screen
    display2.setCursor(0,8); // set cursor postion
    display2.println("1:"); // print on screen
    display2.setCursor(0,16); // set cursor postion
    display2.println("2:"); // print on screen
    display2.setCursor(0,24); // set cursor postion
    display2.println("3:");  // print on screen
    display2.setCursor(0,32); // set cursor postion
    display2.println("4:");  // print on screen
    display2.setCursor(12,8); // set cursor postion
    display2.println(voltage1); // print voltage 1 on screen
    display2.setCursor(12,16); // set cursor postion
    display2.println(voltage2); // print voltage 2 on screen
    display2.setCursor(12,24); // set cursor postion
    display2.println(voltage3); // print voltage 3 on screen
    display2.setCursor(12,32); // set cursor postion
    display2.println(voltage4); // print voltage 3 on screen
    display2.setCursor(64,0); // set cursor postion 
    display2.println("Current:"); // print on screen
    display2.setCursor(64,8); // set cursor postion
    display2.println("1:"); // print on screen
    display2.setCursor(64,16); // set cursor postion
    display2.println("2:"); // print on screen
    display2.setCursor(64,24); // set cursor postion
    display2.println("3:");  // print on screen
    display2.setCursor(64,32); // set cursor postion
    display2.println("3:");  // print on screen
    display2.setCursor(76,8); // set cursor postion
    display2.println(current1); // print voltage 1 on screen
    display2.setCursor(76,16); // set cursor postion
    display2.println(current2); // print voltage 2 on screen
    display2.setCursor(76,24); // set cursor postion
    display2.println(current3); // print voltage 3 on screen
    display2.setCursor(76,32); // set cursor postion
    display2.println(current4); // print voltage 3 on screen
    display2.display();
    
  }
/*
Function that takes two float values and displays them on OLED display 3
*/
void display_3Temp(float amb_Temp, float board_Temp){
    TCA9548A(7);
    display3.clearDisplay();
    display3.setTextSize(1); // set text size 1 to 8
    display3.setTextColor(WHITE); // set text color to white
    display3.setCursor(0,0); // set cursor postion 
    display3.println("Amb Temp:"); // print on screen
    display3.setCursor(0,8); // set cursor postion
    display3.println(amb_Temp); // print voltage 1 on screen
    display3.setCursor(0,16); // set cursor postion 
    display3.println("Onboard Temp:"); // print on screen
    display3.setCursor(0,24); // set cursor postion
    display3.println(board_Temp); // print voltage 1 on screen
    display3.display();
  }

  
/*
Function that takes ten float variables of votlage and current and changes LED color based on value
*/
void LEDs(float voltage1, float current1, float voltage2, float current2, float voltage3, float current3, float voltage4, float current4, float voltage5, float current5){
  //rail 1: 3.3V @ 0.5A
   if((voltage1 >= 3.135) && (voltage1 <= 3.465)) //if measured voltage value is within 5% of 3.3V
      leds[0] = CRGB (255, 0, 255); //glow green
      else
   if(voltage1 > 3.465) //if it's too high
      leds[0] = CRGB (0, 255, 255); //glow red
      else
   if((voltage1 >= 0.01) && (voltage1 < 3.135)) //if it's too low
      leds[0] = CRGB (255, 255, 0); //glow blue
      else
   if(voltage1 < 0.01) //if it's too low
      leds[0] = CRGB (127, 255, 127); //glow purple
      
   if(current1 < 0.001) //if measured current value is less than 1 mA
      leds[1] = CRGB (127, 255, 127); //glow purple
      else
   if((current1 >= 0.001) && (current1 < 525)) //if measured current value is within 5% of 0.5A
      leds[1] = CRGB (255, 0, 255); //glow green
   else
   if(current1 >= 525) //if it's too high
      leds[1] = CRGB (0, 255, 255); //glow red

      //rail 2: 5V @ 5A
   if((voltage2 >= 4.975) && (voltage2 <= 5.1)) //if measured voltage value is within 5% of 3.3V
      leds[2] = CRGB (255, 0, 255); //glow green
      else
   if(voltage2 > 5.1) //if it's too high                                                                Change Later
      leds[2] = CRGB (0, 255, 255); //glow red
      else
   if((voltage2 >= 0.01) && (voltage2 < 4.975)) //if it's too low
      leds[2] = CRGB (255, 255, 0); //glow blue
      
   if(voltage2 < 0.01) //if it's zero
      leds[2] = CRGB (127, 255, 127); //glow purple
      
   if(current2 < 0.001) //if measured current value is less than 1 mA
      leds[3] = CRGB (127, 255, 127); //glow purple
      else
   if((current2 >= 0.001) && (current2 < 5050)) //if measured current value is within 5% of 0.5A
      leds[3] = CRGB (255, 0, 255); //glow green
      else
   if(current2 >= 5050) //if it's too high
      leds[3] = CRGB (0, 255, 255); //glow red

//rail 3: 0-12V @ 1A
   if((voltage3 >= 0.01) && (voltage3 <= 12.2)) //if measured voltage value is within 5% of 3.3V
      leds[4] = CRGB (255, 0, 255); //glow green
      else
   if(voltage3 > 12.2) //if it's too high                                                                Change Later
      leds[4] = CRGB (0, 255, 255); //glow red
      else
   if(voltage3 < 0.001) //if it's too low
      leds[4] = CRGB (127, 255, 127); //glow purple
        
   if(current3 < 0.001) //if measured current value is less than 1 mA
      leds[5] = CRGB (127, 255, 127); //glow purple
      else
   if((current3 >= 0.001) && (current3 < 1000)) //if measured current value is within 5% of 0.5A           Change Later
      leds[5] = CRGB (255, 0, 255); //glow green
      else
   if(current3 >= 1000) //if it's too high
      leds[5] = CRGB (0, 255, 255); //glow red

   //rail 4: 3.3V @ 0.5A
   if((voltage4 >= 3.135) && (voltage4 <= 3.465)) //if measured voltage value is within 5% of 3.3V
      leds[6] = CRGB (255, 0, 255); //glow green
      else
   if(voltage4 > 3.465) //if it's too high
      leds[6] = CRGB (0, 255, 255); //glow red
      else
   if((voltage4 >= 0.001) && (voltage4 < 3.135)) //if it's too low
      leds[6] = CRGB (255, 255, 0); //glow blue
      else
   if(voltage4 < 0.001) //if it's too low
      leds[6] = CRGB (127, 255, 127); //glow purple
   if(current4 < 0.001) //if measured current value is less than 1 mA
      leds[7] = CRGB (127, 255, 127); //glow purple
      else
   if((current4 >= 0.001) && (current4 < 525)) //if measured current value is within 5% of 0.5A
      leds[7] = CRGB (255, 0, 255); //glow green
   else
   if(current4 >= 525) //if it's too high
      leds[7] = CRGB (0, 255, 255); //glow red

   //rail 5: 5V @ 5A
   if((voltage5 >= 4.975) && (voltage5 <= 5.1)) //if measured voltage value is within 5% of 3.3V
      leds[8] = CRGB (255, 0, 255); //glow green
      else
   if(voltage5 > 5.1) //if it's too high                                                                Change Later
      leds[8] = CRGB (0, 255, 255); //glow red
      else
   if((voltage5 >= 0.01) && (voltage5 < 4.975)) //if it's too low
      leds[8] = CRGB (255, 255, 0); //glow blue
      
   if(voltage5 < 0.001) //if it's zero
      leds[8] = CRGB (127, 255, 127); //glow purple
      
   if(current5 < 0.001) //if measured current value is less than 1 mA
      leds[9] = CRGB (127, 255, 127); //glow purple
      else
   if((current5 >= 0.001) && (current5 < 5050)) //if measured current value is within 5% of 0.5A
      leds[9] = CRGB (255, 0, 255); //glow green
      else
   if(current5 >= 5050) //if it's too high
      leds[9] = CRGB (0, 255, 255); //glow red
  
   FastLED.show();
  }
/*
Function that takes four float variables of votlage and current and changes LED color based on value
*/
void LEDs2(float voltage1, float current1, float voltage2, float current2){
//rail 6: 0-12V @ 1A
   if((voltage1 >= 0.01) && (voltage1 <= 12.2)) //if measured voltage value is within 5% of 3.3V
      leds[10] = CRGB (255, 0, 255); //glow green
      else
   if(voltage1 > 12.2) //if it's too high                                                                Change Later
      leds[10] = CRGB (0, 255, 255); //glow red
      else
   if(voltage1 < 0.01) //if it's too low
      leds[10] = CRGB (127, 255, 127); //glow purple
        
   if(current1 < 0.001) //if measured current value is less than 1 mA
      leds[11] = CRGB (127, 255, 127); //glow purple
      else
   if((current1 >= 0.001) && (current1 < 1000)) //if measured current value is within 5% of 0.5A           Change Later
      leds[11] = CRGB (255, 0, 255); //glow green
      else
   if(current1 >= 1000) //if it's too high
      leds[11] = CRGB (0, 255, 255); //glow red

    //rail 7: 5V @ 5A
   if((voltage2 >= 4.975) && (voltage2 <= 5.1)) //if measured voltage value is within 5% of 3.3V
      leds[12] = CRGB (255, 0, 255); //glow green
      else
   if(voltage2 > 5.1) //if it's too high                                                                Change Later
      leds[12] = CRGB (0, 255, 255); //glow red
      else
   if((voltage2 >= 0.01) && (voltage2 < 4.975)) //if it's too low
      leds[12] = CRGB (255, 255, 0); //glow blue
      
   if(voltage2 < 0.001) //if it's zero
      leds[12] = CRGB (127, 255, 127); //glow purple
      
   if(current2 < 0.001) //if measured current value is less than 1 mA
      leds[13] = CRGB (127, 255, 127); //glow purple
      else
   if((current2 >= 0.001) && (current2 < 5050)) //if measured current value is within 5% of 0.5A
      leds[13] = CRGB (255, 0, 255); //glow green
      else
   if(current2 >= 5050) //if it's too high
      leds[13] = CRGB (0, 255, 255); //glow red
   FastLED.show();
}

/* 
Ambient Sensor (1) Function 
request ambient temp and returns as float 
*/
float sensor1(){
  DS18B20_Sensor.requestTemperatures(); // request temp1 data
  float temp1 = DS18B20_Sensor.getTempCByIndex(0); // 0 refers to first IC on wire - can have multiple DS18B20 on same bus
  return temp1;
}

/* 
Onboard Sensor (2) Function
request pcb temp and returns as float 
can remove serial print functions for final prep 
*/
float sensor2(){
  TCA9548A(2);
  Wire.beginTransmission(DS1621_sensor);  // connect to sensor
  Wire.write(0xAA); // read temperature 
  Wire.endTransmission(false); // send repeated start condition
  Wire.requestFrom(DS1621_sensor, 2); // request 2 bytes from sensor
  float temp2 = Wire.read();
  return temp2; 
}

// Warning 1 Function
// will set a warning beep if either sensor measures higher than 70C.
// Change temporary values used for testing for final prep
void warning_One(float temp1, float temp2){
  //Warning beep one at 70.0 C for ambient sensor
  if ((temp1 >= 70.0) && WarnOne_A == 0) //quick warning beep when ambient temp reaches 70.0 C
  {
    tone(buzzer, 1000); //beep
    delay(100); //keep beeping
    noTone(buzzer); // beeping stops
    WarnOne_A = 1; //prevents this from going off while temperature is lowering back below 70.0 C
  }
  if (temp1 < 70.0) //if ambient temp goes back below 70.0 C, reset warning beep condition
    WarnOne_A = 0;
  //Warning beep one at 70.0 C for onboard sensor
  if ((temp2 >= 70.0) && WarnOne_C == 0) //quick warning beep when onboard temp reaches 70.0 C
  {
    tone(buzzer, 1200); //beep
    delay(100); //keep beeping
    noTone(buzzer); // beeping stops
    WarnOne_C = 1; //prevents this from going off while temperature is lowering back below 70.0 C
  }
  if (temp2 < 70.0) //if onboard temp goes back below 70.0 C, reset warning beep condition
    WarnOne_C = 0;
}

// Second Warning Function
// will beep twice if either sensor measures over 75C.
// change temporary values used for testing for final prep
void warning_Two(float temp1, float temp2){
  if ((temp1 >= 75.0) && WarnTwo_A == 0) //quick warning beep when ambient temp reaches 75.0 C
  {
    tone(buzzer, 2000); //beep one
    delay(50); //keep beeping
    noTone(buzzer); //shut up
    delay(25); //keep silent
    tone(buzzer, 2000); //beep two
    delay(50); //keep beeping
    noTone(buzzer); //shut up
    WarnTwo_A = 1; //prevents this from going off while temperature is lowering back below 75.0 C 
  }
  if (temp1 < 75.0) //if ambient temp goes back below 75.0 C, reset warning beep condition
    WarnTwo_A = 0;
  //Warning beep two at 75.0 C for onboard sensor
  if ((temp2 >= 75.0) && WarnTwo_C == 0) //quick warning beep when onboard temp reaches 75.0 C
  {
    tone(buzzer, 2200); //beep one
    delay(50); //keep beeping
    noTone(buzzer); //shut up
    delay(25); //keep silent
    tone(buzzer, 2200); //beep two
    delay(50); //keep beeping
    noTone(buzzer); //shut up
    WarnTwo_C = 1; //prevents this from going off while temperature is lowering back below 75.0 C 
  }
  if (temp2 < 75.0) //if onboard temp goes back below 75.0 C, reset warning beep condition
    WarnTwo_C = 0;
}

// Continuous beeping until both temperatures return below 80.0 C // 
// Change temporary values for final prep                         
void continuous(float temp1, float temp2){

  if (temp1 >= 80.0 || temp2 >= 80.0) //if either measured temp is above 80.0 C     
  {
    tone(buzzer, 2400); //scream
    delay(45); //keep screaming
    noTone(buzzer); // stop beeping
    delay(45); // no beepng
  }
      else //both temps are below threshold
        noTone(buzzer); // stop beeping
}
/*
Function that monitors when the current of the first 0-12 [V]
rail is above 1 amp. If it is then it opens the relay until someone pushes the button
connected to RESET pin.
*/
void latch(float current){

  Reset = digitalRead(RESET); //Check RESET pin if it has been pushed

  //Set Set variable to zero if current less than specified value
  if(current < 10.00){ 
    Set = 0;
    }
  //Set Set variable to one if current is more than specified value  
  else if(current >= 10.00){
    Set = 1;
    }
  // Print to serial monitor for debugging purposes
  Serial.print("Set: ");
  Serial.println(Set);
  Serial.print("Reset: ");
  Serial.println(Reset);

  if ((Set == 0) && (Reset == 1)){
   digitalWrite(COIL, HIGH);} // Turn on coil if it was off and if the reset button was pushed
  else if ((Set == 1) && (Reset == 0)){
   digitalWrite(COIL, LOW);} // Turn off coil if current is too high
  else if ((Set == 1) && (Reset == 1)){
   digitalWrite(COIL, LOW);} // Turn off coil if current is too high and reset is pressed
  
  }
/*
Function that monitors when the current of the second 0-12 [V]
rail is above 1 amp. If it is then it opens the relay until someone pushes the button
connected to RESET2 pin.
*/
void latch2(float current){
  
  Reset = digitalRead(RESET2); //Check RESET2 pin if it has been pushed
  
  //Set Set variable to zero if current less than specified value
  if(current < 10.00){ 
    Set = 0;
    }

  //Set Set variable to one if current is more than specified value 
  else if(current >= 10.00){
    Set = 1;
    }
  // Print to serial monitor for debugging purposes 
  Serial.print("Set: ");
  Serial.println(Set);
  Serial.print("Reset: ");
  Serial.println(Reset);

  if ((Set == 0) && (Reset == 1)){
   digitalWrite(COIL2, HIGH);} // Turn on coil if it was off and if the reset button was pushed
  else if ((Set == 1) && (Reset == 0)){
   digitalWrite(COIL2, LOW);} // Turn off coil if current is too high
  else if ((Set == 1) && (Reset == 1)){
   digitalWrite(COIL2, LOW);} // Turn off coil if current is too high and reset is pressed
  
  }




  
