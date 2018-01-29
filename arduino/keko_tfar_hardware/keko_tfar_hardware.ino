#include <Keypad.h>
#include <LiquidCrystal.h>

#define FREQ_MODE_DECI 1
#define FREQ_MODE_FRAC 0
#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

// keypad setup
const byte KEYPAD_ROWS = 4; //four rows
const byte KEYPAD_COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char keypadHexaKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte keypadRowPins[KEYPAD_ROWS] = {12, 11, 10, 9}; //connect to the row pinouts of the keypad
byte keypadColPins[KEYPAD_COLS] = {8, 7, 6, 5}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(keypadHexaKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS); 

int volumeEncoderPinA = 2;
int volumeEncoderPinB = 3;
int volumeEncoderPos = 7;
int volumeEncoderPinALast = LOW;

// channel switch buttons
int buttonApin = 28;
int buttonBpin = 29;
int channel = 1;
int additionalChannel = 0;

// channel frequency
float freq[8];
byte freqMode = FREQ_MODE_DECI;

int buttonStateA;    
int lastButtonStateA = LOW;
int buttonStateB;    
int lastButtonStateB = LOW;

unsigned long lastDebounceTimeA = 0;
unsigned long debounceDelayA = 50;
unsigned long lastDebounceTimeB = 0;
unsigned long debounceDelayB = 50;

void setup(){
  pinMode(volumeEncoderPinA, INPUT);
  pinMode(volumeEncoderPinB, INPUT);

  pinMode(buttonApin, INPUT);  
  pinMode(buttonBpin, INPUT);  
  
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("CH ");
}

void clipEncoderPos(){
  if(volumeEncoderPos < 0) 
    volumeEncoderPos = 0;  
  else if (volumeEncoderPos > 10) 
    volumeEncoderPos = 10;  
}

void clipChannel(){
  if(channel < 1) 
    channel = 8;  
  else if (channel > 8) 
    channel = 1;  
}

void updateVolumeDisplay(){
  lcd.setCursor(0, 1);

  if(volumeEncoderPos == 10) {
    lcd.print("Volume: ");
    lcd.setCursor(8, 1);
  } 
  else if(volumeEncoderPos != 0) {
    lcd.print("Volume:  ");
    lcd.setCursor(9, 1);
  }
  else {    
    lcd.print("Volume:   ");
    lcd.setCursor(10, 1);
  }
  
  lcd.print(volumeEncoderPos * 10);
}

void updateChannelDisplay(){
  lcd.setCursor(0, 0);

  if(channel == additionalChannel)
    lcd.print("CA ");  
  else
    lcd.print("CH "); 
  
  lcd.setCursor(3, 0);  
  lcd.print(channel);
}

void updateFreqDisplay(){
  if(freq[channel-1] > 0){
      int decimal = ((int)(freq[channel-1] * 10)) % 10;
      String mystring = String(freq[channel-1]);
      if(decimal > 0){
        mystring = mystring.substring(0, mystring.length()-1);
      }
      else {
        if(freqMode == FREQ_MODE_FRAC)
          mystring = mystring.substring(0, mystring.length()-1);        
        else
          mystring = mystring.substring(0, mystring.length()-3);             
      }
      
      lcd.setCursor(16 - mystring.length(), 0);
      lcd.print(freq[channel-1]);
    }      
}

void clearFreqDisplay(){
   lcd.setCursor(4, 0);
   lcd.print("               ");
}

String channelsToString(){
  String output = "[";
  for(unsigned int i = 0; i < arr_len( freq ); i++){
    if(i != 0)
      output+=",";    
    output+=freq[i];    
  }
  output+="]";
  return output;
}

void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    switch (customKey) {
      case 'A':case 'B':case 'C':case 'D':        
        freq[channel-1]=0;
        freqMode = FREQ_MODE_DECI;
        clearFreqDisplay();
        break;
      case '*':
        freqMode = FREQ_MODE_FRAC;
        break;
      case '#':
        if(additionalChannel == channel)
          additionalChannel = 0;
        else
          additionalChannel=channel;              
        break;
      default:        
        int digit = (int) customKey - 48;
        if(freqMode == FREQ_MODE_DECI){
          freq[channel-1]*=10;
          freq[channel-1]+=digit;
        }
        else{
          int tFreq = (int) freq[channel-1] * 10;
          tFreq= tFreq >> 1;
          tFreq= tFreq << 1;
          tFreq+= digit;          
          freq[channel-1]= (float) tFreq / 10.0f;
        }        
        break;      
    }
  }

  int n = digitalRead(volumeEncoderPinA);
  if ((volumeEncoderPinALast == LOW) && (n == HIGH)) {
    if (digitalRead(volumeEncoderPinB) == LOW)      
      volumeEncoderPos-=1;
    else
      volumeEncoderPos+=1;    
    clipEncoderPos();
  }  
  volumeEncoderPinALast = n;




  int readingA = digitalRead(buttonApin);
  if (readingA != lastButtonStateA) {
    lastDebounceTimeA = millis();
  }

  if ((millis() - lastDebounceTimeA) > debounceDelayA) {
    if (readingA != buttonStateA) {
      buttonStateA = readingA;
      if (buttonStateA == HIGH) {
        channel-=1;
        clipChannel();
        freqMode = FREQ_MODE_DECI;
        clearFreqDisplay();
      }
    }
  }
  lastButtonStateA = readingA;

  int readingB = digitalRead(buttonBpin);
  if (readingB != lastButtonStateB) {
    lastDebounceTimeB = millis();
  }

  if ((millis() - lastDebounceTimeB) > debounceDelayB) {
    if (readingB != buttonStateB) {
      buttonStateB = readingB;
      if (buttonStateB == HIGH) {
        channel+=1;
        clipChannel();
        freqMode = FREQ_MODE_DECI;
        clearFreqDisplay();
      }
    }
  }
  lastButtonStateB = readingB;

  // send data only when you receive data:
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    String serialOutput = "[";
    Serial.print(serialOutput + channel + "," + additionalChannel + "," + volumeEncoderPos + "," + channelsToString() + "]#");    
  }

  // update the display
  updateFreqDisplay();
  updateVolumeDisplay();  
  updateChannelDisplay();
}
