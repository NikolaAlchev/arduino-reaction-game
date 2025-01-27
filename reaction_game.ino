#include <LiquidCrystal.h>

int Contrast = 75;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int PIN_RED = 9;
const int PIN_GREEN = 10;
const int PIN_BUTTON_1 = 7;
const int PIN_BUTTON_2 = 8;

void setColor(int R, int G) {
  analogWrite(PIN_RED, R);
  analogWrite(PIN_GREEN, G);
}

void displayCentered(const char* line1, const char* line2) {
  lcd.clear();
  if (line1 != nullptr) {
    int padding1 = (16 - strlen(line1)) / 2;
    for (int i = 0; i < padding1; i++) lcd.print(" ");
    lcd.print(line1);
  }
  if (line2 != nullptr) {
    lcd.setCursor(0, 1);
    int padding2 = (16 - strlen(line2)) / 2;
    for (int i = 0; i < padding2; i++) lcd.print(" ");
    lcd.print(line2);
  }
}

void clearRow(int row) {
  lcd.setCursor(0, row);
  lcd.print("                ");
  lcd.setCursor(0, row);
}

void blink(){
  int i = 0;
  while(i < 3){
    setColor(0, 255);
    delay(100);
    setColor(0, 0);
    delay(100);
    i++;
  }
}

void waitForPress(){
  delay(200);
  while(digitalRead(PIN_BUTTON_1) == LOW && digitalRead(PIN_BUTTON_2) == LOW){
  }
  delay(200);
}

void waitForButtonsRelease(){
  while(digitalRead(PIN_BUTTON_1) == HIGH || digitalRead(PIN_BUTTON_2) == HIGH){
  }
}

void startGame(int players, int playerButton) {
  int randNumber = random(3, 10);

  setColor(255, 255);
  displayCentered("Get Ready!", nullptr);

  unsigned long startTime = millis();
  while (millis() - startTime < randNumber * 1000) {
    if (players == 2 && (digitalRead(PIN_BUTTON_1) == HIGH || digitalRead(PIN_BUTTON_2) == HIGH)) {
      if(digitalRead(PIN_BUTTON_1) == HIGH){
        setColor(255, 0);
        displayCentered("Game Over!", "Player 1 Lost");
        waitForButtonsRelease();
        waitForPress();
        return;
      }

      if(digitalRead(PIN_BUTTON_2) == HIGH){
        setColor(255, 0);
        displayCentered("Game Over!", "Player 2 Lost");
        waitForButtonsRelease();
        waitForPress();
        return;
      }
      
    }
    if (players == 1 && digitalRead(playerButton) == HIGH) {
      setColor(255, 0);
      displayCentered("Game Over!", "Too Early");
      waitForButtonsRelease();
      waitForPress();
      return;
    }
  }

  setColor(0, 255);
  displayCentered("Go!", "Press Now!");

  unsigned long greenTime = millis();

  while (true) {
    if (players == 1 && digitalRead(playerButton) == HIGH) {
      unsigned long reactionTime = millis() - greenTime;
      char buffer[16];
      sprintf(buffer, "Time: %lums", reactionTime);
      displayCentered("Your Time!", buffer);
      blink();
      break;
    }

    if (players == 2) {
      if (digitalRead(PIN_BUTTON_1) == HIGH) {
        unsigned long reactionTime = millis() - greenTime;
        char buffer[16];
        sprintf(buffer, "P1: %lums", reactionTime);
        displayCentered("Player 1 Wins!", buffer);
        blink();
        break;
      }

      if (digitalRead(PIN_BUTTON_2) == HIGH) {
        unsigned long reactionTime = millis() - greenTime;
        char buffer[16];
        sprintf(buffer, "P2: %lums", reactionTime);
        displayCentered("Player 2 Wins!", buffer);
        blink();
        break;
      }
    }
  }

  waitForButtonsRelease();
  waitForPress();
}

int selectPlayers() {
  lcd.clear();

  displayCentered("Press Button", "1 or 2 to Start");

  while (true) {
    if (digitalRead(PIN_BUTTON_1) == HIGH) {
      waitForButtonsRelease();
      clearRow(1);
      lcd.setCursor(0, 1);
      lcd.print("P1");

      while (true) {
        if (digitalRead(PIN_BUTTON_1) == HIGH) {
          waitForButtonsRelease();
          delay(200);
          return 1; // Single Player (Button 1)
        }
        if (digitalRead(PIN_BUTTON_2) == HIGH) {
          waitForButtonsRelease();
          lcd.setCursor(14, 1);
          lcd.print("P2");
          waitForPress();
          return 2; // Two Players
        }
      }
    }

    if (digitalRead(PIN_BUTTON_2) == HIGH) {
      waitForButtonsRelease();
      clearRow(1);
      lcd.setCursor(14, 1);
      lcd.print("P2");

      while (true) {
        if (digitalRead(PIN_BUTTON_2) == HIGH) {
          waitForButtonsRelease();
          delay(200);
          return 3; // Single Player (Button 2)
        }
        if (digitalRead(PIN_BUTTON_1) == HIGH) {
          waitForButtonsRelease();
          lcd.setCursor(0, 1);
          lcd.print("P1");
          waitForPress();
          return 2; // Two Players
        }
      }
    }
  }
}

void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  analogWrite(6, Contrast);
  lcd.begin(16, 2);
  randomSeed(analogRead(A0));

  delay(1000);
}

void loop() {
  int mode = selectPlayers();
  waitForButtonsRelease();
  delay(500);
  if (mode == 1) {
    startGame(1, PIN_BUTTON_1); // Single Player, Button 1
  } else if (mode == 3) {
    startGame(1, PIN_BUTTON_2); // Single Player, Button 2
  } else {
    startGame(2, -1); // Two Players
  }
}
