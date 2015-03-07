/*--------------------------------------------------------------------------------------
 Includes
 --------------------------------------------------------------------------------------*/
#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
//#include "SystemFont5x7.h"
//#include "Arial_Black_16_ISO_8859_1.h"
#include "Arial_black_16.h"

/*--------------------------------------------------------------------------------------
 Only 7 of the 16 pins on the DMD are actually used:
 GND - Hopefully obvious
 nOE - Enable / Disable ALL the LEDs on the DMD
 A - A&B pins select which 1/4 of the DMD is selected 
 B - A&B pins select which 1/4 of the DMD is selected 
 CLK - Used to clock each pixel into the DMD shift registers
 SCLK - Latches the current content of the DMD shift registers
 R - The raw pixel data stream (NB: The 'R' stands for RED. You might notice an unused 'G' pin which is for Green on a HUB12 connector)
 --------
 nOE(D9) |1     2| A(D6)
 Gnd     |3     4| B(D7) 
 Gnd     |5     6| C
 Gnd     |6     8| CLK(D13)
 Gnd     |7    10| SCLK(D8)
 Gnd     |11   12| R(D11) 
 Gnd     |13   14| G
 Gnd     |15   16| D
 ---------
 --------------------------------------------------------------------------------------*/
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

/*--------------------------------------------------------------------------------------
 Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
 called at the period set in Timer1.initialize();
 --------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

const byte maxStringLength = 2;
char charBuf[maxStringLength+1];

String inputString = "";          // a string to hold incoming data
byte character_count;
//String previous_inputString = ""; // hold previous incoming data
boolean stringComplete = false;  // whether the string is complete

#define SPEAKER_PIN 19             //connect speaker to Analog 5 (digital 19)
unsigned long previousMillis_Blink_interval  = 0;
unsigned long previousMillis_Tone_interval  = 0;

#define BLINK_INTERVAL 250 //250 millisecond
byte blinkID=0;
#define TONE_DURATION 1500//1.5 second

/*--------------------------------------------------------------------------------------
 setup, Called by the Arduino architecture before the main loop begins
 --------------------------------------------------------------------------------------*/
void setup(void)
{
  delay(1000);
  Serial.begin(9600);
  Serial.println("DMD Project");
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);

  //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

  dmd.clearScreen(true);

  dmd.selectFont(Arial_Black_16);
  //inputString.toCharArray(charBuf, maxStringLength+1);  //string to char array 
  //updateLEDpanel(charBuf);
  dmd.drawChar( 8,  1, '0', GRAPHICS_NORMAL );  
  dmd.drawChar( 17,  1, '0', GRAPHICS_NORMAL ); 
}

/*--------------------------------------------------------------------------------------
 Arduino architecture main loop
 --------------------------------------------------------------------------------------*/
void loop(void)
{
  // print the string when a newline arrives:
  if (stringComplete) {
    //inputString will reset to null when new line detected
    //we must count the input length before next new line coming in serial
    character_count = inputString.length() -1; //do not count terminate string
    inputString.toCharArray(charBuf, maxStringLength+1);  //string to char array 
    
    //toggle the SPEAKER_PIN to start playing tone 
    digitalWrite(SPEAKER_PIN, LOW);  
    digitalWrite(SPEAKER_PIN, HIGH); 

    blinkID = 1;
    previousMillis_Blink_interval = millis();
    previousMillis_Tone_interval = previousMillis_Blink_interval;
    inputString = "";
    stringComplete = false;    
  }

  if ((blinkID>0) && (blinkID<10)) {
    if (millis() >= previousMillis_Blink_interval + BLINK_INTERVAL) { //run blink_LEDpanel() every 250 milliseconds since BLINK_INTERVAL is predefined to 250
      previousMillis_Blink_interval = millis();
      blink_LEDpanel();
      blinkID = blinkID + 1;
    }
  }

      if (millis() >= previousMillis_Tone_interval + TONE_DURATION) {
        previousMillis_Tone_interval = millis();
        Serial.print("PlayTone"); 
        digitalWrite(SPEAKER_PIN, LOW);
      }  
}

void blink_LEDpanel() {
  if ((blinkID % 2) == 0) { //even number
    dmd.clearScreen(true);
  } 
  else { //odd number
    updateLEDpanel();
  }  
}


/*--------------------------------------------------------------------------------------
 Show numerals on the LED panel
 --------------------------------------------------------------------------------------*/
void updateLEDpanel()
{
  dmd.clearScreen(true);
  int x_offset=(32- 8*(character_count-1)) / maxStringLength;
  dmd.drawChar( x_offset - 5,  1, charBuf[0], GRAPHICS_NORMAL ); 
  dmd.drawChar( x_offset + 4 ,  1, charBuf[1], GRAPHICS_NORMAL ); 
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}


