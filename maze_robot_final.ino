#include <NewPing.h>
#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

#define MLa 3
#define MLb 9
#define MRa 6
#define MRb 5
#define trigpin 11
#define echopin 10
#define maximum_distance 500
#define buttonPin 2

boolean movestraight = false;
int distance = 100;
int buttonState = 0;
int lastButtonState = LOW;
int buttonPressCount = 0;
int lcount = 0;
int rcount = 0;
NewPing sonar(trigpin, echopin, maximum_distance);
Servo servo_pin;

void setup() {
    lcd.begin(16,2);
    lcd.backlight();
    pinMode(MLa, OUTPUT);
    pinMode(MLb, OUTPUT);
    pinMode(MRa, OUTPUT);
    pinMode(MRb, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    servo_pin.attach(7);
    servo_pin.write(115);
    delay(500);
    distance = readPing(); 
    Serial.begin(9600);
}

void loop() {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH && lastButtonState == LOW) {
        buttonPressCount++;
        delay(300); // Debounce delay
    }
    lastButtonState = buttonState;

    if (buttonPressCount == 1) {
        moveforward();
    } else if (buttonPressCount == 2) {
        movestop();
    } else if (buttonPressCount == 3) {
        disright();
    } else if (buttonPressCount == 4) {
        disleft();
        buttonPressCount = 0; // Reset cycle
    }

    if (buttonPressCount == 1) {
        int distanceright=0;
        int distanceleft=0;
        distance = readPing();
        Serial.println(distance);
        if (distance <= 6) {
            movestop();
            delay(300);
            movebackward();
            delay(80);
            movestop();
            delay(300);
            int distanceright = lookright();
            delay(300);
            int distanceleft = lookleft();
            delay(300);
            if (distanceleft > 20) {
                turnleft();
                delay(180);
                lcount++;
                movestop();
            } else if (distanceright > 20) {
                turnright();
                delay(180);
                rcount++;
                movestop();
            }
        } else {
            moveforward();
        }
    }
}

void disright() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Right Turns:");
    lcd.setCursor(0,1);
    lcd.print(rcount);
    Serial.println("RIGHT COUNT");
    Serial.println(rcount);
}

void disleft() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Left Turns:");
    lcd.setCursor(0,1);
    lcd.print(lcount);
    Serial.println("LEFT COUNT");
    Serial.println(lcount);
    
}

int lookright() {
    servo_pin.write(10);
    delay(400);
    int distance = readPing();
    delay(50);
    servo_pin.write(110);
    return distance;
}

int lookleft() {
    servo_pin.write(180);
    delay(400);
    int distance = readPing();
    delay(50);
    servo_pin.write(110);
    return distance;
}

int readPing() {
    delay(25);
    int cm = sonar.ping_cm();
    return cm == 0 ? 250 : cm;
}

void movestop() {
    digitalWrite(MRa, LOW);
    digitalWrite(MRb, LOW);
    digitalWrite(MLa, LOW);
    digitalWrite(MLb, LOW);
}

void moveforward() {
    if (!movestraight) {
        movestraight = true;
        analogWrite(MRa, 72);
        digitalWrite(MRb, LOW);
        analogWrite(MLa, 60);
        digitalWrite(MLb, LOW);
    }
}

void movebackward() {
    movestraight = false;
    digitalWrite(MRa, LOW);
    digitalWrite(MRb, HIGH);
    digitalWrite(MLa, LOW);
    digitalWrite(MLb, HIGH);
}

void turnright() {
    digitalWrite(MRa, LOW);
    digitalWrite(MRb, LOW);
    digitalWrite(MLa, HIGH);
    digitalWrite(MLb, LOW);
}

void turnleft() {
    digitalWrite(MRa, HIGH);
    digitalWrite(MRb, LOW);
    digitalWrite(MLa, LOW);
    digitalWrite(MLb, LOW);
}
