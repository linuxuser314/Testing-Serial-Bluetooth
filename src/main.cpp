#include "myiolib.h"


int main(void){
  initSerial9600();
  serialTransmitMsg('A', 20);
  serialTransmitMsg('Z', 90);
  serialTransmitMsg('S', 200);
  serialTransmitMsg('<', 36);
  while(true){}
}