#include <ui.h>
#include <buffer.h>
#include <vs1053b_patches.h>
#include <vs10xx.h>
#include <player.h>
#include <venc44k2q05.h>
#include <config.h>
#include <mmc.h>
#include <vs1053b_patches_flac.h>
#include <filesys.h>
#include <record.h>
#include <storage.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);//pin2-Rx,pin3-Tx(note: pin3 is actually later used as volume down input)
unsigned char card[100];

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello test!");
  mySerial.begin(9600);// used for receiving command data from the iPod dock.
  
  InitSPI();
  Serial.println("Init SPI");
  
  InitIOForVs10xx();
  Serial.println("Init IO");

  InitIOForKeys();
  Serial.println("Init IO keys");
  
  InitIOForLEDs();
  Serial.println("Init IO LED");

  InitFileSystem();
  Serial.println("InitFS");

  Mp3Reset();
  Serial.println("MP3");  
  Mp3SetVolume(0,0); //max volume
}

long lastplayed = 0;

void loop(){
    int r = readCard();
    if(r==1) {
        decode();
        currentFile = findSong() ;
        AvailableProcessorTime();
        OpenFile(currentFile);
      
        PlayCurrentFile();
        Mp3SoftReset();    
  
  }
  
 
}


int readCard(){
    if (mySerial.available()>0){
         unsigned char i=0; 
         if(mySerial.read()==2){
             while(1){
               if(mySerial.available()) {           
                  char c=mySerial.read();
                  if(c==3) {
                    return 1;
                    break;
                  } else{
                     card[i] = c;
                     i++;
                     if(i==20) return 2; //error
                  }
               }
             }
         }
       } else{
         return 0;
       }
} 

void decode(){
  unsigned char p;
  for(p=0;p<11;p++){
      char c = card[p];
      Serial.print(c);
      Serial.print(",");
  }
  Serial.println();
}

int findSong(){
  int r = 3;
  if(compare("0000A371429")==1) r=1;//bird
  if(compare("0000A33126B")==1) r=2; //dog
  if(compare("0000A323CF4")==1) r=3; //hen
  if(compare("0000A32A7BF")==1) r=4; //sheep
  if(compare("0000A35B1AE")==1) r=5; //rooster
  

  return r;
  
}


int compare(const char* p){
   int r = 1;
   for(int i=0; i< 11; i++){
      if(card[i] == *p) p++;
      else {
        r = 0;
        break;
      }
   }
   return r;
}
