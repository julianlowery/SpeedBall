#include <LiquidCrystal.h>
#include <SPI.h>

#include "NRF24L01.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  1  // 32 unsigned chars TX payload
unsigned char RX_ADDRESS[TX_ADR_WIDTH]  = {0x34, 0x43, 0x10, 0x10, 0x01};
unsigned char rx_buf[TX_PLOAD_WIDTH] = {0};

int print_val = 0;

//LCD pins
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  NRF_Init();                        // Initialize IO     
  NRF_SetRxMode();
  lcd.begin(16, 2);
}

void loop()
{
  NRF_SetRxMode();
  lcd.setCursor(0,0);
  lcd.print("Throw Speed:");
  lcd.setCursor(0, 1);

  if(NRF_Receive(rx_buf))
  {
    print_val = rx_buf[0];
    rx_buf[0] = 0;
  }
  lcd.print(print_val);
  lcd.setCursor(3 ,1);
  lcd.print("m/s");
}
