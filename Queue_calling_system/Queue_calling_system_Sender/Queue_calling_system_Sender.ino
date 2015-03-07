/*--------------------------------------------------------------------------------------
  ps2 keyboard
  Valid irq pins:
  Arduino Uno:  2, 3
  Arduino Due:  All pins, except 13 (LED)
  Arduino Mega: 2, 3, 18, 19, 20, 21
--------------------------------------------------------------------------------------*/
#include <PS2Keyboard.h>
const int DataPin = 2;
const int IRQpin =  3;
PS2Keyboard keyboard;

/*--------------------------------------------------------------------------------------
 LCD1602 module
--------------------------------------------------------------------------------------*/
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //LiquidCrystal(RS, Enable, D4, D5, D6, D7)

int value[3] = {0,0,0};
const byte maxStringLength = 2;

String currentValue="";
int currentNumber, lastNumber = 0;

/*--------------------------------------------------------------------------------------
  setup, Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
  Serial.begin(9600);
  Serial.println("");
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  record_missNumber();
  lcd.blink();
  keyboard.begin(DataPin, IRQpin);
  show_currentNumer_and_LastNumber();
}

/*--------------------------------------------------------------------------------------
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   currentValue= getInput();
   if (currentValue!=".") {
     currentNumber= currentValue.toInt(); //convert currentValue to integer
     if (currentNumber>=100) {
       currentValue="1";
       currentNumber=1;
       lastNumber=1;
     }
     if (lastNumber<currentNumber) lastNumber= currentNumber;
     show_currentNumer_and_LastNumber(); 
   }
   
   char charBuf[maxStringLength+1];
   //currentValue=String(currentNumber); //remove leading zero
   currentValue.toCharArray(charBuf, maxStringLength+1);  //string to char array  
   Serial.print(currentValue+'\n'); //send data to serial port
   delay(100);
}

  
/*--------------------------------------------------------------------------------------
keyboard input 
--------------------------------------------------------------------------------------*/
String getInput() {
  String inputString = "";
  //byte inputStringLength = 0;
  clearFirstLineLCD();

  while (true) {  
    if (keyboard.available()) { // if a key is pressed
      char key = keyboard.read(); 
      
      if (key == PS2_ENTER) break;

      /*
      if (key =='.')  { //blink the matrix LED
        inputString=".";
        break;
      }  
      */

      if ((key == '+') || (key == '-') || (key == '*') || (key == '/') || (key ==  '.')){
         if (key == '+') currentNumber= currentNumber + 1;
         if (key == '-') currentNumber= currentNumber - 1;
         if (key == '*') currentNumber= lastNumber;
         if (key == '/') record_missNumber();
         if (key == '.') currentNumber= currentNumber; //resend current number
         inputString=String(currentNumber); //converting integer into a string
         break; 
      }   
      
      //if (inputString.length()==maxStringLength){  
      //  inputString = "";
      //  break;
     // }
      
     if (key == PS2_BACKSPACE){ //remove one character from the right
        if (inputString.length()>0) {
            inputString= inputString.substring(0,inputString.length()-1); 
            clearFirstLineLCD(); lcd.print(inputString);
         }
      }

      if (inputString.length() < maxStringLength) {
        if (key >= '0' && key <= '9')  {
          lcd.print(key); 
          inputString = inputString + key;
        }
      } //inputString.length
    } //keyboard.available
  } //while(true)
  
  //inputString= formatString(inputString);
  return inputString; 
}

void clearFirstLineLCD()
{
  lcd.setCursor(0, 0);        //first line (x=0, y=0)
  lcd.print("No.:   ");
  lcd.setCursor(4, 0);        //first line (x=4, y=0)  
}

void show_currentNumer_and_LastNumber()
{
  lcd.setCursor(0, 1);        //second line (x=0, y=1)
  lcd.print("                ");

  lcd.setCursor(0, 1);        //second line (x=0, y=1)
  lcd.print("Now:");
  lcd.setCursor(4, 1);        //second line (x=5, y=1)  
  lcd.print(currentNumber);
  
  lcd.setCursor(9, 1);        //second line (x=9, y=1)
  lcd.print("Last:");
  lcd.setCursor(14, 1);        //second line (x=14, y=1)  
  lcd.print(lastNumber);
}

void record_missNumber()
{
  if (currentValue.length()>0) {
    value[2]= value[1];
    value[1]= value[0];
    value[0]= currentNumber;
  }
  print_MissNumber();
}

void print_MissNumber()
{
  lcd.setCursor(8, 0); lcd.print("        "); 
  lcd.setCursor(8, 0); lcd.print(value[0]);  //first line (x=8, y=0)
  lcd.setCursor(11, 0); lcd.print(value[1]);  //first line (x=11, y=0)
  lcd.setCursor(14, 0); lcd.print(value[2]);  //first line (x=14, y=0)
}  

String formatString(String inputString)
{
  if (inputString.length()>0) {
    if (inputString.length()<=2) inputString=inputString+"00";
    if (inputString.length()==3) inputString=inputString+"0";
  }  
  return inputString; 
}


