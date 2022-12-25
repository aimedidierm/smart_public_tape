#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

int Interrupt = 1;
int sensorPin       = 3;
int Valve = 6;
const int red =  4;
const int buzzer =  2;
float calibrationFactor = 90;
volatile byte pulseCount = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 270;
unsigned long oldTime = 0;
String data = "";
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
int outml = 0;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char newNum[12] = "", amount[12] = "", kwishyuraamount[12] = "", kwiyaboneshaamount[12] = "";
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {

  {'1', '2', '3'},

  {'4', '5', '6'},

  {'7', '8', '9'},

  {'*', '0', '#'}

};

byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7}; //connect to the column pinouts of the keypad

String card;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int drink = 0, drinkvolume = 0;


void setup()
{
  lcd.init();                      // initialize the lcd
  lcd.init();
  SPI.begin();
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(Valve , OUTPUT);
  digitalWrite(Valve, HIGH);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  attachInterrupt(Interrupt, pulseCounter, FALLING);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Smart public");
  lcd.setCursor(5, 1);
  lcd.print("tape");
  delay(3000);

}

void loop()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hitamo uburyo");
  lcd.setCursor(0, 1);
  lcd.print("bwo kwishyura");
  paymentmethod();
}

void paymentmethod() {
  unsigned int method = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. Na MOMO");
  lcd.setCursor(0, 1);
  lcd.print("2. N'ikarita");
  int key = keypad.getKey();
  if (key == '1') {
    momo();
  }
  if (key == '2') {
    readcard();
  }
  delay(100);
  paymentmethod();
}

void momo() {
  int i = 0, j = 0, m = 0, x = 0, s = 0, k = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Injiza nimero");
  lcd.setCursor(0, 1);
  lcd.print("ukande #");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Injiza nimero");
  for (i = 2; i > 0; i++) {
    lcd.setCursor(0, 1);
    int key = keypad.getKey();
    if (key != NO_KEY && key != '#' && key != '*') {
      newNum[j] = key;
      newNum[j + 1] = '\0';
      j++;
      lcd.setCursor(0, 1);
      lcd.print(newNum);
    }
    if (key == '#' && j > 0)
    {
      j = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Amafaranga:");
      lcd.setCursor(0, 1);
      for ( int d = 2; d > 1; d++) {
        int key = keypad.getKey();
        if (key != NO_KEY && key != '#' && key != '*') {
          amount[j] = key;
          amount[j + 1] = '\0';
          j++;
          lcd.setCursor(0, 1);
          lcd.print(amount);
        }
        if (key == '#' && j > 0)
        {
          j = 0;
          lcd.clear();
          lcd.print("Tegereza");
          Serial.println("{'phone':'07835', 'amount': 100}");
          //Serial.println((String)"?phone=" + newNum + "&amount=" + amount); //kohereza data kurinodemcu
          while (k == 0) {
            if (Serial.available() > 0) {
              data = Serial.readStringUntil('\n');
              //Serial.println(data);
              DynamicJsonBuffer jsonBuffer;
              JsonObject& root = jsonBuffer.parseObject(data);
              if (root["outml"]) {
                outml = root["outml"];
                if (outml == 1) {
                  lowbalance();
                } else {
                  drinkvolume = outml;
                  drinkout();
                }
              }
            }
            //delay(10000);
          }
        }
        delay(100);
      }
    }
    delay(100);
  }
}

void(* resetFunc) (void) = 0;

void drinkout() {
  digitalWrite(Valve, LOW);
  while (drinkvolume > 20) {
    if ((millis() - oldTime) > 1000)   // Only process counters once per second
    {
      detachInterrupt(Interrupt);
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      oldTime = millis();
      flowMilliLitres = (flowRate / 60) * 1000;
      drinkvolume -= flowMilliLitres;

      unsigned int frac;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Murakoze!");
      lcd.setCursor(0, 1);
      lcd.print(drinkvolume);
      pulseCount = 0;
      attachInterrupt(Interrupt, pulseCounter, FALLING);
    }
  }
  digitalWrite(Valve, HIGH);
  resetFunc();
  delay(3000);
  resetFunc();
}

void readcard() {
  // Look for new cards
  int i = 0, j = 0, m = 0, x = 0, s = 0, money = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kozaho");
  lcd.setCursor(0, 1);
  lcd.print("Ikarita");
  delay(500);
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    readcard();
    //return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    readcard();
    //return;
  }
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  card = content.substring(1);
  for ( int d = 2; d > 1; d++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1. Kwishyura");
    lcd.setCursor(0, 1);
    lcd.print("2. Kwiyabonesha");
    int key = keypad.getKey();
    if (key == '1') {
      kwishyura();
    }
    if (key == '2') {
      kwiyabonesha();
    }
    delay(100);
  }
}
void kwishyura() {
  int j = 0, k = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Amafaranga:");
  for ( int d = 2; d > 1; d++) {
    int key = keypad.getKey();
    if (key != NO_KEY && key != '#' && key != '*') {
      kwishyuraamount[j] = key;
      kwishyuraamount[j + 1] = '\0';
      j++;
      lcd.setCursor(0, 1);
      lcd.print(kwishyuraamount);
    }
    if (key == '#' && j > 0)
    {
      j = 0;
      lcd.clear();
      lcd.print("Tegereza");
      card.replace(" ", "");
      Serial.println((String)"?card=" + card + "&kwishyuraamount=" + kwishyuraamount); //kohereza data kurinodemcu
      while (k == 0) {
        if (Serial.available() > 0) {
          data = Serial.readStringUntil('\n');
          Serial.println(data);
          DynamicJsonBuffer jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(data);
          if (root["outml"]) {
            outml = root["outml"];
            if (outml == 1) {
              lowbalance();
            } else {
              drinkvolume = outml;
              drinkout();
            }
          }
        }
      }
    }
    delay(100);
  }
}
void kwiyabonesha() {
  int j = 0, k = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Amafaranga:");
  for ( int d = 2; d > 1; d++) {
    int key = keypad.getKey();
    if (key != NO_KEY && key != '#' && key != '*') {
      kwiyaboneshaamount[j] = key;
      kwiyaboneshaamount[j + 1] = '\0';
      j++;
      lcd.setCursor(0, 1);
      lcd.print(kwiyaboneshaamount);
    }
    if (key == '#' && j > 0)
    {
      j = 0;
      lcd.clear();
      lcd.print("Tegereza");
      Serial.println((String)"?card=" + card + "&kwiyaboneshaamount=" + kwiyaboneshaamount); //kohereza data kurinodemcu
      while (k == 0) {
        if (Serial.available() > 0) {
          data = Serial.readStringUntil('\n');
          Serial.println(data);
          DynamicJsonBuffer jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(data);
          if (root["outml"]) {
            outml = root["outml"];
            if (outml == 1) {
              lowbalance();
            } else {
              drinkvolume = outml;
              drinkout();
            }
          }
        }
      }
    }
    delay(100);
  }
}
void lowbalance() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Amafaranga");
  lcd.setCursor(0, 1);
  lcd.print("Ntahagije");
  digitalWrite(red, HIGH);
  tone(buzzer, 1000, 1000);
  delay(3000);
  digitalWrite(red, LOW);
  lcd.clear();
  resetFunc();
}
void pulseCounter()
{
  pulseCount++;
}
