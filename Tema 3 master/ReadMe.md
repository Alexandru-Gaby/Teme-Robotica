# Componente
- x1 Arduino UNO
- x1 Ecran LCD
- x1 Servomotor
- x1 Potențiometru
- x1 Buton
- x1 Rezistor 330 Ohm
- x1 Rezistor 1k Ohm

# Cerințe


SPI: Tema aceasta implică foarte multe legături. Atât de multe încât un singur arduino uno nu ne oferă suficienți pini GPIO. De aceea pentru această temă este nevoie de 2 arduino uno care vor comunica folosind SPI. Arduinoul master va fi cel responsabil pentru controlul LCD-ului, servomotorului și va fi cel care ține în memorie starea jocului (ex. punctajul fiecărui jucator, ledul care trebuie să fie aprins acum, etc.). Arduino-ul slave va controla butoanele și ledurile, primind mesaje de la arduinoul master pentru a ști ce led să aprindă și trimițând înapoi mesaje despre butonul apăsat.

Butoane:

Pentru începerea jocului butonul de start poate fi implementat în diverse moduri:
Orice buton începe jocul
Un anume buton începe jocul (ar trebui să fie clar pe breadboard care este acel buton care pornește jocul)• Un al 7-lea buton dedicat poate fi pus pentru pornirea jocului
Cât timp jocul este în desfășurare butoanele trebuie să poată fi folosite doar pentru controlul jocului și să nu reseteze progresul
Doar butoanele jucătorului din acea rundă trebuie să poată controla jocul
Chiar și cu 2 plăci arduino nu avem suficienți pini pentru toate componentele. De aceea, putem multiplexa butoanele folosind rezistențe (www.youtube.com/watch?v=Y23vMfynUJ0)

LED-uri:

Fiecare buton are asociat un LED de o culoare diferită. Pe parcursul jocului acestea trebuie să fie aprinse pentru a vedea cărei culoare îi corespunde fiecare buton
LED-ul rgb trebuie să se aprindă în una din cele 3 culori ale butoanelor
LED-ul rgb trebuie să fie stins dacă nu este runda jucătorului corespunzător acelui LED

LCD:

Pentru controlul acestuia ne putem folosi de biblioteca LiquidCrystal
Trebuie să aibă setată o luminozitate și un contrast suficient de bune cât să fie vizibil textul pe ecran.
Vor fi folosiți doar pini D4-7 pentru liniile de date ale ecranului
Pe parcursul jocului trebuie să afișeze punctajele celor 2 jucători
Servomotor:

Servomotorul va începe de la poziția de 0 grade și se va deplasa în sens antiorar pentru a indica scurgerea timpului.
Anteție la valoarea trimisă către servo-motor. Prin biblioteca Servo.h noi trimitem la servomotor rotație absolută, nu relativă la cea curentă.

# Flow

Inițializare

Jocul pornește cu afișarea unui mesaj de bun venit pe LCD. Apăsarea unui buton declanșează startul jocului.

Începerea jocului se realizează prin apăsarea al 7-lea buton dedicat destinat exclusiv pornirii jocului.

Desfășurarea Rundelor

Fiecare jucător are trei butoane, fiecare asociat unui LED de o culoare diferită și un al 4-lea LED RGB.
La fiecare rundă, fiecare jucător este cel activ.
LED-ul RGB al jucătorului activ se aprinde într-o culoare corespunzătoare unuia dintre butoanele sale. Jucătorul trebuie să apese cât mai rapid butonul care corespunde culorii LED-ului RGB, pentru a obține puncte. Cu cât reacționează mai repede, cu atât primește mai multe puncte.
La finalul unei runde LCD-ul afișează punctajul actualizat al ambilor jucători.
Pe tot parcursul jocului display-ul LCD va arata punctajul fiecarui jucator

Timpul și Finalizarea Jocului

Servomotorul se rotește pe parcursul jocului, indicând progresul. O rotație completă a servomotorului marchează sfârșitul jocului (voi decideti cat de repede se misca).
La final, LCD-ul afișează numele câștigătorului și scorul final pentru câteva secunde, apoi revine la ecranul de start cu mesajul de bun venit.

# Functii
- **`setup()`**: Inițializează hardware-ul (LCD, servo, SPI, butonul de start).
- **`loop()`**: Verifică dacă jocul a început, gestionează runde și actualizează scorul pe LCD.
- **`startRound()`**: Trimite ID-ul jucătorului activ către dispozitivul slave și actualizează scorul.
- **`displayScore()`**: Afișează scorurile actuale ale celor doi jucători pe LCD.
- **`receiveSlaveResponse()`**: Primiți și returnați răspunsul de la dispozitivul slave prin SPI.
- **`stopGame()`**: Oprește jocul, afișează rezultatul și resetează sistemul.


# Poze

![imagine1](https://github.com/user-attachments/assets/cc29e33f-149a-4241-aadc-4833577b5673)

![imagine2](https://github.com/user-attachments/assets/9e7f0bba-bd95-4fe2-9fe0-b7262ef4c8b0)

![poza1](https://github.com/user-attachments/assets/f187999a-ae25-447f-b488-b7eed443f9b4)

# Schema electrică Tinkercad

![poza2](https://github.com/user-attachments/assets/9acddaef-1c2a-4965-b306-1dd8e62d871a)

# Link video cu funcționalitatea montajului fizic

[Link youtube](https://www.youtube.com/shorts/1wlZ9vS242c)
