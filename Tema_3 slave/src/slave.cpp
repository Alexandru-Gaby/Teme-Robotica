#include <SPI.h>

// Pin definitions for player LEDs
const int player1LedPin = 2;
const int player2LedPin = 3;

// RGB LED pins for each player
const int player1RgbRedPin = 4, player1RgbGreenPin = 5, player1RgbBluePin = 6;
const int player2RgbRedPin = 7, player2RgbGreenPin = 8, player2RgbBluePin = 9;

// Button pins for player 1 and player 2
const int player1ButtonRedPin = A0, player1ButtonGreenPin = A1, player1ButtonBluePin = A2;
const int player2ButtonRedPin = A3, player2ButtonGreenPin = A4, player2ButtonBluePin = A5;

// Game variables
int activePlayer = 1;

// Timing variables for round and color change
unsigned long roundStartTime = 0;
const unsigned long roundDuration = 10000; // Round duration in milliseconds
int currentColor = 0; // 0 = red, 1 = green, 2 = blue
unsigned long lastColorChangeTime = 0;
const unsigned long colorChangeDuration = 2000; // Duration for each color

// Debouncing
unsigned long debounceDelay = 250;
unsigned long lastDebounceTime = 0;

// SPI communication variables
volatile byte receivedMessage = 0;
volatile bool newMessageFlag = false;

void updateRgb();
void checkButtons();
void turnOffRgb(int player);

void setup() {
  Serial.begin(9600);

  // Setup LED pins for players
  pinMode(player1LedPin, OUTPUT);
  pinMode(player2LedPin, OUTPUT);
  digitalWrite(player1LedPin, HIGH);  // Turn on player 1 LED
  digitalWrite(player2LedPin, HIGH);  // Turn on player 2 LED

  pinMode(player1RgbRedPin, OUTPUT); pinMode(player1RgbGreenPin, OUTPUT); pinMode(player1RgbBluePin, OUTPUT);
  pinMode(player2RgbRedPin, OUTPUT); pinMode(player2RgbGreenPin, OUTPUT); pinMode(player2RgbBluePin, OUTPUT);

  // Setup button pins for both players
  pinMode(player1ButtonRedPin, INPUT_PULLUP);
  pinMode(player1ButtonGreenPin, INPUT_PULLUP);
  pinMode(player1ButtonBluePin, INPUT_PULLUP);
  pinMode(player2ButtonRedPin, INPUT_PULLUP);
  pinMode(player2ButtonGreenPin, INPUT_PULLUP);
  pinMode(player2ButtonBluePin, INPUT_PULLUP);

  // Setup SPI communication
  SPI.begin();
  pinMode(MISO, OUTPUT); // Set MISO as output
  SPCR |= _BV(SPE); // Enable SPI in slave mode
  SPI.attachInterrupt();  // Attach SPI interrupt for receiving messages

  turnOffRgb(1); // Turn off RGB for player 1 at start
}

void loop() {

  checkButtons();

  // Check if it's time to update the RGB color
  if (millis() - lastColorChangeTime >= colorChangeDuration) {
    updateRgb();
    lastColorChangeTime = millis();
  }

  // Check if round time has expired
  if (millis() - roundStartTime >= roundDuration) {
    turnOffRgb(activePlayer); // Turn off RGB for the active player
    activePlayer = (activePlayer == 1) ? 2 : 1; // Switch to the other player
    roundStartTime = millis(); // Reset round timer for the next round
    Serial.print("Switching to player ");
    Serial.println(activePlayer);
  }
}

void updateRgb() {
  // Turn off both players' RGB LEDs before setting a new color
  turnOffRgb(1);
  turnOffRgb(2);

  currentColor = random(0, 3); // Randomly choose a color (0: red, 1: green, 2: blue)

  // Turn on the selected color for the active player's RGB LED
  if (activePlayer == 1) {
    if (currentColor == 0) digitalWrite(player1RgbRedPin, HIGH);
    if (currentColor == 1) digitalWrite(player1RgbGreenPin, HIGH);
    if (currentColor == 2) digitalWrite(player1RgbBluePin, HIGH);
  } else {
    if (currentColor == 0) digitalWrite(player2RgbRedPin, HIGH);
    if (currentColor == 1) digitalWrite(player2RgbGreenPin, HIGH);
    if (currentColor == 2) digitalWrite(player2RgbBluePin, HIGH);
  }
}

void checkButtons() {
  int buttonDetected = -1;

  if (millis() - lastDebounceTime > debounceDelay) {
    // Check buttons based on active player
    if (activePlayer == 1) {
      if (digitalRead(player1ButtonRedPin) == LOW) {
        buttonDetected = 0;
      } else if (digitalRead(player1ButtonGreenPin) == LOW) {
        buttonDetected = 1;
      } else if (digitalRead(player1ButtonBluePin) == LOW) {
        buttonDetected = 2;
      }
    } else {
      if (digitalRead(player2ButtonRedPin) == LOW) {
        buttonDetected = 0;
      } else if (digitalRead(player2ButtonGreenPin) == LOW) {
        buttonDetected = 1;
      } else if (digitalRead(player2ButtonBluePin) == LOW) {
        buttonDetected = 2;
      }
    }

    lastDebounceTime = millis(); // Update debounce time
  }

  // If a button is detected, check if it's the correct one
  if (buttonDetected != -1) {
    if (buttonDetected == currentColor) {
      Serial.println("Correct press!");
      SPI.transfer(0x01); // Send correct press signal
    } else {
      Serial.println("Incorrect press!");
      SPI.transfer(0x00); // Send incorrect press signal
    }
  }
}

void turnOffRgb(int player) {
  if (player == 1) {
    digitalWrite(player1RgbRedPin, LOW);
    digitalWrite(player1RgbGreenPin, LOW);
    digitalWrite(player1RgbBluePin, LOW);
  } else {
    digitalWrite(player2RgbRedPin, LOW);
    digitalWrite(player2RgbGreenPin, LOW);
    digitalWrite(player2RgbBluePin, LOW);
  }
}

// SPI interrupt service routine
ISR(SPI_STC_vect) {
  byte receivedCommand = SPDR;  // Read the command from the SPI data register
  receivedMessage = receivedCommand;
  newMessageFlag = true; // Set the flag for new message
}
