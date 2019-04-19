#include "pitches.h"
#include <Servo.h>

Servo ufoServo;
int ufoPos = 90;

int correctMelody[] = { NOTE_C5, NOTE_G5 };
int wrongMelody[] = { NOTE_A2, NOTE_A2 };
int moveMelody[] = { NOTE_C4, NOTE_A4 };
int shortDuration[] = {4, 4};

int startMelody[] = { NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5 };
int winMelody[] = { NOTE_C6, NOTE_E6, NOTE_D6, NOTE_F6 };
int failMelody[] = { NOTE_G3, NOTE_E3, NOTE_E3, NOTE_E3 };
int longDuration[] = {2, 2, 4, 2};

boolean myTurn = false; 
boolean canReplay = true;

int chances = 0;

boolean lastResult = true; // true means win, false means loss

const int leftLedPin = 13;
const int rightLedPin = 3;

const int leftButtonPin = 2;
const int rightButtonPin = 4;
const int resetButtonPin = 12; 

const int myTurnLedPin = 11;
const int cpuTurnLedPin = 10;

const int fullLifeLedPin = 6;
const int someLifeLedPin = 7;
const int lowLifeLedPin = 9;

int leftButtonState = 0;
int rightButtonState = 0;
int resetButtonState = 0; 

int i = 0; // increment
int seqLength = 2;
int sequence[11];

void setup() {
  Serial.begin(9600); // initializing serial communication

  // adding LED and button for the left control
  pinMode(leftLedPin, OUTPUT);
  pinMode(leftButtonPin, INPUT);

  // adding LED and button for the right control
  pinMode(rightLedPin, OUTPUT);
  pinMode(rightButtonPin, INPUT);

  pinMode(myTurnLedPin, OUTPUT);
  pinMode(cpuTurnLedPin, OUTPUT);

  pinMode(resetButtonPin, INPUT);
  
  pinMode(lowLifeLedPin, OUTPUT);
  pinMode(someLifeLedPin, OUTPUT);
  pinMode(fullLifeLedPin, OUTPUT);

  ufoServo.attach(5);
  ufoServo.write(90);
}


/**
 * controls function uses LEDs to show if the left button is clicked 
 * or the right button is clicked. You can only click the button one 
 * at a time. 
 * it returns 1 for left button, 2 for right button, 3 to play again
 */

int controls() {
  if (digitalRead(leftLedPin) == 0 && digitalRead(rightLedPin) == 0) {
    
    if (leftButtonState == HIGH && rightButtonState == LOW) {
      // Serial.println("Left button has been pressed.");
      digitalWrite(leftLedPin, HIGH);
      return 1;
    }
  
    if (rightButtonState == HIGH && leftButtonState == LOW) {
      // Serial.println("Right button has been pressed.");
      digitalWrite(rightLedPin, HIGH);
      return 2;
    }

    if (resetButtonState == HIGH && canReplay) {
      return 3;
    }
    
  }
  else {
    digitalWrite(leftLedPin, LOW);
    digitalWrite(rightLedPin, LOW);
  }
  return 0;
}

void playTune(int tune[], int arrLength) {
  for (int note=0; note < arrLength;note++) {
    int duration = 1000;
    if (arrLength > 2) {
      duration /= longDuration[note];
    }
    else {
      duration /= shortDuration[note];
    }
    tone(8, tune[note], duration);
    int rest = duration * 1.30;
    delay(rest);
    noTone(8);
  }
}

void generateSequence() {
  for (int j=0;j < seqLength;j++) {
    sequence[j] = random(2) + 1;
  }
  runSequence();
  myTurn = true;
  
  Serial.println("Sequence has been generated.");
}

void runSequence() {
  for (int j=0; j < seqLength; j++) {
    Serial.println((String)(sequence[j]));
    turnUFO(sequence[j]);
    playTune(moveMelody, 2);
    delay(200);
  }
}

void turnUFO(int dir) {
  if (dir == 1) { // turn left
    for (ufoPos = 90; ufoPos <= 110; ufoPos += 1) {
      ufoServo.write(ufoPos);
      delay(15);
    }
    delay(100);
    for (ufoPos = 110; ufoPos >= 90; ufoPos -= 1) {
      ufoServo.write(ufoPos);
      delay(15);
    }
  }
  else {
   for (ufoPos = 90; ufoPos >= 70; ufoPos -= 1) {
      ufoServo.write(ufoPos);
      delay(15);
    }
    delay(100);
    for (ufoPos = 70; ufoPos <= 90; ufoPos += 1) {
      ufoServo.write(ufoPos);
      delay(15);
    }
  }
}


void check(int answer) {
    if (answer != 0) {
      if (answer == 3 && canReplay) {
        canReplay = false; 
        runSequence(); // if its 3, just rerun the sequence 
      }
      else if (i < seqLength && answer == sequence[i]) {
          i += 1;
          turnUFO(answer);
          playTune(moveMelody, 2);
          Serial.println("Correct.");
          if (seqLength == i) {
            Serial.println("You got them all!");
            playTune(correctMelody, 2);
            i = 0;
            myTurn = false;
            lastResult = true;
            canReplay = true;
          }
      }
      else {
        turnUFO(answer);
        Serial.println("Wrong.");
        playTune(wrongMelody, 2);
        chances += 1;
        i = 0;
        myTurn = false;
        lastResult = false;
        canReplay = true; 
      }
  }
}

void loop() {
  
  leftButtonState = digitalRead(leftButtonPin);
  rightButtonState = digitalRead(rightButtonPin);
  resetButtonState = digitalRead(resetButtonPin);
  int whichClicked = controls();

  // turns on the remaining LIVES leds
  if (chances == 0) {
    digitalWrite(fullLifeLedPin, HIGH);
    digitalWrite(someLifeLedPin, HIGH);
    digitalWrite(lowLifeLedPin, HIGH);
  }
  else if (chances == 1) {
    digitalWrite(fullLifeLedPin, LOW);
  }
  else if (chances == 2) {
    digitalWrite(someLifeLedPin, LOW);
  }
  else {
    digitalWrite(lowLifeLedPin, LOW);
  }

  if (chances < 3) {
    if (myTurn) {
      check(whichClicked);
      digitalWrite(myTurnLedPin, HIGH);
      digitalWrite(cpuTurnLedPin, LOW);
    }
    else {
      digitalWrite(cpuTurnLedPin, HIGH);
      digitalWrite(myTurnLedPin, LOW);
      delay(800);
      
      if (lastResult && seqLength < 11) { // 11 is the set amount of space reserved for the array
        seqLength += 1;
      }

      if (seqLength == 3) {
        playTune(startMelody, 4);
      }
      
      if (seqLength < 11) {
        generateSequence();
      }
      else {
        Serial.println("You win!");
        playTune(winMelody, 4);
        chances = 0;
        seqLength = 2;
        delay(100);
      }
    }
  }
  else {
    Serial.println("Game over...");
    playTune(failMelody, 4);
    chances = 0;
    seqLength = 3;
    delay(100);
  }

  
  
  
  delay(175);

  
  

}
