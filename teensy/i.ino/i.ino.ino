#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;
#define ofLayers 2//set the number of layers, the number has to match the number of entries in the first dimension in the matrices array
#define ofRows   4//set the number of rows, the number has to match the number of entries in the second dimension in the matrices array and the number of entries in the input array
#define ofColumns 5//set the number of columns, the number has to match the number of entries in the third dimension in the matrices array and the number of entries in the output array
#define ofMods   1//set the number of modifiers

int input[ofRows] = {10, 9, 8, 7};//put in the pin numbers you use for the input from the highest Row to the lowest
int output[ofColumns] = {2, 3, 4, 5, 6};//put in the pins you use for the output from the left column to the right

int presscount=1;

int controllerInput[ofColumns]={0,0,0,0};

unsigned short matrices[ofLayers][ofColumns][ofRows] = {{//this is the keyboard layout. put in the characters that you want to have and convert it to a byte. 
                                               //the first dimension is the layer the second one the column and the third one the row
                                               //the buttons are send based on the Qwerty layout 
                                               //so if you use a different one on you computer you have to look which
                                               //button is at the position on the US layout where the button you want is on your layout
  {byte(1) , byte('&'), byte('}'), byte('/') }, //L2  /   *   -
  {KEY_A , byte('8'), byte('9'), byte(']') }, //7   8   9   +
  {byte('4') , byte('5'), byte('6'),      0    }, //4   5   6
  {byte('1') , byte('2'), byte('3'), byte('\n')}, //1   2   3   Enter
  {byte('0') ,     0    , byte('.'),      0    }  //0       .
},{
  {byte(1) , byte('&'), byte('}'), byte('/') }, //L2 !F2!!Tab! -
  {byte('7') , byte('8'), byte('9'), byte(']') }, //7   8    9   +
  {byte('4') , byte('5'), byte('6'),      0    }, //6   5    6
  {byte('1') , byte('2'), byte('3'), byte(')') }, //1   2    3   =   )
  {byte('0') ,     0    , byte(','),      0    }  //0        ,
}
};

long layerprimecombo[ofLayers]={1,2};//put the number, that equals all the prime numbers that sould be active to trigger the layer multiplyed, here

//the number of the entrys in all the following arreys has to match the ofMods constant
byte modifyer[ofMods]={byte(1)};//the symbol you put here should be one you don't use on your keyboard if you don't have any unused characters you can use a number not mapped by ascii
                                  //and the arduino libary form 0 to 255. put the same number/symbol into your layout at the poin in which you want to have the modifyer
long modifyerprime[ofMods]={2};// just put in as many prime numbers as you have modifyers
bool modifyerpersistence[ofMods]={false};//set wich modifyers lock and wich are only active if you hold them
bool modifyerlocklight[ofMods]={true};
int modifyerlocklightpin[ofMods]={13};

//the part where you may have to make changes to make this work for your keyboard and change the keyboard layout ends here


bool state = 0;
long currentlayer = 1;
long currentlayeractual = 0;
long persistentlayer = 1;
byte lastmod=100;
bool none =true;
void setup() {
  mcp.begin();
  for(int i=0; i<ofMods;i++){
    pinMode(modifyerlocklightpin[i], OUTPUT);
  }
  for(int i=0;i < ofColumns; i++){
    if (controllerInput[i]==0){
      pinMode(output[i], OUTPUT);}
    else{
      mcp.pinMode(output[i], OUTPUT);
    }
  }
  for(int i=0;i < ofRows; i++){
    if (controllerInput[i]==0){
      pinMode(input[i], INPUT_PULLUP);}
    else{
      mcp.pinMode(input[i], INPUT_PULLUP);
    }
    }
  for(int i=0;i < ofColumns; i++){
    if (controllerInput[i]==0){
      digitalWrite(output[i], HIGH);}
    else{
      mcp.digitalWrite(output[i], HIGH);
    }
  }
}



void loop(){
  presscount=1;
  none=true;
  currentlayer=persistentlayer;
  for (int i = 0; i < ofColumns; i++) {
    //Serial.print("Micro");
    digitalWrite(output[i], LOW);
    for (int j = 0; j < ofRows; j++) {
      state = digitalRead(input[j]);
      if (state == LOW) {
        none=false;
        for(int k=0; k< ofMods; k++){
          if (modifyer[k] ==matrices[currentlayeractual][i][j] and modifyer[k]!= lastmod){
            
            if (modifyerpersistence[k]==false){
              currentlayer*=modifyerprime[k];
            }
            else if (persistentlayer%modifyerprime[k]!=0){
              lastmod=modifyer[k];
              currentlayer*=modifyerprime[k];
              persistentlayer*=modifyerprime[k];
              if (modifyerlocklight[k]==true){
                  digitalWrite(modifyerlocklightpin[k],HIGH);
              }
            }
            else{
              lastmod=modifyer[k];
              persistentlayer/=modifyerprime[k];
              if (modifyerlocklight[k]==true){
                digitalWrite(modifyerlocklightpin[k],LOW);
              }
            }
          }
        }
      }
      if (none==true){
          lastmod=100;
        }
    }
    digitalWrite(output[i], HIGH);
  }
  for(int i; i<ofLayers; i++){
    if (layerprimecombo[i]==currentlayer){
      currentlayeractual=i;
      break;
    }
  }
  Serial.print(currentlayeractual);
  checkbuttons(matrices[currentlayeractual]);
}



void Smash(unsigned short toSmash){
  if(presscount==1){
    Keyboard.set_key1(toSmash);
  }
  else if(presscount==2){
    Keyboard.set_key2(toSmash);
  }
  else if(presscount==3){
    Keyboard.set_key3(toSmash);
  }
  else if(presscount==4){
    Keyboard.set_key4(toSmash);
  }
  else if(presscount==5){
    Keyboard.set_key5(toSmash);
  }
  else if(presscount==6){
    Keyboard.set_key6(toSmash);
  }
  presscount++;
}



void checkbuttons(unsigned short matrix[5][4]) {
  for (int i = 0; i < ofColumns; i++) {
    //Serial.print("Micro");
    if(i<=8){
      mcp.digitalWrite(output[i], LOW);
    }
    digitalWrite(output[i], LOW);
    for (int j = 0; j < ofRows; j++) {
      if (controllerInput[j]==0){
        state = digitalRead(input[j]);}
      else{
        state=mcp.digitalRead(input[j]);
      }
      if (state == LOW) {
        Smash(matrix[i][j]);
        delay(20);
      }
    }
    if(i<=8){
      mcp.digitalWrite(output[i], HIGH);
    }
    digitalWrite(output[i], HIGH);
  }
}


