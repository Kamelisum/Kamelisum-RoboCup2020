/*
      © 2020 by Simon Jan Plachta
      Zeilen: 398
*/

//Bibliotheken für das OLED-Display
#include <U8g2lib.h>
#include <Wire.h>
#include <Arduino.h>
#ifdef U8X8_HAVE_HW_SPI 
#include <SPI.h>
#endif
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

int loops = 0;                     //loop-Zähler
int stopwatch = 0;
unsigned long lastblack;
int vZeit = 0;                    // Verzoegerungszeit zum Testen
int messung, abstand;           //Variablungen für Messungen des Abstandes
int nDreh = 1500;                  //Zeit, die der Roboter für das Drehen um 90° braucht
int lichtWerte[16];                 //Lichtwerte der Sensoren
bool lookforRZ;                   //Zeigt an, ob dort ein "Krankenhaus" ist/war
int speedA;                     //Geschwindigkeit des Roboters zur Analyse
String prevColorL, prevColorR;  //Vorherige Lichtwerte der Seiten
int messung2, abstand2;           //Variablungen für Messungen des Abstandes
int diodenWerte[16];               //Empfangene Werte über den I²C Bus
int speedcontr1 = 9;              //Pin für die Geschwindigkeitskontrollierung
int speedcontr2 = 10;              //Pin für die Geschwindigkeitskontrollierung
int speedM = 70;                 // Normalgeschwindigkeit (65)
int speedW = 180;                 // Lenkwarscheinlichkeit (200)
int speedMO = 200;                //Normalgeschwindigkeit fuer das fahren nach oben (200)
int speedWO = 100;                   //Lenkgeschwindigkeit fuer das fahren nach oben
int speedMU = 50;                   //Normalgeschwindigkeit fuer das fahren nach unten
int speedWU = 50;                   //Lenkgeschwindigkeit fuer das fahren nach unten
int x = 0;                           //x-Neigungswert
int y = 0;                           //y-Neigungswert
int z = 0;                           //z-Neigungswert
int yoff = 0;                       //Offsetwert für die y-Neigung
int yW;                             // y-Wert, Aktueller - Offset

enum {BufSize = 3};
char buf[16][BufSize];

String sw;

char kurve = 'g';


void stopp()
{
  u8x8.drawString(7, 1, "stop()");
  delay(vZeit);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}



//Fahrmethoden
void vor()
{
  u8x8.drawString(7, 1, "vor()");
  delay(vZeit);
  digitalWrite(6, LOW);
  digitalWrite(5, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void links()
{
  u8x8.drawString(7, 1, "links()");
  delay(vZeit);
  digitalWrite(6, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void rechts()
{
  u8x8.drawString(7, 1, "rechts()");
  delay(vZeit);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void rueck()
{
  u8x8.drawString(7, 1, "rueck");
  delay(vZeit);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
}

void drehen() {
  u8x8.drawString(7, 1, "drehen()");
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(2000);
}

void Umfahren() {
  u8x8.drawString(7, 1, "A.Obs.");
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  links();
  delay(1000);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(3000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(1100);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(2000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(1000);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(3350);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  links();
  delay(500);
}

void oledDisplay()
{
  u8x8.drawString(7, 1, "oled");
  delay(vZeit);
  for (int i = 0; i < 16; i++)
  {
    snprintf (buf[i], BufSize, "%d", lichtWerte[i]);
  }
  u8x8.drawString(0, 3, "1");
  u8x8.drawString(0, 4, "2");
  u8x8.drawString(0, 5, "3");
  u8x8.drawString(0, 6, "4");
  u8x8.drawString(3, 3, buf[0]);
  u8x8.drawString(3, 4, buf[4]);
  u8x8.drawString(3, 5, buf[8]);
  u8x8.drawString(3, 6, buf[12]);
  u8x8.drawString(6, 3, buf[3]);
  u8x8.drawString(6, 4, buf[7]);
  u8x8.drawString(6, 5, buf[11]);
  u8x8.drawString(6, 6, buf[15]);
  u8x8.drawString(9, 3, buf[2]);
  u8x8.drawString(9, 4, buf[6]);
  u8x8.drawString(9, 5, buf[10]);
  u8x8.drawString(9, 6, buf[14]);
  u8x8.drawString(12, 3, buf[1]);
  u8x8.drawString(12, 4, buf[5]);
  u8x8.drawString(12, 5, buf[9]);
  u8x8.drawString(12, 6, buf[13]);
  u8x8.drawString(3, 7, "WH");
  u8x8.drawString(6, 7, "Ro");
  u8x8.drawString(9, 7, "Gr");
  u8x8.drawString(12, 7, "Bl");
  enum {BufSize1 = 4};
  char yStr[BufSize1];
  snprintf (yStr, BufSize1, "%03d", yW);
  u8x8.drawString(0, 2, "y:");
  u8x8.drawString(4, 2, yStr);
}





void wendenRZR1(){   //Drehung verschiebt Roboter um keine Bahn
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(1000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh*2);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(1000);
}
void wendenRZR5(){   //Drehung verschiebt Roboter um halbe Bahn
      analogWrite(speedcontr1, speedM);
      analogWrite(speedcontr2, speedM);
  rueck();
  delay(1000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(1111);       //Bis der Roboter die  halbe Breite einer Runde gefahren ist
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  rueck();
  delay(1000);
}
void wendenRZ(){     //Drehung verschiebt Roboter um eine Bahnrueck();
  delay(1000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(2222);       //Bis der Roboter die Breite einer Runde gefahren ist
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(1000);
}
void entladen(){     //Kugeln werden entladen
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(2000);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(2000);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(5000);
  stopp();
}
void lookIfRZ(){     //Es wird geprüft, ob hier ein "Krankenhaus" ist
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(1000);
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(1111);       //Bis der Roboter die halbe Breite einer Runde gefahren ist
  if((lichtWerte[0] < 75 && lichtWerte[4] < 75 && lichtWerte[8] < 75 && lichtWerte[12] < 75)){
    lookforRZ = true;  //Falls das "Krankenhaus" hier in der Ecke ist, wird die Variable auf true gesetzt
  }
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  rueck();
  delay(1111);       //Bis der Roboter die halbe Breite einer Runde gefahren ist
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  links();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(1000);
}






//Methode für die Rote Zone
/*void RoteZone(){
  const int whileToLoop = 2; //Diese Variable stellt sicher, dass die while-Schleife endlos läuft
  int Runden = 0;                //Zählt wieviele Runden der Roboter gefahren ist
  int blackPos = 0;             //Position des "Krankenhauses"
  
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  delay(1000);                 //Noch anpassen
  analogWrite(speedcontr1, speedW);
  analogWrite(speedcontr2, speedW);
  rechts();
  delay(nDreh);
  analogWrite(speedcontr1, speedM);
  analogWrite(speedcontr2, speedM);
  vor();
  if(RZTasterW == 0){
    if(lichtWerte[0] < 75 && lichtWerte[4] < 75 && lichtWerte[8] < 75 && lichtWerte[12] < 75){
      blackPos = 1;  //Falls das "Krankenhaus" hier unten ist, wird die Variable auf 1 gesetzt
    }
    wendenRZR1();                //Wenden und 1ste Reihe wiederholt abfahren
  }
  while(whileToLoop == 2){  //Loop-Schleife
    analogWrite(speedcontr1, speedM);
    analogWrite(speedcontr2, speedM);
    vor();
    if(RZTasterW == 0){
      if((lichtWerte[0] < 75 && lichtWerte[4] < 75 && lichtWerte[8] < 75 && lichtWerte[12] < 75) && Runden == 0){
      blackPos = 2;  //Falls das "Krankenhaus" hier unten ist, wird die Variable auf 2 gesetzt
    }
      wendenRZ();          //Wenden
      Runden++;            //Inkrementiert die Anzahl der Runden
    }
    if(Runden == 4){
      lookIfRZ();          //Gucken, ob hier das "Krankenhaus" ist
      wendenRZR5();         //Drehung halbe Bahn
      if(lookforRZ == true){  //Falls das "Krankenhaus" hier ist...
        blackPos = 5;         //Falls das "Krankenhaus" hier oben ist, wird die Variable auf 5 gesetzt
      }
      else{                       //Falls das "Krankenhaus" hier nicht ist...
        if(blackPos == 0){        //Und auch sonst nirgendswo...
          analogWrite(speedcontr1, speedM);
          analogWrite(speedcontr2, speedM);
          vor();                  //Fahre vorwärts...
          if(RZTasterW == 0){
            entladen();           //Und wenn du angekommen bist entlade.
          }
        }
        if(blackPos == 5){       //Falls das "Krankenhaus" auf Pos. 5 ist,...
          analogWrite(speedcontr1, speedM);
          analogWrite(speedcontr2, speedM);
          vor();                 //Fahre vor
          if(RZTasterW == 0){
            wendenRZR1();       //Wende auf der Stelle
          }
          analogWrite(speedcontr1, speedM);
          analogWrite(speedcontr2, speedM);
          vor();
          if(RZTasterW == 0){
            entladen();          //Wenn du angekommen bist entlade
          }
        }
        
        else if(blackPos == 2){       //Falls das "Krankenhaus" auf Pos. 2 ist,...
          analogWrite(speedcontr1, speedM);
          analogWrite(speedcontr2, speedM);
          vor();                      //Fahre vor
          if(RZTasterW == 0){
            analogWrite(speedcontr1, speedW);
            analogWrite(speedcontr2, speedW);
            rechts();                 //Fahre nach unten
            delay(nDreh);
            analogWrite(speedcontr1, speedM);
            analogWrite(speedcontr2, speedM);
            vor();
            if(RZTasterW == 0){
              entladen();            //Und entlade.
            }
          }
        }
        else if(blackPos == 1){       //Falls das "Krankenhaus" auf Pos. 2 ist,...
          analogWrite(speedcontr1, speedM);
          analogWrite(speedcontr2, speedM);
          vor();                      //Fahre vor
          if(RZTasterW == 0){
            analogWrite(speedcontr1, speedW);
            analogWrite(speedcontr2, speedW);
            rechts();                 //Fahre nach unten
            delay(nDreh);
            analogWrite(speedcontr1, speedM);
            analogWrite(speedcontr2, speedM);
            vor();
            if(RZTasterW == 0){
              analogWrite(speedcontr1, speedW);
              analogWrite(speedcontr2, speedW);
              rechts();               //Und nach rechts
              delay(nDreh);
              analogWrite(speedcontr1, speedM);
              analogWrite(speedcontr2, speedM);
              vor();
              if(RZTasterW == 0){
                entladen();          //Wenn du angekommen bist entlade.
              }
            }
          }
        }
      }
    }
  }
}*/
