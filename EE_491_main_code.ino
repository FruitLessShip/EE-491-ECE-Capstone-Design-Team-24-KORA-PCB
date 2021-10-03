#include <SPI.h>                // Serial monitor library
#include <Wire.h>               // I2C library
#include <Adafruit_SSD1306.h>   // display library
#include <Adafruit_GFX.h>       // display library
#include <FastLED.h>            // LED library
#define LED_PIN   23
#define num_LED   2

Adafruit_SSD1306 display(-1); // -1 for no reset pin
CRGB leds[num_LED]; // Define array for LEDs in strip
float v_Source1[] = {0.00, 0.00}; // Contains voltage for both 3.3 [V] rails
float v_Source2[] = {0.00, 0.00, 0.00}; // Contains voltage for all three 5 [V] rails
float v_Source3[] = {0.00, 0.00}; // Contains voltage for both 0-12 [V] rails
float shunt_Current1[] = {0.00, 0.00}; // Contains current for both 3.3 [V] rails
float shunt_Current2[] = {0.00, 0.00, 0.00}; // Contains current for all three 5 [V] rails 
float shunt_Current3[] = {0.00, 0.00}; // Contains current for both 0-12 [V] rails
/*
Setup function that initilize all pins and displays
*/
void setup() {
    //OLED display set-up code
    Wire.setSCL(19); // Declare pin 19 as an SCL pin for I2C communications for OLED
    Wire.setSDA(18); // Declare pin 18 as an SDA pin for I2C communications for OLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize OLED with I2C address 0x3C

    //LED set-up code
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, num_LED); // Define LEDS and set the digital pin to 0 and number of LEDS to 6
    
    // Set pins to digital or analog
    pinMode(1, OUTPUT); // Set pin 1 as an output pin
    digitalWrite(1, HIGH); // Set pin 1 to high logic at 3.3 [V] 
    pinMode(2, OUTPUT); // Set pin 2 as an output pin
    digitalWrite(2, HIGH); // Set pin 2 to high logic at 3.3 [V]
    pinMode(A0, INPUT); // Set pin 14 as an analog input, 3.3 [V] #1 
    //pinMode(A6, INPUT); // Set pin 20 as an analog input, 3.3 [V] #2
    //pinMode(A8, INPUT); // Set pin 22 as an analog input, 5 [V] #1
    //pinMode(A10, INPUT); // Set pin 24 as an analog input, 5 [V] #2
    //pinMode(A16, INPUT); // Set pin 40 as an analog input, 5 [V] #3
    //pinMode(A14, INPUT); // Set pin 28 as an analog input, 0-12 [V] #1
    //pinMode(A2, INPUT); // Set pin 16 as an analog input, 0-12 [V] #2
    //pinMode(A1, INPUT); // Set pin 15 as an analog input, 3.3 [V] current #1
    //pinMode(A7, INPUT); // Set pin 21 as an analog input, 3.3 [V] current #2
    //pinMode(A9, INPUT); // Set pin 23 as an analog input, 5 [V] current #1
    //pinMode(A11, INPUT); // Set pin 25 as an analog input, 5 [V] current #2
    //pinMode(A17, INPUT); // Set pin 41 as an analog input, 5 [V] current #3
    //pinMode(A15, INPUT); // Set pin 39 as an analog input, 0-12 [V] current #1
    //pinMode(A3, INPUT); // Set pin 17 as an analog input, 0-12 [V] current #2 
    
}
/*
Main function that calls all other functions and loops until the microcontrollers reset button
is pressed or power to the microcontroller is turned off
*/
void loop() {
    v_Source1[0] = readVoltage(A0, 979.00, 98.9); // Call readVoltage function with parameters for the 3.3 [V] rail and set equal to v_Source1[0]
    //v_Source2[0] = readVoltage(, 1000.00, 1500.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[0]
    //v_Source3[0] = readVoltage(, 10800.00, 3600.00); // Call readVoltage function with parameters for the 0-12 [V] rail and set equal to v_Source3[0]
    //v_Source1[1] = readVoltage(A0, 979.00, 98.9); // Call readVoltage function with parameters for the 3.3 [V] rail and set equal to v_Source1[1]
    //v_Source2[1] = readVoltage(, 1000.00, 1500.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[1]
    //v_Source3[1] = readVoltage(, 10800.00, 3600.00); // Call readVoltage function with parameters for the 0-12 [V] rail and set equal to v_Source3[1]
    //v_Source2[2] = readVoltage(, 1000.00, 1500.00); // Call readVoltage function with parameters for the 5 [V] rail and set equal to v_Source2[2]
    //shunt_Current1[0] = readCurrent(, 0.04, 1000.00, 149000.00); // Call readCurrent function with parameters for the 3.3 [V] rail and set equal to shunt_Current1_1 
    //shunt_Current2[0] = readCurrent(, 0.002, 1000.00, 299000.00); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_1
    //shunt_Current3[0] = readCurrent(, 0.05, 1000.00, 59000.00); // Call readCurrent function with parameters for the 0-12 [V] rail and set equal to shunt_Current3_1
    //shunt_Current1[1] = readCurrent(, 0.04, 1000.00, 149000.00); // Call readCurrent function with parameters for the 3.3 [V] rail and set equal to shunt_Current1_2 
    //shunt_Current2[1] = readCurrent(, 0.002, 1000.00, 299000.00); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_2
    //shunt_Current3[1] = readCurrent(, 0.05, 1000.00, 59000.00); // Call readCurrent function with parameters for the 0-12 [V] rail and set equal to shunt_Current3_2
    //shunt_Current2[2] = readCurrent(, 0.002, 1000.00, 299000.00); // Call readCurrent function with parameters for the 5 [V] rail and set equal to shunt_Current2_3
    displayVoltage(v_Source1[0], 5.00, 12.00); // Call displayVoltage function with V_source1 and two dummy inputs to display on the OLED
    displayCurrent(0.5, 5.00, 1.00); // Call displayCurrent function with three dummy inputs to display on the OLED
    LEDs(v_Source1[0], 0.5); // LEDs function that changes the RGB LEDs based on voltage and current of a specific rail
}
/*
Function that takes a pin number and two resistor values as inputs. Reads the voltage of that pin and calculates 
the source voltage based on the voltage divider equation. Outputs the source voltage as a float.
*/
float readVoltage(int pinNum, float R1, float R2){
    int R_digital = analogRead(pinNum); // Read digital value at that pin
    float R_voltage = (R_digital * 3.30)/1023.00; // Find the voltage at that pin based on read value
    R_voltage = R_voltage - 0.07; // Offset due to ADC inaccuracy. Found experimently
    float divider = (R2 + R1)/R1; // Calculate the multiplier for the voltage divider equation
    float V_source = (R_voltage)*(divider); // Calculate the source voltage based on the divider and the read voltage

    if(V_source < 0.00){ // If V_source is less than 0 then set it equal to zero
        V_source = 0.00; // Necessary due to the offset value needed to display the correct voltage
      }

    return V_source; // return V_source
  
  }
/*
Function that takes a pin number and two resistor values as inputs. Reads the voltage of that pin and calculates 
the source voltage based on the voltage divider equation. Outputs the source voltage as a float.
*/
float readCurrent(int pinNum, float R_shunt, float R1, float R2){
    int V_outDigital = analogRead(pinNum); // Read digital value at that pin
    float V_out = (V_outDigital * 3.30)/1023.00; // Find the voltage at that pin based on read value
    V_out = V_out - 0.07; // Offset due to ADC inaccuracy. Found experimently
    float V_shunt = V_out/(1+R2/R1); // Op amp non-inverting gain equation
    float shunt_Current = V_shunt/R_shunt ; // Calculate current based on Ohm's law

    if(shunt_Current < 0.00){ // If shunt_Current is less than 0 then set it equal to zero
        shunt_Current = 0.00; // Necessary due to the offset value needed to display the correct current
      }

    return shunt_Current; // return shunt_Current
  
  }
/*
Function that takes three float values and displays them on the OLED display
*/
void displayVoltage(float voltage1, float voltage2, float voltage3){
    display.clearDisplay(); // clear the display
    display.setTextSize(1); // set text size 1 to 8
    display.setTextColor(WHITE); // set text color to white
    display.setCursor(0,0); // set cursor postion 
    display.println("Voltage"); // print on screen
    display.setCursor(0,8); // set cursor postion
    display.println("1:"); // print on screen
    display.setCursor(0,16); // set cursor postion
    display.println("2:"); // print on screen
    display.setCursor(0,24); // set cursor postion
    display.println("3:");  // print on screen
    display.setCursor(12,8); // set cursor postion
    display.println(voltage1); // print voltage 1 on screen
    display.setCursor(12,16); // set cursor postion
    display.println(voltage2); // print voltage 2 on screen
    display.setCursor(12,24); // set cursor postion
    display.println(voltage3); // print voltage 3 on screen
    
  }
/*
Function that takes three float values and displays them on the OLED display
*/
void displayCurrent(float current1, float current2, float current3){
    display.setTextSize(1); // set text size 1 to 8
    display.setTextColor(WHITE); // set text color to white
    display.setCursor(48,0); // set cursor postion 
    display.println("Current"); // print on screen
    display.setCursor(48,8); // set cursor postion
    display.println("1:"); // print on screen
    display.setCursor(48,16); // set cursor postion
    display.println("2:"); // print on screen
    display.setCursor(48,24); // set cursor postion
    display.println("3:");  // print on screen
    display.setCursor(60,8); // set cursor postion
    display.println(current1); // print current 1 on screen
    display.setCursor(60,16); // set cursor postion
    display.println(current2); // print current 2 on screen
    display.setCursor(60,24); // set cursor postion
    display.println(current3); // print current 3 on screen
    display.display(); // Display 
  }

void LEDs(float voltage, float current){
  //rail 1: 3.3V @ 0.5A
   if((voltage >= 3.135) && (voltage <= 3.465)) //if measured voltage value is within 5% of 3.3V
      leds[0] = CRGB (255, 0, 255); //glow green
   else
   if(voltage > 3.465) //if it's too high
      leds[0] = CRGB (0, 255, 255); //glow red
   else
   if(voltage < 3.135) //if it's too low
      leds[0] = CRGB (255, 255, 0); //glow blue

   if((current >= 0.475) && (current <= 0.525)) //if measured current value is within 5% of 0.5A
      leds[1] = CRGB (255, 0, 255); //glow green
   else
   if(current > 0.525) //if it's too high
      leds[1] = CRGB (0, 255, 255); //glow red
     
   FastLED.show();
  }

  
