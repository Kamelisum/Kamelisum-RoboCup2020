/*
      © 2020 by Simon Jan Plachta
      Zeilen: 426
*/

//Importieren der Header-Dateien
#include "einigeMethoden.h"
//Bibliotheken für das OLED-Display
#include <U8g2lib.h>
#include <Wire.h>
#include <Arduino.h>
#ifdef U8X8_HAVE_HW_SPI 
#include <SPI.h>
#endif
//Pins der Ultraschallsensoren
#define sender 3
#define empfaenger 2
#define sender2 11
#define empfaenger2 4


void Gyro() {
  u8x8.drawString(7, 1, "Gyro");
  delay(vZeit);
  x = analogRead(A0);
  Serial.print("X: ");
  Serial.print(x);
  Serial.print("   ");

  y = analogRead(A1);
  Serial.print("Y: ");
  Serial.print(y);
  Serial.print("   ");

  z = analogRead(A2);
  Serial.print("Z: ");
  Serial.println(z);
}

void Abstand() {
  u8x8.drawString(7, 1, "Abst1");
  delay(vZeit);
  digitalWrite(sender, HIGH);                 // Ein Ultraschallsignal wird gesendet.
  delayMicroseconds(100);                     // Er macht es 100ms=0,1s.
  digitalWrite(sender, LOW);                  // Das Ultraschallsignal wird beendet.
  messung = pulseIn(empfaenger, HIGH, 5000);        // Laufzeitmessung
  abstand = messung / 57, 7;                  // Umrechnung in cm
  enum {BufSize = 3};
  char buf1[BufSize];
  snprintf (buf1, BufSize, "%02d", abstand);
  u8x8.drawString(0, 0, "AU:");
  u8x8.drawString(4, 0, buf1);
  Serial.print("A1: ");
  Serial.print(buf1);
  Serial.print(" cm  ");
}

void Abstand2() {
  u8x8.drawString(7, 1, "Abst2");
  delay(vZeit);
  digitalWrite(sender2, HIGH);                 // Ein Ultraschallsignal wird gesendet.
  delayMicroseconds(100);                     // Er macht es 100ms=0,1s.
  digitalWrite(sender2, LOW);                  // Das Ultraschallsignal wird beendet.
  messung2 = pulseIn(empfaenger2, HIGH, 5000);        // Laufzeitmessung
  abstand2 = messung2 / 57, 7;                  // Umrechnung in cm
  enum {BufSize2 = 3};
  char Abs2[BufSize2];
  snprintf (Abs2, BufSize2, "%02d", abstand2);
  u8x8.drawString(7, 0, "AO:");
  u8x8.drawString(10, 0, Abs2);
  Serial.print("A2: ");
  Serial.print(Abs2);                      // Ausgabe auf den Monitor
  Serial.print(" cm ");
}



void getWerte(bool ultraschall) {
  u8x8.drawString(7, 1, "getW");
  delay(vZeit);
  for (int j = 0; j < 16; j++) lichtWerte[j] = -1;
  int i = 0;
  Serial.print("How many were available?");
  Wire.requestFrom(18, 16);    // request 16 bytes from slave device #18
  Serial.print("request done ");
  Serial.print(Wire.available());         // print number of available bytes;
  Serial.print(" |");
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    /* if (i>15) {
       Serial.println("Das darf nicht passieren!!!!!");
       i=15;
      }*/
    lichtWerte[i] = c + 0;
    Serial.print(/*lichtWerte*/buf[i]);         // print the characters
    i++;
    Serial.print(" ");
    if(i == 4 || i == 8 || i == 12){Serial.print("   ");}
  }
  Gyro();
  if(ultraschall == true){
    Abstand();
    Serial.print(" ");
    Abstand2();
  }
  else{
    Serial.print("                      ");
  }
}


// Ende Methoden


// Setup, das einmal ausgefuehrt wird.
void setup()
{

  Wire.begin();        // join i2c bus (address optional for master)

  Serial.begin(115200);  // start serial for output
  for (int i = 0; i < 16; i++)  lichtWerte[i] = -2;
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.setPowerSave(0);
  u8x8.drawString(7, 1, "setup");
  delay(vZeit);
  links();
  delay(100);
  rechts();
  delay(100);
  pinMode(speedcontr1, OUTPUT);
  pinMode(speedcontr2, OUTPUT);
  pinMode(sender, OUTPUT);                    // Digital 3 ist ein Sender (PWM).
  pinMode(empfaenger, INPUT);                 // Digital 2 ist ein Empfänger.
  pinMode(sender2, OUTPUT);                    // Digital 9 ist ein Sender (PWM).
  pinMode(empfaenger2, INPUT);                 // Digital 10 ist ein Empfänger.
  analogWrite(speedcontr1, 0);
  analogWrite(speedcontr2, 0);
  Gyro();
  yoff = y;
  Abstand();
  Abstand2();
  }


// Die Endlosschleife
void loop()
{
  stopwatch = (millis()-lastblack)/1000;
 /*Serial.print("    In loop now for ");
  Serial.print(loops);
  Serial.print("th time now    ");*/
  enum {BufSize2 = 3};
  char strwatch[4];
  snprintf (strwatch, 4, "S%02d", stopwatch);
  u8x8.drawString(0,1,strwatch);
  //Werte auf dem OLED-Display
  enum {BufSize = 3};
  char speedAStr[BufSize];
  snprintf (speedAStr, BufSize, "%02d", speedM);
  u8x8.drawString(10, 2, speedAStr);
  u8x8.drawString(7, 1, "loop");
  delay(vZeit);
  getWerte(false);
  yW = y - yoff;
  //Serial.println("getWerte done");
  oledDisplay();
  //Serial.println("oledDisplay done");

  //Abfrage der Lichtwerte
  if ((lichtWerte[0] >= 88 || lichtWerte[4] >= 88) && (lichtWerte[8] >= 88 || lichtWerte[12] >= 88)) {
    //Serial.println("inForward");
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedM);
      analogWrite(speedcontr2, speedM);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedMU);
      analogWrite(speedcontr2, speedMU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedMO);
      analogWrite(speedcontr2, speedMO);
    }
    speedA = speedM;
    vor();
    getWerte(true);
    prevColorR = "white";
    prevColorL = "white";
  }

  // Linie Fahren rechts
  while ((lichtWerte[0] <= 87 /*&&  lichtWerte[3] >= 40 && lichtWerte[3] <= 60 && lichtWerte[2] >= 30 && lichtWerte[2] <= 45 && lichtWerte[1] >= 10 && lichtWerte[1] <= 25*/)
         /*|| (lichtWerte[4] <= 85*/ /*&&  lichtWerte[7] >= 40 && lichtWerte[7] <= 60 && lichtWerte[6] >= 30 && lichtWerte[6] <= 45 && lichtWerte[5] >= 10 && lichtWerte[5] <= 25)*/) {
    //Serial.println("inRight");
    getWerte(false);
    oledDisplay();
    stopp();
    delay(10);
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedW);
      analogWrite(speedcontr2, speedW);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedWU);
      analogWrite(speedcontr2, speedWU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedWO);
      analogWrite(speedcontr2, speedWO);
    }
    speedA = speedW;
    rechts();
    lastblack=millis();
    prevColorR = "black";
    kurve = 'r';
  }
  if(kurve == 'r'){
    /*stopp();
    delay(200);*/
    rechts();
    delay(500);
    kurve = 'g';
    rueck();
    delay(150);
  }
  kurve = 'g';

  // Linie Fahren rechts
  while /*((lichtWerte[0] <= 89*/ /*&&  lichtWerte[3] >= 40 && lichtWerte[3] <= 60 && lichtWerte[2] >= 30 && lichtWerte[2] <= 45 && lichtWerte[1] >= 10 && lichtWerte[1] <= 25)*/
         /*||*/ (lichtWerte[4] <= 87 /*&&  lichtWerte[7] >= 40 && lichtWerte[7] <= 60 && lichtWerte[6] >= 30 && lichtWerte[6] <= 45 && lichtWerte[5] >= 10 && lichtWerte[5] <= 25)*/) {
    //Serial.println("inRight");
    getWerte(false);
    oledDisplay();
    stopp();
    delay(10);
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedW);
      analogWrite(speedcontr2, speedW);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedWU);
      analogWrite(speedcontr2, speedWU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedWO);
      analogWrite(speedcontr2, speedWO);
    }
    speedA = speedW;
    rechts();
    lastblack=millis();
    prevColorR = "black";
    kurve = 'R';
  }
  if(kurve == 'R'){
    /*stopp();
    delay(200);*/
    rechts();
    delay(150);
    kurve = 'g';
    rueck();
    delay(50);
  }
  kurve = 'g';


  
  // Linie fahren links
  while (/*(lichtWerte[8] <= 89*/ /*&&  lichtWerte[11] >= 40 && lichtWerte[11] <= 60 && lichtWerte[10] >= 30 && lichtWerte[10] <= 45 && lichtWerte[9] >= 10 && lichtWerte[9] <= 25)*/
        /*||*/  (lichtWerte[12] <= 87 /*&&  lichtWerte[15] >= 40 && lichtWerte[15] <= 60 && lichtWerte[14] >= 30 && lichtWerte[14] <= 45 && lichtWerte[13] >= 10 && lichtWerte[13] <= 25*/)) {
    //Serial.println("inLeft");
    getWerte(false);
    oledDisplay();
    stopp();
    delay(10);
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedW);
      analogWrite(speedcontr2, speedW);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedWU);
      analogWrite(speedcontr2, speedWU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedWO);
      analogWrite(speedcontr2, speedWO);
    }
    speedA = speedW;
    links();
    lastblack=millis();
    prevColorL = "black";
    kurve = 'l';
  }
  if(kurve == 'l'){
    /*stopp();
    delay(200);*/
    links();
    delay(500);
    kurve = 'g';
    rueck();
    delay(150);
  }
  kurve = 'g';

// Linie Fahren rechts
  while /*((lichtWerte[0] <= 89*/ /*&&  lichtWerte[3] >= 40 && lichtWerte[3] <= 60 && lichtWerte[2] >= 30 && lichtWerte[2] <= 45 && lichtWerte[1] >= 10 && lichtWerte[1] <= 25)*/
         /*||*/ (lichtWerte[8] <= 87 /*&&  lichtWerte[7] >= 40 && lichtWerte[7] <= 60 && lichtWerte[6] >= 30 && lichtWerte[6] <= 45 && lichtWerte[5] >= 10 && lichtWerte[5] <= 25)*/) {
    //Serial.println("inRight");
    getWerte(false);
    oledDisplay();
    stopp();
    delay(10);
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedW);
      analogWrite(speedcontr2, speedW);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedWU);
      analogWrite(speedcontr2, speedWU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedWO);
      analogWrite(speedcontr2, speedWO);
    }
    speedA = speedW;
    rechts();
    lastblack=millis();
    prevColorR = "black";
    kurve = 'L';
  }
  if(kurve == 'L'){
    /*stopp();
    delay(200);*/
    links();
    delay(150);
    kurve = 'g';
    rueck();
    delay(50);
  }
  kurve = 'g';

  

  if ((lichtWerte[0] >= 88 || lichtWerte[4] >= 88) && (lichtWerte[8] >= 88 || lichtWerte[12] >= 88)) {
    //Serial.println("inForward");
    if (yW > -10 && yW < 15) {
      analogWrite(speedcontr1, speedM);
      analogWrite(speedcontr2, speedM);
    }
    if (yW < -10) {
      analogWrite(speedcontr1, speedMU);
      analogWrite(speedcontr2, speedMU);
    }
    if (yW > 15) {
      analogWrite(speedcontr1, speedMO);
      analogWrite(speedcontr2, speedMO);
    }
    speedA = speedM;
    vor();
    getWerte(true);
    prevColorR = "white";
    prevColorL = "white";
  }
  if ((abstand < 10 && abstand != 0) && (abstand2 < 10 && abstand2 != 0)) {
    Serial.println("seeObstacle");
    Umfahren();
    getWerte(true);
  }
  if((lichtWerte[0] <=87 || lichtWerte[12] <= 87) && (lichtWerte[0] <=87 || lichtWerte[12] <= 87)){
    analogWrite(speedcontr1, speedW);
    analogWrite(speedcontr2, speedW);
    links(); 
    delay(1500);
  }

  
  /*
  //Erkennen der grünen Punkte
  if ( (lichtWerte[0] <= 70 &&  lichtWerte[3] >= 20 && lichtWerte[3] <= 40 && lichtWerte[2] >= 40 && lichtWerte[2] <= 60 && lichtWerte[1] >= 15 && lichtWerte[1] <= 25)
       || (lichtWerte[4] <= 70 &&  lichtWerte[7] >= 20 && lichtWerte[7] <= 40 && lichtWerte[6] >= 40 && lichtWerte[6] <= 60 && lichtWerte[5] >= 15 && lichtWerte[5] <= 25))
  {
    if (prevColorL.equals("white") && prevColorR.equals("white")) {
      Serial.println("GreenAtRight");
      u8x8.drawString(0, 1, "Green at right");
      while (lichtWerte[8] >= 80) {
        rechts();
        getWerte(true);
      }
      u8x8.drawString(0, 1, "                  ");
    }
    else {
      vor();
      delay(500);
    }
  }

  if ((lichtWerte[8] <= 70 &&  lichtWerte[11] >= 20 && lichtWerte[11] <= 40 && lichtWerte[10] >= 40 && lichtWerte[10] <= 60 && lichtWerte[9] >= 15 && lichtWerte[9] <= 25)
      || (lichtWerte[12] <= 70 &&  lichtWerte[15] >= 20 && lichtWerte[15] <= 40 && lichtWerte[14] >= 40 && lichtWerte[14] <= 60 && lichtWerte[13] >= 15 && lichtWerte[13] <= 25))
  {
    if (prevColorL.equals("white") && prevColorR.equals("white")) {
      Serial.println("GreenAtLeft");
      u8x8.drawString(0, 1, "Green at left");
      while (lichtWerte[4] >= 80) {
        links();
        getWerte(true);
      }
      u8x8.drawString(0, 1, "                  ");
    }
    else {
      vor();
      delay(500);
    }
  }

  if (((lichtWerte[0] <= 70 &&  lichtWerte[3] >= 20 && lichtWerte[3] <= 40 && lichtWerte[2] >= 40 && lichtWerte[2] <= 60 && lichtWerte[1] >= 15 && lichtWerte[1] <= 25)
       || (lichtWerte[4] <= 70 &&  lichtWerte[7] >= 20 && lichtWerte[7] <= 40 && lichtWerte[6] >= 40 && lichtWerte[6] <= 60 && lichtWerte[5] >= 15 && lichtWerte[5] <= 25))
      && ((lichtWerte[8] <= 70 &&  lichtWerte[11] >= 20 && lichtWerte[11] <= 40 && lichtWerte[10] >= 40 && lichtWerte[10] <= 60 && lichtWerte[9] >= 15 && lichtWerte[9] <= 25)
          || (lichtWerte[12] <= 70 &&  lichtWerte[15] >= 20 && lichtWerte[15] <= 40 && lichtWerte[14] >= 40 && lichtWerte[14] <= 60 && lichtWerte[13] >= 15 && lichtWerte[13] <= 25)))
  {
    if (prevColorL.equals("white") && prevColorR.equals("white")) {
      Serial.println("GreenAtRightAndLeft");
      u8x8.drawString(0, 1, "Green at both");
      drehen();
      delay(2000);
      u8x8.drawString(0, 1, "                  ");
    }
    else {
      vor();
      delay(500);
    }
  }*/
  /*if(stopwatch >=  10 && (yW<15 && yW>-15)){
    if(RZTasterW == 0){
      u8x8.drawString(0,1,"RZ found");
      rueck();
      delay(9000);
      RoteZone();
    }
  }*/
  loops++;
}
