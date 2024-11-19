# Componente
    6x LED-uri (2 grupuri de câte 3 leduri, în cadrul unui grup trebuie să avem culori diferite)
    2x LED RGB (1 pentru fiecare jucător)
    6x butoane (3 pentru fiecare jucător)
    6x rezistențe 1000 ohm
    12x rezistențe 220 ohm
    1x Breadboard
    Fire de legatura
    1x Arduino Uno

# Flow

    Jocul pornește cu afișarea unui mesaj de bun venit pe LCD. Apăsarea unui buton declanșează startul jocului.

    Pentru începerea jocului, butonul de start poate fi implementat într-un mod flexibil,
    un al 7-lea buton dedicat* – se poate adăuga un buton suplimentar destinat exclusiv pornirii jocului.
## Desfășurarea Rundelor
    Fiecare jucător are trei butoane, fiecare asociat unui LED de o culoare diferită și un al 4-lea LED RGB.

    La fiecare rundă, fiecare jucător este cel activ.
    LED-ul RGB al jucătorului activ se aprinde într-o culoare corespunzătoare unuia dintre butoanele sale. Jucătorul trebuie să apese cât mai rapid butonul care corespunde culorii LED-ului RGB, pentru a obține puncte. Cu cât reacționează mai repede, cu atât primește mai multe puncte.
    La finalul unei runde LCD-ul afișează punctajul actualizat al ambilor jucători.
    Pe tot parcursul jocului display-ul LCD va arata punctajul fiecarui jucator
## Timpul și Finalizarea Jocului
    Servomotorul se rotește pe parcursul jocului, indicând progresul. O rotație completă a servomotorului marchează sfârșitul jocului (voi decideti cat de repede se misca).
    La final, LCD-ul afișează numele câștigătorului și scorul final pentru câteva secunde, apoi revine la ecranul de start cu mesajul de bun venit.

# Codul este organizat astfel:
Funcția ```setup()``` se execută o singură dată atunci când programul pornește, pentru a configura pinii și setările inițiale ale hardware-ului:
- Pinii pentru LED-uri: Se setează pinurile pentru LED-urile jucătorilor (1 și 2) ca ieșiri și le aprinde (setează la ```HIGH```).
- Pinii RGB: Se configurează pinii pentru LED-urile RGB pentru fiecare jucător ca ieșiri.
- Pinii pentru butoane: Setează pinurile pentru butoanele de apăsat (pentru fiecare culoare - roșu, verde, albastru) ale fiecărui jucător ca intrări cu ```rezistență de pull-up```.
- Configurație SPI: Se setează SPI-ul pentru a funcționa în modul slave și se atașează o întrerupere pentru a gestiona mesajele primite prin SPI.


Funcția ```checkButtons()```  verifică dacă unul dintre butoanele jucătorului activ a fost apăsat și dacă butonul corespunzător culorii RGB afișate a fost apăsat corect.

Funcția ```updateRgb()``` se ocupă cu schimbarea culorii RGB a jucătorului activ. La fiecare apel, aceasta:
Oprește RGB-ul ambilor jucători (îl setează pe LOW pe fiecare pin RGB al ambilor jucători).
Alege o culoare aleatoare pentru LED-ul RGB al jucătorului activ. Aceasta se face prin funcția random(0, 3) care returnează un număr aleatoriu între 0 și 2: 0 = Roșu, 1 = Verde,2 = Albastru. 
Activează culoarea aleasă pentru RGB-ul jucătorului activ. Dacă currentColor este 0 (roșu), se aprinde LED-ul roșu. Dacă este 1 (verde), se aprinde LED-ul verde și așa mai departe pentru albastru.

Funcția ```turnOffRgb(int player)``` dezactivează toate LED-urile RGB pentru un anumit jucător, setându-le pe toate la LOW (starea de oprire).

Funcția ```ISR(SPI_STC_vect)``` este un ISR (Interrupt Service Routine) gestionează o întrerupere generată de comunicarea SPI.