#include "myiolib.h"


int main(void){
  initSerial9600();
  serialTransmitMsg('A', 66);
  while(true){}
}