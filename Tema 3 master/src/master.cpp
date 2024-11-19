#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// LCD Pin Definitions
const int rsPin = 3, enPin = 2, d4Pin = 4, d5Pin = 5, d6Pin = 6, d7Pin = 7;
LiquidCrystal lcd(rsPin, enPin, d4Pin, d5Pin, d6Pin, d7Pin);

// Servo Pin Definition
const int servoPin = 9;
Servo servomotor;

// Button and Game Timing Constants
const int startButton = A0;
const uint32_t ROUND_DURATION = 10000;  // 10 seconds for each round
const uint32_t GAME_DURATION = 20000;   // 20 seconds for total game time
const uint32_t LCD_UPDATE_INTERVAL = 1000; // Update LCD every 1000ms (1 second)
const uint32_t GAME_END_DELAY = 3000;   // 3 seconds delay after the game ends

// Game Variables
int player1Score = 0;
int player2Score = 0;
int activePlayer = 1; // 1 for Player 1, 2 for Player 2

uint32_t roundStartTime = 0;
uint32_t gameStartTime = 0;
bool gameInProgress = false;
uint32_t lastLcdUpdate = 0;

void startRound();
void displayScore();
byte receiveSlaveResponse();
void stopGame();

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Welcome!");

  servomotor.attach(servoPin);
  servomotor.write(0); // Initial position at 0 degrees

  SPI.begin();
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  
  pinMode(startButton, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Master System Initialized.");
}

void loop() {
  if (!gameInProgress && digitalRead(startButton) == LOW) {
    delay(200); // Debouncing
    lcd.clear();
    lcd.print("Game Started!");
    delay(1000);

    // Initialize game
    player1Score = 0;
    player2Score = 0;
    activePlayer = 1;
    gameInProgress = true;

    roundStartTime = millis(); // Start first round
    gameStartTime = millis(); // Start game timer
  }

  if (gameInProgress) {
    // Check if total game time has expired
    if (millis() - gameStartTime >= GAME_DURATION) {
      stopGame();
      return;
    }

    // Run round for the active player
    startRound();

    // Check if the round time has expired
    if (millis() - roundStartTime >= ROUND_DURATION) {
      activePlayer = (activePlayer == 1) ? 2 : 1; // Switch active player
      roundStartTime = millis(); // Reset round timer
    }

    // Update LCD score
    if (millis() - lastLcdUpdate >= LCD_UPDATE_INTERVAL) { // Update every second
      displayScore();
      lastLcdUpdate = millis();
    }

    // Update the servo gradually based on game time
    int servoAngle = map(millis() - gameStartTime, 0, GAME_DURATION, 0, 180); 
    servomotor.write(servoAngle);
  }
}

void startRound() {
  digitalWrite(SS, LOW); // Activate Slave select

  // Send active player for the current round
  SPI.transfer(activePlayer);

  // Receive response from slave
  byte response = receiveSlaveResponse();
  if (response == 0x01) { // Correct press
    if (activePlayer == 1) {
      player1Score++;
      Serial.println("Player 1 scored!");
    } else {
      player2Score++;
      Serial.println("Player 2 scored!");
    }
  } else if (response == 0x00) { // Incorrect press
    Serial.println("Incorrect press!");
  }

  digitalWrite(SS, HIGH); // Deactivate Slave select
}

void displayScore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Player 1: ");
  lcd.print(player1Score);
  lcd.setCursor(0, 1);
  lcd.print("Player 2: ");
  lcd.print(player2Score);
}

byte receiveSlaveResponse() {
  byte response = SPI.transfer(0x00); // Receive response from Slave
  Serial.print("Response from Slave: ");
  Serial.println(response, HEX); // Debugging
  return response;
}

void stopGame() {
  gameInProgress = false;

  // Stop RGB LEDs (send signal to slave)
  digitalWrite(SS, LOW);
  SPI.transfer(0xFF); // Stop signal
  digitalWrite(SS, HIGH);

  lcd.clear();
  if (player1Score > player2Score) {
    lcd.print("Winner: Player 1");
  } else if (player2Score > player1Score) {
    lcd.print("Winner: Player 2");
  } else {
    lcd.print("Draw!");
  }

  uint32_t endTime = millis();
  while (millis() - endTime < GAME_END_DELAY); // Wait for 3 seconds

  Serial.println("Game Over.");
  Serial.print("Final Score Player 1: ");
  Serial.println(player1Score);
  Serial.print("Final Score Player 2: ");
  Serial.println(player2Score);

  servomotor.write(0); // Reset servo motor
  lcd.clear();
  lcd.print("Welcome!");
}
