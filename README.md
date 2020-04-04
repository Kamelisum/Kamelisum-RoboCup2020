## Our robot Kamelisum at 2020
#### The explanation for the code of the main Arduino
```
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
```
Importing the header-Files , the librarys and defining some pins
```
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
```
Read out the data from the gyroscope and print it in the Serial Monitor and on the OLED-Display.
```
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
```
Read the data from the ultrasonic-range detectors, change it into Strings and print it.
```
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

```
Get values over I²C-Bus and the other data.
```
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

```
Initializing pins, setting Serial baud and start the library for the Display.
