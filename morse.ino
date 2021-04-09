/*
 * Latin-to-Morse Transl(iter)ator
 * Michael Ordower
 * Ver 0.1
 * April 2021
 * 
 * Project Description:
 * Silly app that converts ASCII into Morse code. 
 * Made it to show I'm capable of simple C-based embedded systems programming.
 * 
 * Connections:
 * - The app takes input into the Arduino Uno's serial port.
 * - Port 9 of the Arduino should connect to an LED.
 * - Port 10 of the Arduino should connect to a Piezo.
 *  
 *  
 *  Usage:
 * - The user inputs ASCII bytes into the Serial in.
 * - The Piezo should beep, and the LED should blink, playing back Morse Code that matches the ASCII input.
 */

#include <string.h> //not strictly necessary. I only use the strcpy function, but I had extra memory and didn't want to manually populate the character array.

//Miscellaneous constants
const int debug = 1; //boolean flag for printing debug info to serial monitor
const int debugStack = 0; //flag for printing function names in Serial monitor
const int readSerial = 1; //set 1 for reading from serial. 0 for reading hardcoded text string.

//Arduino Port Constants
const int LED_PIN = 9;
const int TONE_PIN = 8;
const int FREQ = 440; //frequency in Hertz of tone

//Timing Constants
const float DOT_LENGTH = 50; //length of a dot in milliseconds
/*
 * Time units.
 * 
 * Dot duration = DOT_LENGTH
 * Dash duration = 3*DOT_LENGTH
 * Pause between morse characters = DOT_LENGTH
 * Space between letters = 3*DOT_LENGTH
 * Space between words = 7*DOT_LENGTH
 */

//Misc. constants
const char* text = "What is this? This is a test. Sir, yes, sir."; //hardcoded string for translation

//struct containing the Morse Code string corresponding to a letter, as well as information about its size
//Morse Code letters consist of 1-to-5 characters, where each character is a dot (short signal) or dash (long signal)
typedef struct MORSE_SIZE_COMBO {
  char l_size; //number of characters in Morse representation of a letter -> use this to iterate through characters in a single morse letter
  char* morse_letter; //string denoting Morse representation of a letter   
} morse;

//Debug functions

//Prints Morse characters to Serial
void printMorse(morse* m){
  debugFunctionName("printMorse(morse*)");
  if (Serial.available() && debug && m){
    Serial.print("[");
    Serial.print(m->morse_letter);
    Serial.print("]"); 
  }
}

//Pass in text to print to Serial. Use this when debugging individual functions
void debugFunctionName(const char *c){
    if (debugStack && Serial.available()){
      Serial.print("In ");
      Serial.print(c);
      Serial.println(".");    
    }
}


//MAIN FUNCTIONS

//Deallocates memory for morse letter pointer and sets pointer to NULL.
void freeMorse(morse* m){
  debugFunctionName("freeMorse()");
   
  if (!m) {
    if (debug && Serial.available()){
      Serial.println("Attempting to free null pointer.");
    }
    return;
  }

  
  free(m->morse_letter);
  free(m);
  m=NULL; //set original pointer to null memory location. Use in boolean conditon (e.g. if(m){}) to check for broken references.
  
}


//PLAYBACK FUNCTIONS

//functions for playing different Morse Code characters
//call these in a switch loop when iterating through Morse letters/strings
void playDot(){
    digitalWrite(LED_PIN,HIGH);
    tone(TONE_PIN, FREQ, DOT_LENGTH);
    delay(DOT_LENGTH);
    digitalWrite(LED_PIN,LOW);
}
void playDash(){
    digitalWrite(LED_PIN,HIGH);
    tone(TONE_PIN, FREQ, 3*DOT_LENGTH);
    delay(3*DOT_LENGTH);
    digitalWrite(LED_PIN,LOW);
  }
void playLetterSpace(){
  digitalWrite(LED_PIN,LOW);
  delay(3*DOT_LENGTH);
  }
void playSpace(){
  digitalWrite(LED_PIN,LOW);
  delay(7*DOT_LENGTH);
  }

//function for playing the entire Morse letter after converting a Latin letter into a string of dots and dashes
//functionality: iterates through a Morse Letter's characters, playing the corresponding sounds
void playLetter(morse* m){
 debugFunctionName("playLetter(morse*)");
  
  if (!m) {
    if (debug && Serial.available()){
      Serial.println("Attempting to play null pointer to Morse letter.");
    }
    return;
  }

  //Traverse string and play each dot or dash
  for (int i=0; i<m->l_size; i++){
    char c = m->morse_letter[i];
    switch(c){
      case '.':
        playDot();
        break;
      case '-':
        playDash();
        break;
      default:
        return; //if input character is not a dot or dash, ignore it and skip pause between letters
        break;
     }
     delay(DOT_LENGTH); //leave space between each dot/dash
  }
 
}



//input the Latin letter + pointer to Morse character. This function will allocate memory for the char* inside the Morse struct. 
void setMorseSize(char letter, morse* m){
  debugFunctionName("setMorseSize(char, morse*)");
  
  //to upper-case
  if (letter >= 'a' && letter <= 'z'){
    letter-='a';
    letter+='A';
  }
  
  //allocates char* for morse*
  switch(letter){
    case 'E':
    case 'T':
    case ' ':
      m->l_size=1;
      break;
    case 'A':
    case 'I':
    case 'M':
    case 'N':
      m->l_size=2;
      break;
    case 'D':
    case 'G':
    case 'K':
    case 'O':
    case 'R':
    case 'S':
    case 'U':
    case 'W':
      m->l_size=3;
      break;
    case 'B':
    case 'C':
    case 'F':
    case 'H':
    case 'J':
    case 'L':
    case 'P':
    case 'Q':
    case 'V':
    case 'X':
    case 'Y':
    case 'Z':
      m->l_size=4;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      m->l_size=5;
      break;
    case ',':
    case '.':
    case '?':
      m->l_size=6;
      break;
    default:
      m->l_size=0;
      break;
  }
 
  m->morse_letter = (char*)malloc((int)(m->l_size)*sizeof(char)); //allocate memory for array of dashes and dots  
}


//input character, return Morse string
morse* getMorse(char letter){
  debugFunctionName("getMorse(char)");
  
  //set letter to upper-case
  if (letter >= 'a' && letter <= 'z'){
    letter-='a';
    letter+='A';
  }

  //allocate memory for morse
  morse* m = (morse*)malloc(sizeof(morse));
  
  setMorseSize(letter,m); //allocate size for malloc
  switch(letter){
    case 'A':
      strcpy(m->morse_letter,".-");
      //m->morse_letter =(char*)(".-");
      break;
    case 'B':
      strcpy(m->morse_letter,"-...");
      break;
    case 'C':
     strcpy(m->morse_letter,"-.-.");
      break;
    case 'D':
      strcpy(m->morse_letter,"-..");
      break;
    case 'E':
      strcpy(m->morse_letter,".");
      break;
    case 'F':
      strcpy(m->morse_letter,"..-.");
      break;
    case 'G':
      strcpy(m->morse_letter,"--.");
      break;
    case 'H':
      strcpy(m->morse_letter,"....");
      break;
    case 'I':
      strcpy(m->morse_letter,"..");
      break;
    case 'J':
      strcpy(m->morse_letter,".---");
      break;
    case 'K':
      strcpy(m->morse_letter,"-.-");
      break;
    case 'L':
      strcpy(m->morse_letter,".-..");
      break;
    case 'M':
      strcpy(m->morse_letter,"--");
      break;
    case 'N':
      strcpy(m->morse_letter,"-.");
      break;
    case 'O':
      strcpy(m->morse_letter,"---");
      break;
    case 'P':
      strcpy(m->morse_letter,".--.");
      break;
    case 'Q':
      strcpy(m->morse_letter,"--.-");
      break;
    case 'R':
      strcpy(m->morse_letter,".-.");
      break;
    case 'S':
      strcpy(m->morse_letter,"...");
      break;
    case 'T':
      strcpy(m->morse_letter,"-");
      break;
    case 'U':
      strcpy(m->morse_letter,"..-");
      break;
    case 'V':
      strcpy(m->morse_letter,"...-");
      break;
    case 'W':
      strcpy(m->morse_letter,".--");
      break;
    case 'X':
      strcpy(m->morse_letter,"-..-");
      break;
    case 'Y':
      strcpy(m->morse_letter,"-.--");
      break;
    case 'Z':
      strcpy(m->morse_letter,"--..");
      break;
    case '0':
      strcpy(m->morse_letter,"-----");
      break;
    case '1':
      strcpy(m->morse_letter,".----");
      break;
    case '2':
      strcpy(m->morse_letter,"..---");
      break;
    case '3':
      strcpy(m->morse_letter,"...--");
      break;
    case '4':
      strcpy(m->morse_letter,"....-");
      break;
    case '5':
      strcpy(m->morse_letter,".....");
      break;
    case '6':
      strcpy(m->morse_letter,"-....");
      break;
    case '7':
      strcpy(m->morse_letter,"--...");
      break;
    case '8':
      strcpy(m->morse_letter,"---..");
      break;
    case '9':
      strcpy(m->morse_letter,"----.");
      break;
    case ' ':
      strcpy(m->morse_letter," ");
      break;
    case '.':
      strcpy(m->morse_letter,".-.-.-");
      break;
    case ',':
      strcpy(m->morse_letter,"--..--");
      break;
    case '?':
      strcpy(m->morse_letter,"..--..");
      break;
    default:
       break;
  }
  
  return m;
}


//Call when streaming characters. This is the main function to call in loop()
void runMorse(char src, morse* mchar){
    debugFunctionName("runMorse(char, morse*)");
  
   //if character is valid, create Morse Character
      if (
          (src >= 'a' && src <= 'z')||
          (src >= 'A' && src <= 'Z') ||
          (src >= '0' && src <= '9') ||
          (src == ',') ||
          (src == '.') ||
          (src == '?') 
          )
      {
         mchar = getMorse(src);
         printMorse(mchar);
         playLetter(mchar);
         freeMorse(mchar);
         playLetterSpace();  
      }
      //If it's a whitespace, pause.
      else if (src == ' ' || src == '\n'){
        if (Serial.available() && debug){
          Serial.print(" ");
        }
        playSpace();
      }
      
}




//Main program:
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //initialize serial
  pinMode(LED_PIN,OUTPUT);   //initialize LED out
  pinMode(TONE_PIN,OUTPUT); //initialize Piezo out
}

void loop() {
  // put your main code here, to run repeatedly:
  morse* mchar;
 
  //If Serial reading is turned off, read the hardcoded string.
  if (!readSerial){
      //if debug mode is on, print the original string
      if (Serial.available() && debug){
          Serial.print("\n");
          for (int i = 0; i< strlen(text); i++){
            Serial.print(text[i]);
          }
          Serial.print("\n");
      }
   
    //iterate through string and play the Morse version of it.
      for (int i = 0; i< strlen(text); i++){
        runMorse(text[i],mchar);
        }

        delay(2000);  //delay 2 seconds before repeating.
    }

  //Stream input from Serial port and convert it to Morse in real time 
  else { 
    while (Serial.available()>0){
      runMorse((char)Serial.read(),mchar);
    }
  }
       
   
}
