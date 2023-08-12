#include <Arduino.h>
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal_I2C library
#include <Keypad.h>
#include <math.h>

//Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

//Initialize Keypad
const byte ROWS = 4; 
const byte COLS = 4;  
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 12, 11, 10, 9 };    
byte colPins[COLS] = { 8, 7, 6, 5 };  
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


//Variable Initialization
String setUpMessageLCD = "Hi, I am alive........";
String inputWord = "";
bool isKeyAction;
bool isScrollingAction;
bool isDecoded;
int displayStartRow;

void setup() {
  lcd.init();      
  lcd.backlight(); 
  lcd.begin(16, 2);

  isKeyAction = false;
  isScrollingAction = false;
  isDecoded = false;
  displayStartRow = 0;
  displayWrappedText(setUpMessageLCD);
  Serial.begin(115200);
}

void loop() {
  //Read Key
  char getKey = keypad.getKey();
  if (getKey) {
    //Key Actions
    inputWord = actionForKey(inputWord, getKey, displayStartRow, isScrollingAction, isDecoded);

    //Display Data
    if (isDecoded){
      displayDecodedText(inputWord, displayStartRow);
    } else {
      displayEncodedText(inputWord, displayStartRow, isScrollingAction);
    }
  }
}


//Key Action Function
String actionForKey(String& text, char key, int& displayStartRow, bool& isScrollingAction, bool& isDecoded) {
  int textLength = text.length();
  int minimumStartDisplayRow = 0;
  int totaldisplayRow = textLength/16; 
  int maximumStartDisplayRow = totaldisplayRow - 1;
  switch (key) {
    case 'A':
      text += " ";
      break;
    case 'B':
      if (displayStartRow > minimumStartDisplayRow && isScrollingAction == true) {
        displayStartRow -= 1;
      }
      if (isScrollingAction == false) {
        displayStartRow = totaldisplayRow - 1;
      }
      isScrollingAction = true;
      break;
    case 'C':
      if (maximumStartDisplayRow > displayStartRow) {
        displayStartRow += 1;
      }
      isScrollingAction = true;
      break;
    case 'D':
      text = "";
      isScrollingAction = false;
      isDecoded = false;
      break;
    case '*':
      //Shall be deleted for their creativity.
      text = deleteCharacter(text);
      break;
    case '#':
      displayStartRow = 0;
      isScrollingAction = false;
      if (!isDecoded && text.length() != 0) {
          text = numberToTextDecoder(text);
      }
      isDecoded = true;
      break;
    default:
      isScrollingAction = false;
      if (isDecoded){
        text = "";
        isDecoded = false;
      }
      text += key;
      break;
  }
  return text;
}

// Display Boot Up Text
void displayWrappedText(String& text) {
  int textLength = text.length();
  int row = 0;
  int col = 0;

  for (int i = 0; i < textLength; i++) {
    delay(100);
    lcd.setCursor(col, row);
    lcd.write(text.charAt(i));
    col++;
    if (col >= 16) {
      col = 0;
      row++;
      if (row >= 2) {
        delay(0);
        lcd.clear();
        row = 0;
      }
    }
  }
}

//Display Encoded Text
void displayEncodedText(String& text, int displayStartRow, bool isScrollingAction){
  int textLength = text.length();
  int row =0 ;
  int col = 0;
  int totaldisplayRow = textLength/16; 

  lcd.clear();
  if (textLength < 16){
    lcd.setCursor(row,col);
    lcd.print(text);
  } else {
    if (isScrollingAction){
        for (int i = displayStartRow; i <= displayStartRow + 1; i++) {
        lcd.setCursor(0,row);
        lcd.print(text.substring(i*16, i*16+16));
        row++;
      }
    } else {
        for (int i = totaldisplayRow - 1; i <= totaldisplayRow; i++) {
          lcd.setCursor(0,row);
          lcd.print(text.substring(i*16, i*16+16));
          row++;
        }
    }
  }
}

//Display Decoded Text
void displayDecodedText(String& text, int displayStartRow){
  int textLength = text.length();
  int row =0 ;
  int col = 0;
  int totaldisplayRow = textLength/16; 

  lcd.clear();
  if (textLength < 16){
    lcd.setCursor(row,col);
    lcd.print(text);
  } else {
      for (int i = displayStartRow; i <= displayStartRow + 1; i++) {
      lcd.setCursor(0,row);
      lcd.print(text.substring(i*16, i*16+16));
      row++;
    } 
  }
}


//TODO - Optional
String deleteCharacter(String& text){
  //Do you reset everytime you key in number wrongly?
  //Build a function to delete single character then :)
  String shortenText = "";
  int textLength = text.length();
  if (textLength > 0){
    shortenText = text.substring(0,textLength-1);
  } else {
    shortenText = text;
  }
  return shortenText;
}

//TODO - Main Function
String numberToTextDecoder(String& text){
  //Please include your numeric to text decoder logic here.
  //Input data type example: "23 32 102 203" #Each number represent 1 char, which mean 4 characters in total with the given example.
  //Output data type example: "High" #As mentioned, each number represent 1 char, hence, 4 numbers converted into 4 letter of word.

  //Hint 1: 
  //char y = static_cast<char>(parameter1) 
  //This is a function to convert decimal number into single character. Input parameter data type: integer, Output parameter data type: char 

  //Hint 2:
  //Serial.print() or Serial.println() to print at Serial Monitor Console.
  

  int index = 0; //index for numberArray
  int startingPosition = 0;
  int numArrIndex = 0;
  String convertedResult = "";
  String subString = "";
  int stringToInt;
  char characterValue = ' ';
  int subStringLength;
  int binaryNumberLength = 8;

  for (int i = 0; i < text.length(); i++){
    
    if(text.charAt(i) == ' '){
      subString = text.substring(startingPosition, i);
      subStringLength = subString.length();
      characterValue = subStringLength == binaryNumberLength ? binaryToChar(subString) : decimalToChar(subString);
      convertedResult += characterValue;
      startingPosition = i + 1;
      numArrIndex++;
    }
    if(i == text.length() - 1){
      subString = text.substring(startingPosition); //incase no space, means "101011"
      subStringLength = subString.length();
      characterValue = subStringLength == binaryNumberLength ? binaryToChar(subString) : decimalToChar(subString);
      convertedResult += characterValue;
      numArrIndex++;
    }
  }
  return convertedResult;
}

//Utility
char binaryToChar(const String& binaryString) {
  int charValue = 0;
  int base = 1;
  for (int i = binaryString.length() - 1; i >= 0; i--) {
    if (binaryString[i] == '1') {
      charValue += base;
    }
    base *= 2;
  }
  return static_cast<char>(charValue);
}

//Utility
char decimalToChar(const String& decimalString){
  int stringToInt;
  stringToInt = decimalString.toInt();
  return static_cast<char>(stringToInt);
}