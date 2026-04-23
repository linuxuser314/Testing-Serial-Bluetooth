#include "myiolib.h"


int main(void){
  initSerial9600();
  initTimer2Millis();
  myDelay(3000);

  while(true){
  serialTransmitMsg('A', 255);
  serialTransmitMsg('S', 215);
  serialTransmitMsg('x', 240);
  serialTransmitMsg('<', 36);
  myDelay(1000);
  }

}