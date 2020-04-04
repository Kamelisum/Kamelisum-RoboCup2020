// Anfang Methoden
char sda[16];
// Zeit zur Messung
int zeit = 10;

// Portbelegung Diodenlicht
int diodeRot1   = 4;
int diodeGruen1 = 3;
int diodeBlau1  = 2;
int diodeRot2   = 7;
int diodeGruen2 = 6;
int diodeBlau2  = 5;
int diodeRot3   = 10;
int diodeGruen3 = 9;
int diodeBlau3  = 8;
int diodeRot4   = 13;
int diodeGruen4 = 12;
int diodeBlau4  = 11;

// Portbelegung Fotodiode
// A0 bis A3 ist von links nach rechts in Fahrtrichtung
unsigned char fd1 = A0;
unsigned char fd2 = A1;
unsigned char fd3 = A2;
unsigned char fd4 = A6;

// wert... ist der gemessene, wert...P der Prozentwert
int wertHell;
int wertRot;
int wertGruen;
int wertBlau;
long wertHellP;
long wertRotP;
long wertGruenP;
long wertBlauP;

int diodenWerte[16];
// Ende Attribute


// Anfang Methoden



int lese(unsigned char diode)
{
  int wert = analogRead(diode);
  return wert;
}

int leseHell(unsigned char diode, int diodennummer)
{
  digitalWrite(diodennummer*3-1, HIGH);
  digitalWrite(diodennummer*3,   HIGH);
  digitalWrite(diodennummer*3+1, HIGH);
  delay(zeit);
  int hell = lese(diode);
  return hell;  
}

int leseRot(unsigned char diode, int diodennummer)
{
  digitalWrite(diodennummer*3-1, HIGH);
  digitalWrite(diodennummer*3,   LOW);
  digitalWrite(diodennummer*3+1, LOW);
  delay(zeit);
  int rot = lese(diode);
  return rot;
}

int leseGruen(unsigned char diode, int diodennummer)
{
  digitalWrite(diodennummer*3-1, LOW);
  digitalWrite(diodennummer*3,   HIGH);
  digitalWrite(diodennummer*3+1, LOW);
  delay(zeit);
  int gruen = lese(diode);
  return gruen;
}

int leseBlau(unsigned char diode, int diodennummer)
{
  digitalWrite(diodennummer*3-1, LOW);
  digitalWrite(diodennummer*3,   LOW);
  digitalWrite(diodennummer*3+1, HIGH);
  delay(zeit);
  int blau = lese(diode);
  return blau;
}

void einlesen(unsigned char diode, int diodennummer)
{
  wertHell  = 1023-leseHell(diode, diodennummer);
  wertRot   = 1023-leseRot(diode, diodennummer);
  wertGruen = 1023-leseGruen(diode, diodennummer);
  wertBlau  = 1023-leseBlau(diode, diodennummer);
  wertHellP = wertHell/11;
  wertRotP  = wertRot * 100L / ( wertRot + wertGruen + wertBlau);
  wertGruenP = wertGruen * 100L / ( wertRot + wertGruen + wertBlau);
  wertBlauP = wertBlau * 100L / ( wertRot + wertGruen + wertBlau);
}
// Ende Methoden








void setupK() {
  pinMode(diodeRot1, OUTPUT);  
  pinMode(diodeGruen1, OUTPUT);
  pinMode(diodeBlau1, OUTPUT);
  pinMode(diodeRot2, OUTPUT);  
  pinMode(diodeGruen2, OUTPUT);
  pinMode(diodeBlau2, OUTPUT);
  pinMode(diodeRot3, OUTPUT);  
  pinMode(diodeGruen3, OUTPUT);
  pinMode(diodeBlau3, OUTPUT);
  pinMode(diodeRot4, OUTPUT);  
  pinMode(diodeGruen4, OUTPUT);
  pinMode(diodeBlau4, OUTPUT);
  pinMode(fd1, INPUT);
  pinMode(fd2, INPUT);
  pinMode(fd3, INPUT);
  pinMode(fd4, INPUT);
  Serial.begin(9600);                       // Serieller Monitor an, um am Computer die Werte zu sehen.
  digitalWrite(diodeRot1, HIGH);
  digitalWrite(diodeRot2, HIGH);
  digitalWrite(diodeRot3, HIGH);
  digitalWrite(diodeRot4, HIGH);
  Serial.println("Rot leuchtet");
  delay(500);
  digitalWrite(diodeRot1, LOW);
  digitalWrite(diodeRot2, LOW);
  digitalWrite(diodeRot3, LOW);
  digitalWrite(diodeRot4, LOW);
  digitalWrite(diodeGruen1, HIGH);
  digitalWrite(diodeGruen2, HIGH);
  digitalWrite(diodeGruen3, HIGH);
  digitalWrite(diodeGruen4, HIGH);
  Serial.println("Gruen leuchtet");
  delay(500);
  digitalWrite(diodeGruen1, LOW);
  digitalWrite(diodeGruen2, LOW);
  digitalWrite(diodeGruen3, LOW);
  digitalWrite(diodeGruen4, LOW);
  digitalWrite(diodeBlau1, HIGH);
  digitalWrite(diodeBlau2, HIGH);
  digitalWrite(diodeBlau3, HIGH);
  digitalWrite(diodeBlau4, HIGH);
  Serial.println("Blau leuchtet");
  delay(500);
  digitalWrite(diodeBlau1, LOW);
  digitalWrite(diodeBlau2, LOW);
  digitalWrite(diodeBlau3, LOW);
  digitalWrite(diodeBlau4, LOW);
}
