#include <LiquidCrystal.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

// create RF24 object and initialize CSN, CE pins
RF24 radio(2, 3);

// "pipe" through which the nrf24l01 modules communicate
const byte pipe[6] = "00001";

// speed to display on LCD
int throw_speed = 0;

//LCD pins
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  lcd.begin(16, 2);
  radio.begin();
  radio.openReadingPipe(0, pipe);
  // set power output level to minimum
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop()
{
  lcd.setCursor(0,0);
  lcd.print("Throw Speed:");
  lcd.setCursor(0, 1);
  lcd.print(throw_speed);
  if(radio.available())
  {
    radio.read(&throw_speed, sizeof(throw_speed));
    lcd.clear();
  }

}
