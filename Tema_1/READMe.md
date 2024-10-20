# Componente
    4x LED-uri (pentru a simula procentul de încărcare)
    1x LED RGB (pentru starea de liber sau ocupat)
    2x Butoane (pentru start încărcare și stop încărcare)
    9x Rezistoare (7x 220ohm, 2x 1K)
    1x Breadboard
    Linii de legătură

# Detalii tehnice
(1p) Led-ul RGB reprezintă disponibilitatea stației. Dacă stația este liberă led-ul va fi verde, iar dacă stația este ocupată se va face roșu.

(2p) Led-urile simple reprezintă gradul de încărcare al bateriei, pe care îl vom simula printr-un loader progresiv (L1 = 25%, L2 = 50%, L3 = 75%, L4 = 100%). Loader-ul se încărca prin aprinderea succesivă a led-urilor, la un interval fix de 3s. LED-ul care semnifică procentul curent de încărcare va avea starea de clipire, LED-urile din urma lui fiind aprinse continuu, iar celelalte stinse.

(1p) Apăsarea scurtă a butonului de start va porni încărcarea. Apăsarea acestui buton în timpul încărcării nu va face nimic.

(2p) Apăsarea lungă a butonului de stop va opri încărcarea forțat și va reseta stația la starea liberă. Apăsarea acestui buton cat timp stația este liberă nu va face nimic.

# Documentație
Această documentație oferă o descriere detaliată a codului pentru un sistem de simulare al unui proces de încărcare al unui vehicul electric, implementat pe un microcontroler Arduino. Sistemul utilizează LED-uri pentru a indica stadiul de încărcare și permite controlul prin intermediul unor butoane.

Proiectul simulează un proces de încărcare cu patru stadii, fiecare reprezentat de un LED. Utilizatorul poate iniția și opri procesul de încărcare prin apăsarea butoanelor dedicate. LED-urile se aprind progresiv, iar LED-ul RGB indică dacă stația de încărcare este liberă sau ocupată.

# Codul este organizat astfel:
### Definirea Pinilor

```cpp
const int ledRGB_R = 6;    // Pinul pentru LED-ul RGB Roșu 
const int ledRGB_G = 5;    // Pinul pentru LED-ul RGB Verde
const int ledRGB_B = 4;    // Pinul pentru LED-ul RGB Albastru
const int ledL1 = 13;      // LED-ul 1 - 25%
const int ledL2 = 12;      // LED-ul 2 - 50%
const int ledL3 = 11;      // LED-ul 3 - 75%
const int ledL4 = 10;      // LED-ul 4 - 100%
const int butonStart = 3;  // Butonul de start
const int butonStop = 2;   // Butonul de stop
```
Această secțiune definește pinul pentru fiecare LED, precum și pinul pentru butoane. Acești pini vor fi utilizați ulterior pentru a controla LED-urile și a citi starea butoanelor.

### Variabile pentru Stări

```cpp
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
```
Această secțiune definește variabilele utilizate pentru a urmări starea procesului de încărcare, precum și variabilele necesare pentru a gestiona debouncing-ul butoanelor.

### Funcția setup()

```cpp
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
```
Aici sunt configurate pinurile ca fiind ieșiri sau intrări, iar LED-ul RGB este inițializat în starea liberă (verde).

### Funcția loop()
```cpp
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
```
Funcția loop() este apelată repetat și gestionează interacțiunea utilizatorului cu butoanele și starea încărcării. Verifică dacă butoanele au fost apăsate și actualizează LED-urile în funcție de stadiul de încărcare.

### Funcții pentru Controlul Încărcării


```cpp
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
```
Pornește procesul de încărcare, setează starea corespunzătoare și actualizează LED-urile.


```cpp
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
```
Oprește procesul de încărcare și efectuează o animație de finalizare.

 ```cpp
 void finalizeazaIncarcarea() {
    incarcareInProgres = false; // Setează starea încărcării ca fiind oprită
    statieLibera = true; // Resetează stația ca liberă
    stadiuIncarcare = 0; // Resetează stadiul de încărcare

    // Animație finală: toate LED-urile clipesc de 3 ori
    for (int i = 0; i < 3; i++) {  // Clipire de exact 3 ori
        // Aprinde toate LED-urile
        digitalWrite(ledL1, HIGH);
        digitalWrite(ledL2, HIGH);
        digitalWrite(ledL3, HIGH);
        digitalWrite(ledL4, HIGH);
        delayNonBlocking(500);

        // Stinge toate LED-urile
        digitalWrite(ledL1, LOW);
        digitalWrite(ledL2, LOW);
        digitalWrite(ledL3, LOW);
        digitalWrite(ledL4, LOW);
        delayNonBlocking(500);
    }

    // Stația devine liberă, LED-ul RGB devine verde
    digitalWrite(ledRGB_R, LOW); // Stinge roșu
    digitalWrite(ledRGB_G, HIGH); // Aprinde verde
    digitalWrite(ledRGB_B, LOW); // Stinge albastru
    resetareLEDuriIncarcare();
}
```
Finalizează procesul de încărcare și efectuează o animație de finalizare.

###  Funcții pentru Actualizarea LED-urilor

```cpp
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
```
```resetareLEDuriIncarcare()```
asigură că toate LED-urile (L1, L2, L3, L4) sunt stinse înainte de a începe să lumineze pentru stadiul curent de încărcare. 

```stadiuIncarcare``` poate avea valori de la 1 la 4. Fiecare valoare reprezintă un stadiu diferit al procesului de încărcare:
    1: 25% încărcare (LED-ul L1 trebuie să clipească).
    2: 50% încărcare (LED-urile L1 și L2 trebuie să fie aprinse, dar L2 va clipi).
    3: 75% încărcare (LED-urile L1, L2, și L3 aprinse, L3 va clipi).
    4: 100% încărcare (toate LED-urile L1, L2, L3 și L4 sunt aprinse, dar L4 va clipi).

Condițiile din fiecare ramură if utilizează millis() % 1000 pentru a determina dacă LED-urile ar trebui să fie aprinse sau stinse în funcție de stadiul curent.
millis() returnează timpul în milisecunde de la pornirea programului, iar operatorul % (modulo) împărte timpul curent la 1000, dând restul (0-999). Aceasta permite alternarea LED-ului între aprins și stins.

```resetareLEDuriIncarcare()``` Resetează starea LED-urilor în timpul procesului de încărcare.
```cpp 
void resetareLEDuriIncarcare() {
    digitalWrite(ledL1, LOW);
    digitalWrite(ledL2, LOW);
    digitalWrite(ledL3, LOW);
    digitalWrite(ledL4, LOW);
}
```
### Funcția delayNonBlocking()
```cpp
void delayNonBlocking(unsigned long durata) {
    unsigned long start = millis();
    while (millis() - start < durata) {
        // Codul se va executa în continuare în timpul delay-ului
    }
}
```
Funcția ```delayNonBlocking()``` permite executarea altor coduri în timpul așteptării, fără a bloca execuția programului. Aceasta este esențială pentru a permite actualizarea continuă a stării butoanelor și LED-urilor.

```Debouncing-ul``` este realizat prin verificarea timpului scurs între citirile consecutive ale stării butonului, folosind funcția millis() care returnează timpul în milisecunde de la pornirea dispozitivului. Debouncing-ul se face atât pentru butonul de start, cât și pentru butonul de stop.

Definirea timpului de debounce ```unsigned long debounceTime = 50;```

Aceasta stabilește un timp minim de 50 de milisecunde între citirile succesive ale butonului. Dacă butonul este apăsat din nou înainte de a trece acest timp, noua apăsare va fi ignorată.

Variabilă pentru stocarea ultimului timp de apăsare: ```unsigned long lastButtonPress = 0;```

Aceasta este o variabilă care stochează timpul (în milisecunde) la care butonul a fost apăsat ultima dată. Este folosită pentru a compara cu timpul curent și a determina dacă a trecut suficient timp pentru a considera apăsarea validă.

```cpp
if (digitalRead(butonStart) == LOW && !butonStartApasat && statieLibera) {
    if (timpulCurent - lastButtonPress > debounceTime) {
        butonStartApasat = true;
        lastButtonPress = timpulCurent;
        pornesteIncarcarea();
    }
}
```
```digitalRead(butonStart) == LOW:``` Verifică dacă butonul este apăsat (starea LOW).

```!butonStartApasat ``` Asigură că butonul nu este deja considerat apăsat (pentru a preveni apăsări multiple).

```timpulCurent - lastButtonPress > debounceTime``` Compară timpul curent cu timpul ultimei apăsări (lastButtonPress). Dacă diferența este mai mare decât debounceTime (50 ms), înseamnă că este o apăsare validă.

# Poze

![Poza1](https://github.com/user-attachments/assets/3821be7d-e24f-4f65-a1fb-7be5211ac251)

![Poza2](https://github.com/user-attachments/assets/7e552a61-00dd-48ae-9647-8dedaaa37cbf)

![Circuit](https://github.com/user-attachments/assets/706cdaae-4a6c-4c89-9e3a-34acb4bfd66a)

# Schema electrică Tinkercad
![Poza 3](https://github.com/user-attachments/assets/5cfff703-d3d0-42f3-8778-189de7f71641)

# Link video cu funcționalitatea montajului fizic

[Link](https://youtube.com/shorts/ImRSBbbYiIc)



