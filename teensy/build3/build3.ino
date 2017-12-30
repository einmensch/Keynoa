#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;
#define ofLayers 1//set the number of layers, the number has to match the number of entries in the first dimension in the matrices array
#define ofRows   5//set the number of rows, the number has to match the number of entries in the second dimension in the matrices array and the number of entries in the input array
#define ofColumns 5//set the number of columns, the number has to match the number of entries in the third dimension in the matrices array and the number of entries in the output array
#define ofMods  0 //set the number of modifiers
#define ofVirtualRows 5
#define ofVirtualColumns 3

//int output[ofRows] = {0, 1, 2, 3, 4};//put in the pin numbers you use for the input from the highest Row to the lowest
//int input[ofColumns] = {21, 20, 19, 18, 17};//put in the pins you use for the output from the left column to the right

int input[ofRows] = {0, 1, 2, 3, 4};//put in the pin numbers you use for the input from the highest Row to the lowest
int output[ofColumns] = {21, 20, 19, 18, 17};//put in the pins you use for the output from the left column to the right

int presscount=1;
int modpresscount=1;

int controllerInput[ofColumns]={0,0,0,0,0};

 unsigned int VirtualMatrices[ofLayers][ofVirtualColumns][ofVirtualRows] = {{//this is the keyboard layout. put in the characters that you want to have and convert it to a byte. 
                                               //the first dimension is the layer the second one the column and the third one the row
                                               //the buttons are send based on the Qwerty layout 
                                               //so if you use a different one on you computer you have to look which
                                               //button is at the position on the US layout where the button you want is on your layout
  {0   ,KEY_B, KEY_Q, KEY_W,KEY_E},
  {KEY_E,KEY_A, KEY_A, KEY_S,KEY_D},
  {0    ,0    , KEY_K, KEY_L,KEY_SPACE}//,
  //{KEY_M,KEY_N,KEY_O, KEY_P,KEY_4},
  //{KEY_Q,KEY_R, KEY_S,KEY_T,KEY_5}  
}
};

int VirtualModMatrices[ofLayers][ofVirtualColumns][ofVirtualRows]{{
  {-1 ,0 ,0 ,0 ,0},
  { 0 ,0 ,0 ,0 ,0},
  { MODIFIERKEY_SHIFT ,MODIFIERKEY_ALT ,0 ,0 ,0},
}};


/*  {-2,KEY_B, KEY_C, KEY_D,KEY_1},
  {KEY_E,KEY_F, KEY_G, KEY_H,KEY_2},
  {MODIFIERKEY_SHIFT,MODIFIERKEY_ALT, MODIFIERKEY_CTRL, KEY_L,KEY_3}//,*/

unsigned short int TranslateMatrices[ofVirtualColumns][ofVirtualRows][2]{
    {{2,0},{2,1},{0,3},{1,0},{1,1}},
    {{3,0},{3,1},{0,4},{1,2},{0,1}},
    {{4,0},{4,1},{1,4},{0,2},{0,0}}
};


long layerprimecombo[ofLayers]={1};//put the number, that equals all the prime numbers that sould be active to trigger the layer multiplyed, here

//the number of the entrys in all the following arreys has to match the ofMods constant
byte modifyer[ofMods]={};//the symbol you put here should be one you don't use on your keyboard if you don't have any unused characters you can use a number not mapped by ascii
                                  //and the arduino libary form 0 to 255. put the same number/symbol into your layout at the poin in which you want to have the modifyer
long modifyerprime[ofMods]={};// just put in as many prime numbers as you have modifyers
bool modifyerpersistence[ofMods]={};//set wich modifyers lock and wich are only active if you hold them
bool modifyerlocklight[ofMods]={};
int modifyerlocklightpin[ofMods]={};

String macros[3] ={"#1","#2","#3"};

//the part where you may have to make changes to make this work for your keyboard and change the keyboard layout ends here
 short int presend[7]={0,0,0,0,0,0,0};
 unsigned int matrices[ofLayers][ofColumns][ofRows];
 int modMatrices[ofLayers][ofColumns][ofRows];
bool state = 0;
long currentlayer = 1;
long currentlayeractual = 0;
long persistentlayer = 1;
byte lastmod=100;
bool none =true;

unsigned long time1 =0;
unsigned long timealt=0;
unsigned long timelong=0;
short int lastpresend[6]={0,0,0,0,0,0};


void setup() {
  mcp.begin();
  Serial.begin(9600);
  
  //Serial.println(VirtualMatrices[0][0][0]);
  
  for(int i=0; i< ofLayers;i++){
    for(int j=0; j< ofVirtualColumns; j++){
      for(int k=0; k< ofVirtualRows;k++){
        matrices[i][TranslateMatrices[j][k][0]][TranslateMatrices[j][k][1]]=VirtualMatrices[i][j][k];
      }
    }
  }

  for(int i=0; i< ofLayers;i++){
    for(int j=0; j< ofVirtualColumns; j++){
      for(int k=0; k< ofVirtualRows;k++){
        modMatrices[i][TranslateMatrices[j][k][0]][TranslateMatrices[j][k][1]]=VirtualModMatrices[i][j][k];
      }
    }
  }
  
    
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


  //Serial.print(String(matrices[1][1][2]));
}


void checkbuttons( unsigned int matrix[ofColumns][ofRows], int z) {
  for (int i = 0; i < ofColumns; i++) {
    //Serial.print("Micro");
    digitalWrite(output[i], LOW);
    for (int j = 0; j < ofRows; j++) {
      if (controllerInput[j]==0){
        state = digitalRead(input[j]);}
      //else{
      //  state=mcp.digitalRead(input[j]);
      //}
      if (state == LOW) {
        Smash(matrix[i][j], z , i , j);
        
      }
    }
    digitalWrite(output[i], HIGH);
  }
}


void loop(){

  //////////////////////

  ///////////////////////////

////////////////////////////
  
////////////////////////////
  presscount=1;
  modpresscount=1;
  for(int i=0;i<7;i++){
    presend[i+1]=0;}
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

  //Serial.print(currentlayeractual);
  

  checkbuttons(matrices[currentlayeractual], currentlayeractual);

    sendshit(presend);
    Keyboard.send_now();
}


void sendshit( short int list[10]){
  Keyboard.set_key1(list[1]);
  Keyboard.set_key2(list[2]);
  Keyboard.set_key3(list[3]);
  Keyboard.set_key4(list[4]);
  Keyboard.set_key5(list[5]);
  Keyboard.set_key6(list[6]);
  Keyboard.set_modifier(list[7]);
  //Serial.println(String(list[1])+"  "+String(list[2])+"  "+String(list[3])+"  "+String(list[4])+"  "+String(list[5])+"  "+String(list[6])+"  "+String(list[7]) );
}


void Smash( unsigned short toSmash, int z , int x , int y){
  
  if(toSmash == 0){
    //Serial.print(String(modMatrices[z][x][y]));
    //Serial.println(modMatrices[z][x][y]);
    if(modMatrices[z][x][y]<=0){
      
      if(timelong>200){
        Keyboard.print(macros[-modMatrices[z][x][y]]);
        time1 = millis();
      }
      timealt = millis();
      timelong=timealt-time1;
      
      Serial.println(timelong);
    }
    
    else if(modMatrices[z][x][y] <= 57472  && modMatrices[z][x][y] >= 57345){
        presend[7]=modMatrices[z][x][y] | presend[7];
        //Serial.print(toSmash);
    }
  }
  else if(presscount==1){
    presend[1]=toSmash;
    presscount++;
  }
  else if(presscount==2){
    presend[2]=toSmash;
    presscount++;
  }
  else if(presscount==3){
    presend[3]=toSmash;
    presscount++;
  }
  else if(presscount==4){
    presend[4]=toSmash;
    presscount++;
  }
  else if(presscount==5){
    presend[5]=toSmash;
    presscount++;
  }
  else if(presscount==6){
    presend[6]=toSmash;
    presscount++;
  }
  
}
//void Smash( short toSmash){
//  Keyboard.print(toSmash);
//}





