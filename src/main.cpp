#include "myiolib.h"


int main(void){
  initSerial9600();
  initTimer2Millis();
  myDelay(3000);

  while(true){
    msgStruct msgStructure = readSerialMsg();
    if(msgStructure.id != '\0'){
      serialTransmitMsg(msgStructure.id, msgStructure.val);
    }
  myDelay(100);
  }

}