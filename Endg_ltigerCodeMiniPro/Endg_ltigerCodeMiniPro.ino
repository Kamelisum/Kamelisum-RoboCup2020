#include <Wire.h>
#include "Methoden.h"

// Setup, das einmal ausgefuehrt wird.
void setup()
{
  setupK();
  Wire.begin(18);                // join i2c bus with address #18
  Wire.onRequest(requestEvent); // register event
}

// Die Endlosschleife
void loop()
{
  einlesen(fd1,1);
  diodenWerte[0]=wertHellP;
  diodenWerte[1]=wertRotP;
  diodenWerte[2]=wertGruenP;
  diodenWerte[3]=wertBlauP;
  einlesen(fd2,2);
  diodenWerte[4]=wertHellP;
  diodenWerte[5]=wertRotP;
  diodenWerte[6]=wertGruenP;
  diodenWerte[7]=wertBlauP;
  einlesen(fd3,3);
  diodenWerte[8]=wertHellP;
  diodenWerte[9]=wertRotP;
  diodenWerte[10]=wertGruenP;
  diodenWerte[11]=wertBlauP;
  einlesen(fd4,4);
  diodenWerte[12]=wertHellP;
  diodenWerte[13]=wertRotP;
  diodenWerte[14]=wertGruenP;
  diodenWerte[15]=wertBlauP;
  for (int i=0; i<16; i++)
  {
    Serial.print(diodenWerte[i]);
    if (i%4 == 3) Serial.print("   ");
      else        Serial.print(" ");
  }
  Serial.println();
}

void requestEvent() {
  for (int i=0;i<16;i++)
  {
    sda[i]=char(diodenWerte[i]);
  }
  Wire.write(sda,16); // respond with message of 16 bytes
  // as expected by master
}

  
