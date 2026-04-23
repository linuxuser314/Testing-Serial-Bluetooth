#include "myiolib.h"


int main(void){
  initSerial9600();
  serialTransmitMsg('A', 255);
  serialTransmitMsg('S', 215);
  serialTransmitMsg('x', 240);
  serialTransmitMsg('<', 36);
  while(true){}
}