#include "myiolib.h"

//This is just a testbench for my checksummed wireless serial communication.
//It just echoes back valid messages.

int main(void){
  initSerial9600();
  initTimer2Millis();
  myDelay(3000);

  while(true){
    msgStruct msgStructure = readSerialMsg();
    if(msgStructure.id != '\0'){
      serialTransmitMsg(msgStructure.id, msgStructure.val);
    }
  }

}