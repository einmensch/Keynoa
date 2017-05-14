#include <Keyboard.h>
#define ofLayers 2//set the number of layers, the number has to match the number of entries in the first dimension in the matrices array
#define ofRows   4//set the number of rows, the number has to match the number of entries in the second dimension in the matrices array and the number of entries in the input array
#define ofColumns 5//set the number of columns, the number has to match the number of entries in the third dimension in the matrices array and the number of entries in the output array
#define ofMods   1//set the number of modifiers

int input[ofRows] = {10, 9, 8, 7};//put in the pin numbers you use for the input from the first Row to the last
int output[ofColumns] = {2, 3, 4, 5, 6};//put in the pins you use for the output from the left column to the right
String matrices[ofLayers][ofColumns][ofRows] = {{//this is the keyboard layout. put in the characters that you want to have and convert it to a byte. 
                                               //the first dimensionfunctions is the layer the second one the column and the third one the row
                                               //the buttons are send based on the Qwerty layout 
                                               //so if you use a different one on you computer you have to look which
                                               //button is at the position on the US layout where the button you want is on your layout
                                               //put String(char(0)) at the positions that you do not want to use or are in use by a modifyer
  { String(char(0)) , "&", "}", "/" },
  {"7" , "8", "9", "]" },
  {"4" , "5", "6",  String(char(0))  },
  {"1" , "2", "3","\n" },
  {"0" ,  String(char(0)) , ".",  String(char(0))  }
},{
  { String(char(0)) ,  "&", "}", "/" },
  {"7" , "8", "9", "]" },
  {"4" , "5", "6",  String(char(0))  },
  {"1" ,"2" , "3" ,")" },
  {"0" , String(char(0))  , "," , String(char(0))  }
}
};

byte modmatrices[ofLayers][ofColumns][ofRows] = {//put in the modifyernumbers here at the positions at wich you want them
  {
  {1,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  },
  {
  {1,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
  }
};

bool switchmatrix[ofColumns][ofRows]{
  {false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false}
  };//just fill this matrix with false

long layerprimecombo[ofLayers]={1,2};//put the number, that equals all the prime numbers that sould be active to trigger the layer multiplyed, here

//the number of the entrys in all the following arreys has to match the ofMods constant
byte modifyer[ofMods]={byte(1)};//the number you put in here is the one you have to put into the modmatrices to use the modifyer
long modifyerprime[ofMods]={2};// just put in as many prime numbers as you have modifyers
int modifyerpersistence[ofMods]={1};//set wich modifyers lock and wich are only active if you hold them
bool modifyerlocklight[ofMods]={true};
int modifyerlocklightpin[ofMods]={13};

//the part where you may have to make changes to make this work for your keyboard and change the keyboard layout ends here

byte tabmod = 1;
bool state = 0;
long currentlayer = 1;
long currentlayeractual = 0;
long persistentlayer = 1;
byte lastmod=100;
bool none =true;
void setup() {
  for(int i=0; i<ofMods;i++){
    pinMode(modifyerlocklightpin[i], OUTPUT);
  }
  //Serial.begin(9600);
  Keyboard.begin();
  for(int i=0;i < ofColumns; i++){
    pinMode(output[i], OUTPUT);}
  for(int i=0;i < ofRows; i++){
    pinMode(input[i], INPUT_PULLUP);}
  //        pinMode(input[4],INPUT_PULLUP);
  for(int i=0;i < ofColumns; i++){
    digitalWrite(output[i], HIGH);}
}

void checkbuttons(long layer) {
  for (int i = 0; i < ofColumns; i++) {
    //Serial.print("Micro");
    digitalWrite(output[i], LOW);
    for (int j = 0; j < ofRows; j++) {
      state = digitalRead(input[j]);
      if (state == LOW) {
        pushbutton(matrices[layer][i][j],true);
        delay(20);
        switchmatrix[i][j]=true;
      }
      else if(switchmatrix[i][j]==true){
        pushbutton(matrices[layer][i][j],false);
        switchmatrix[i][j]=false;
        if( tabmod != 1){
          persistentlayer /=tabmod;
          tabmod=1;
          }
        }
      }
    digitalWrite(output[i], HIGH);
  }
}

void pushbutton(String input,bool state){
  if(state){
    if (input.length()==1){
      Keyboard.press(input[0]);
    }
    else{
      for(int i =0; i<input.length(); i++){
        Keyboard.press(input[i]);
        Keyboard.release(input[i]);
      }
      delay(100);
    }
  }
  else{
    if(input.length()==1){
      Keyboard.release(input[0]);
    }
    else{
      return;
    }
  }
}

void loop(){
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
          if (modifyer[k] ==modmatrices[currentlayeractual][i][j] and modifyer[k]!= lastmod){
            
            if (modifyerpersistence[k]==0){
              currentlayer*=modifyerprime[k];
            }
            else if (persistentlayer%modifyerprime[k]!=0){
              if (modifyerpersistence[k] == 2){
                tabmod=modifyerprime[k];
              }
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
  checkbuttons(currentlayeractual);
}
