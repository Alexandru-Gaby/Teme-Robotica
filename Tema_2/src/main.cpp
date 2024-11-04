#include <Arduino.h>
#include <avr/interrupt.h>


const int ledRGB_R = 6;
const int ledRGB_G = 5;
const int ledRGB_B = 4;
const int butonStart = 3;
const int butonDificultate = 2;


enum Dificultate {EASY, MEDIUM, HARD};
Dificultate dificultateCurenta = EASY; // Dificultate implicită
unsigned long timpRunda = 30000;
unsigned long startTimp;
unsigned long intervalCuvant; // Interval între cuvinte în funcție de dificultate
unsigned long ultimaAfisareCuvant = 0;
bool rundaActiva = false;
bool numaratoareInversaActiva = false;
int scor = 0;
int secundeRamase = 3; // Variabilă pentru numărătoarea inversă
String cuvantCurent = ""; // Cuvântul curent de tastat
unsigned long debounceDelay = 200; // Debounce pentru butoane
unsigned long ultimaApasareStart = 0;
unsigned long ultimaApasareDificultate = 0;

// Variabile pentru clipirea LED-ului
bool stareLedAlb = false; // Starea curentă a LED-ului alb
unsigned long ultimaSchimbareLED = 0; // Ultimul moment când LED-ul a fost schimbat


String generareCuvantAleator();


void setareLED(int r, int g, int b) {
  analogWrite(ledRGB_R, r);
  analogWrite(ledRGB_G, g);
  analogWrite(ledRGB_B, b);
}


void schimbareDificultate() {
  if (!rundaActiva && (millis() - ultimaApasareDificultate > debounceDelay)) {
    ultimaApasareDificultate = millis();
    dificultateCurenta = (Dificultate)((dificultateCurenta + 1) % 3);
    switch(dificultateCurenta) {
      case EASY: 
        Serial.println("Easy mode on!");
        intervalCuvant = 7000;
        break;
      case MEDIUM: 
        Serial.println("Medium mode on!");
        intervalCuvant = 5000;
        break;
      case HARD: 
        Serial.println("Hard mode on!");
        intervalCuvant = 3000;
        break;
    }
  }
}

void pornireOprireRunda() {
  if (millis() - ultimaApasareStart > debounceDelay) {
    ultimaApasareStart = millis();
    if (rundaActiva) {
      rundaActiva = false;
      Serial.print("Runda oprită. Scor final: ");
      Serial.println(scor);
      setareLED(255, 255, 255); // LED alb pentru repaus
    } else {
      rundaActiva = true;
      startTimp = millis();
      scor = 0;
      numaratoareInversaActiva = true;
      ultimaAfisareCuvant = millis();
      secundeRamase = 3;
      Serial.println("Numărătoare inversă:");
      setareLED(255, 255, 255); // LED alb în timpul numărătorii inverse
    }
  }
}

void numaratoareInversa() {
  unsigned long timpulCurent = millis();
  //returnează numărul de milisecunde scurse de la pornirea Arduino

  if (numaratoareInversaActiva) {
    // Clipirea LED-ului alb la fiecare 500 ms în timpul numărătorii inverse
    if (timpulCurent - ultimaSchimbareLED >= 500) {
      ultimaSchimbareLED = timpulCurent;
      stareLedAlb = !stareLedAlb;
      if (stareLedAlb) {
        setareLED(255, 255, 255); // LED alb pornit
      } else {
        setareLED(0, 0, 0); // LED alb oprit
      }
    }

    // Numărătoarea inversă
    if (timpulCurent - ultimaAfisareCuvant >= 1000) {
      ultimaAfisareCuvant = timpulCurent;
      Serial.println(secundeRamase);
      secundeRamase--;
      if (secundeRamase <= 0) {
        numaratoareInversaActiva = false;
        setareLED(0, 255, 0); // LED verde pentru începutul rundei
        Serial.println("Runda a început!");
        cuvantCurent = generareCuvantAleator();
        Serial.print("Tastați cuvântul: ");
        Serial.println(cuvantCurent);
        ultimaAfisareCuvant = millis(); // Resetează temporizatorul pentru cuvinte
      }
    }
  }
}


String generareCuvantAleator() {
  String dictionar[] = {"arduino", "led", "circuit", "rezistor", "tensiune", "buton"};
  return dictionar[random(0, 6)];
}


void setup() {
  Serial.begin(9600);

  // Configurare pini
  pinMode(ledRGB_R, OUTPUT);
  pinMode(ledRGB_G, OUTPUT);
  pinMode(ledRGB_B, OUTPUT);
  pinMode(butonStart, INPUT_PULLUP);
  pinMode(butonDificultate, INPUT_PULLUP);


  // Setare LED alb pentru repaus
  setareLED(255, 255, 255);

  dificultateCurenta = EASY; // Dificultate implicită
  intervalCuvant = 7000; // 7 secunde pentru Easy

  // Atașare întreruperi pentru butoane
  attachInterrupt(digitalPinToInterrupt(butonStart), pornireOprireRunda, FALLING);
  attachInterrupt(digitalPinToInterrupt(butonDificultate), schimbareDificultate, FALLING);

  // Inițializare random seed pentru generarea cuvintelor
  randomSeed(analogRead(0));
}


void loop() {
  if (numaratoareInversaActiva) {
    numaratoareInversa();
    return;
  }

  if (rundaActiva) {
    // Generarea unui nou cuvânt dacă timpul intervalului a expirat
    if (millis() - ultimaAfisareCuvant >= intervalCuvant) {
      ultimaAfisareCuvant = millis(); // Resetează temporizatorul
      cuvantCurent = generareCuvantAleator();
      Serial.print("Tastați cuvântul: ");
      Serial.println(cuvantCurent);
    }
    
    // Verificarea dacă există date disponibile pe serial
    if (Serial.available()) {
      String cuvantIntrodus = Serial.readString();
      cuvantIntrodus.trim(); // Elimină spațiile goale

      // Verificarea dacă cuvântul introdus este corect
      if (cuvantIntrodus.equals(cuvantCurent)) {
        scor++;
        setareLED(0, 255, 0); // LED verde pentru corect
        // Serial.println("Corect!");

        // Generare nou cuvânt imediat după un răspuns corect
        cuvantCurent = generareCuvantAleator();
        Serial.print("Tastați cuvântul: ");
        Serial.println(cuvantCurent);
        ultimaAfisareCuvant = millis(); // Resetează temporizatorul pentru următorul cuvânt
      } else {
        setareLED(255, 0, 0); // LED roșu pentru greșit
        // Serial.println("Greșit!");
      }
    }
    
    // Verificarea dacă timpul rundei a expirat
    if (millis() - startTimp >= timpRunda) {
      rundaActiva = false;
      Serial.print("Runda terminată. Scor final: ");
      Serial.println(scor);
      setareLED(255, 255, 255); // LED alb pentru repaus
    }
  }
}
