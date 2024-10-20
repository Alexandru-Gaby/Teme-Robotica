#include <Arduino.h>


void pornesteIncarcarea();
void opresteIncarcarea();
void finalizeazaIncarcarea();
void actualizeazaLEDuriIncarcare();
void resetareLEDuriIncarcare();
void delayNonBlocking(unsigned long durata);


const int ledRGB_R = 6;    // Pinul pentru LED-ul RGB Roșu 
const int ledRGB_G = 5;    // Pinul pentru LED-ul RGB Verde
const int ledRGB_B = 4;    // Pinul pentru LED-ul RGB Albastru
const int ledL1 = 13;      // LED-ul 1 - 25%
const int ledL2 = 12;      // LED-ul 2 - 50%
const int ledL3 = 11;      // LED-ul 3 - 75%
const int ledL4 = 10;      // LED-ul 4 - 100%
const int butonStart = 3;  // Butonul de start
const int butonStop = 2;   // Butonul de stop

// Variabile pentru stări
bool incarcareInProgres = false;
bool statieLibera = true;
int stadiuIncarcare = 0; 
unsigned long timpUltimaSchimbare = 0;
unsigned long intervalClipire = 3000; // 3 secunde pentru fiecare pas de încărcare
unsigned long timpUltimClipit = 0;

// Debouncing și apăsări lungi/scurte
unsigned long debounceTime = 50;  
unsigned long lastButtonPress = 0;
unsigned long durataApasareStop = 0;
bool butonStartApasat = false;
bool butonStopApasat = false;

// inițializarea pinilor
void setup() {
  pinMode(ledRGB_R, OUTPUT);
  pinMode(ledRGB_G, OUTPUT);
  pinMode(ledRGB_B, OUTPUT);
  pinMode(ledL1, OUTPUT);
  pinMode(ledL2, OUTPUT);
  pinMode(ledL3, OUTPUT);
  pinMode(ledL4, OUTPUT);
  
  pinMode(butonStart, INPUT_PULLUP);  // Rezistență internă pull-up
  pinMode(butonStop, INPUT_PULLUP);   // Rezistență internă pull-up
  
  // Pornirea inițială a stației (LED RGB verde)
  digitalWrite(ledRGB_R, LOW);
  digitalWrite(ledRGB_G, HIGH);
  digitalWrite(ledRGB_B, LOW);
}


void loop() {
    unsigned long timpulCurent = millis(); // Timpul curent

    // Verificare buton start (apasare scurtă pentru a începe încărcarea)
    if (digitalRead(butonStart) == LOW && !butonStartApasat && statieLibera) {
        if (timpulCurent - lastButtonPress > debounceTime) {
            butonStartApasat = true;
            lastButtonPress = timpulCurent;
            pornesteIncarcarea();
        }
    } else if (digitalRead(butonStart) == HIGH) {
        butonStartApasat = false;
    }

    // Verificare buton stop (apasare lungă pentru a opri încărcarea)
    if (digitalRead(butonStop) == LOW && incarcareInProgres) {
        if (timpulCurent - lastButtonPress > debounceTime) {
            if (!butonStopApasat) {
                durataApasareStop = timpulCurent; // Inițializăm durata apasarii
                butonStopApasat = true;
            }
            // Apasare lungă pentru minim 1 secundă
            if (butonStopApasat && timpulCurent - durataApasareStop > 1000) {
                opresteIncarcarea(); 
            }
        }
    } else if (digitalRead(butonStop) == HIGH) {
        butonStopApasat = false;
    }

    // Simularea procesului de încărcare
    if (incarcareInProgres) {
        if (timpulCurent - timpUltimaSchimbare >= intervalClipire) {
            timpUltimaSchimbare = timpulCurent;
            stadiuIncarcare++;

            if (stadiuIncarcare > 4) {
                finalizeazaIncarcarea();  // Se finalizează procesul de încărcare
            } else {
                actualizeazaLEDuriIncarcare();
            }
        }

        // Clipire pentru LED-ul curent
        if (timpulCurent - timpUltimClipit >= 500) {  // Clipire la fiecare 500ms
            timpUltimClipit = timpulCurent;
            actualizeazaLEDuriIncarcare();
        }
    }
}


// Funcția pentru a porni procesul de încărcare
void pornesteIncarcarea() {
    if (statieLibera) { // Verifică dacă stația este liberă înainte de a porni
        statieLibera = false; // Setează stația ca ocupată
        incarcareInProgres = true; // Setează starea de încărcare ca în progres
        stadiuIncarcare = 1;  // Începem de la stadiul 1
        timpUltimaSchimbare = millis();
        timpUltimClipit = millis(); // Resetăm timerul pentru clipire

        // LED-ul RGB devine roșu (stația ocupată)
        digitalWrite(ledRGB_G, LOW); // Stinge verde
        digitalWrite(ledRGB_R, HIGH); // Aprinde roșu

        actualizeazaLEDuriIncarcare(); // Actualizează LED-urile
    }
}



void opresteIncarcarea() {
  incarcareInProgres = false;
  statieLibera = true; // Resetare stație liberă
  stadiuIncarcare = 0;  // Resetare încărcare

  // Animație finală: toate LED-urile clipesc de 3 ori
  for (int i = 0; i < 3; i++) { 
    digitalWrite(ledL1, HIGH);
    digitalWrite(ledL2, HIGH);
    digitalWrite(ledL3, HIGH);
    digitalWrite(ledL4, HIGH);
    delayNonBlocking(500);
    digitalWrite(ledL1, LOW);
    digitalWrite(ledL2, LOW);
    digitalWrite(ledL3, LOW);
    digitalWrite(ledL4, LOW);
    delayNonBlocking(500);
  }

  // Resetează starea LED-urilor
  digitalWrite(ledRGB_R, LOW); // Stinge roșu
  digitalWrite(ledRGB_G, HIGH);  // Stația devine liberă, aprinde verde
  digitalWrite(ledRGB_B, LOW); // Stinge albastru
  resetareLEDuriIncarcare();
}


void finalizeazaIncarcarea() {
    incarcareInProgres = false; // Setează starea încărcării ca fiind oprită
    statieLibera = true; // Resetează stația ca liberă
    stadiuIncarcare = 0; // Resetează stadiul de încărcare

    // Animație finală: toate LED-urile clipesc de 3 ori
    for (int i = 0; i < 4; i++) {  // Clipire de exact 3 ori
        // Aprinde toate LED-urile
        digitalWrite(ledL1, HIGH);
        digitalWrite(ledL2, HIGH);
        digitalWrite(ledL3, HIGH);
        digitalWrite(ledL4, HIGH);
        delayNonBlocking(500); // Așteaptă 500 ms

        // Stinge toate LED-urile
        digitalWrite(ledL1, LOW);
        digitalWrite(ledL2, LOW);
        digitalWrite(ledL3, LOW);
        digitalWrite(ledL4, LOW);
        delayNonBlocking(500); // Așteaptă 500 ms
    }

    // Stația devine liberă, LED-ul RGB devine verde
    digitalWrite(ledRGB_R, LOW); // Stinge roșu
    digitalWrite(ledRGB_G, HIGH); // Aprinde verde
    digitalWrite(ledRGB_B, LOW); // Stinge albastru
    resetareLEDuriIncarcare();
}


// Funcția pentru a actualiza LED-urile în timpul procesului de încărcare
void actualizeazaLEDuriIncarcare() {
  // Resetează toate LED-urile
  resetareLEDuriIncarcare();

  // Clipirea LED-ului corespunzător stadiului de încărcare
  if (stadiuIncarcare == 1) {
    if (millis() % 1000 < 500) digitalWrite(ledL1, HIGH);
  } else if (stadiuIncarcare == 2) {
    digitalWrite(ledL1, HIGH);
    if (millis() % 1000 < 500) digitalWrite(ledL2, HIGH);
  } else if (stadiuIncarcare == 3) {
    digitalWrite(ledL1, HIGH);
    digitalWrite(ledL2, HIGH);
    if (millis() % 1000 < 500) digitalWrite(ledL3, HIGH);
  } else if (stadiuIncarcare == 4) {
    digitalWrite(ledL1, HIGH);
    digitalWrite(ledL2, HIGH);
    digitalWrite(ledL3, HIGH);
    if (millis() % 1000 < 500) digitalWrite(ledL4, HIGH);
  }
}

// Funcția pentru a reseta LED-urile în timpul încărcării
void resetareLEDuriIncarcare() {
  digitalWrite(ledL1, LOW);
  digitalWrite(ledL2, LOW);
  digitalWrite(ledL3, LOW);
  digitalWrite(ledL4, LOW);
}

// Funcția pentru a implementa un delay non-blocking
void delayNonBlocking(unsigned long durata) {
  unsigned long start = millis();
  while (millis() - start < durata) {
    // Codul se va executa în continuare în timpul delay-ului
  }
}