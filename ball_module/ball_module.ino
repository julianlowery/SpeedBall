#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>

#include "ball_module.h"

// create RF24 object and initialize CE, CSN pins
RF24 radio(2, 3);

// pipe through which the nrf24l01 modules communicate
const byte pipe[6] = "00001";

// slave select pin
int SS_nrf = 3;

// array of bytes for reading from adxl
//char output[512];                                     // what is this for

// x y z axis measurements
float xRaw, yRaw, zRaw = 0; 
float x, y, z = 0;
float sumAcc = 0;

// array to hold acceleration values
float acc_values[200];
bool exceeded_array = false;

// acc_counter for indexing through acc_values
int acc_counter = 0;

// time values for speed calculation
unsigned long long t1, t2 = 0;

// velocity used as final calculation
int throw_speed = 0;
bool failed = false;

//LCD pins
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ------------------------------------------------------------- MAIN --------------------------------------------------------------
void setup()
{
  lcd.begin(16, 2); 
  
  // set output mode for slave select pins
//  pinMode(SS_nrf, OUTPUT);
//  digitalWrite(SS_nrf, HIGH);

/*
 initialize SPI bus and set mode (clock polarity 1, clock phase 1)
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
*/

  // initialize serial monitor
//  Serial.begin(9600);

  Wire.begin();
  // put adxl345 into measurement mode
  Wire.beginTransmission(ACC_ADDRESS);
  Wire.write(POWER_CTL);
  Wire.write(8);
  Wire.endTransmission(); 
  // set range to +/-4G
  Wire.beginTransmission(ACC_ADDRESS);
  Wire.write(DATA_FORMAT);
  Wire.write(0b00000001);
  Wire.endTransmission();
  // set output data rate to 100Hz
  Wire.beginTransmission(ACC_ADDRESS);
  Wire.write(BW_RATE);
  Wire.write(0b00001010);
  Wire.endTransmission();


/*
  // put adxl345 into measurement mode
  writeTo(POWER_CTL, 0b00001000, SS_acc);
  // set range to +/-4G
  writeTo(DATA_FORMAT, 0b00000001, SS_acc);
  // set output data rate to 100Hz
  writeTo(BW_RATE, 0b00001010, SS_acc);
*/



  radio.begin();
  radio.openWritingPipe(pipe);
  // set power output level to minimum
  radio.setPALevel(RF24_PA_MIN);
  // set to write
  radio.stopListening();
}

void loop()
{
  // read acceleration data from each axis
  read_six_reg(DATA_X_LSB, xRaw, yRaw, zRaw);

  // convert reading to m/s^2 and account for individual axis discrepencies
  if(xRaw >= 0)
    x = xRaw/11.119045;
  else
    x = xRaw/10.50698623;

  if(yRaw >= 0)
    y = yRaw/11.119045;
  else
    y = yRaw/11.42514119;

  if(zRaw >= 0)
    z = zRaw/11.119045;
  else
    z = zRaw/14.689321;

  // calculate magnitude of the sum of all axes
  sumAcc = sqrt((x * x) + (y * y) + (z * z));
  // subtract magnitude of g
  sumAcc -= 9.803;

  // enter statement if throw is detected
  if(sumAcc > 15)
  {
    // get initial time
    t1 = millis();
    // set acc_counter to 0
    acc_counter = 0;
    // stay in loop while ball is being accelerated
    while(sumAcc > 5)
    {
      // keep acceleration values in array and increment acc_counter
      acc_values[acc_counter] = sumAcc;
      acc_counter++;

//      Serial.println(acc_counter);

      // Recalculate sumAcc value
      read_six_reg(DATA_X_LSB, x, y, z);
      if(xRaw >= 0)
        x = xRaw/11.119045;
      else
        x = xRaw/10.50698623;
      if(yRaw >= 0)
        y = yRaw/11.119045;
      else
        y = yRaw/11.42514119;
      if(zRaw >= 0)
        z = zRaw/11.119045;
      else
        z = zRaw/14.689321;
      sumAcc = sqrt((x * x) + (y * y) + (z * z));
      sumAcc -= 9.803;

      if(acc_counter >= 200){
        exceeded_array = true;
        break;
      }

      
    }
    // get final time
    t2 = millis();
    // add all acceleration values from array
    unsigned int acc_vals_sum = 0;
    for(int index = 0; index < acc_counter; index++){
      acc_vals_sum += acc_values[index];
    }
    // calculate average acceleration, time, and final velocity of throw
    int acc_avg = acc_vals_sum / acc_counter;
    double throw_time = (t1-t2) / 1000;
    throw_speed = acc_avg * throw_time;

    // send data to display module
//    radio.write(&throw_speed, sizeof(throw_speed));

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TS:");
    lcd.setCursor(4,0);
    lcd.print(throw_speed);
    lcd.setCursor(7,0);
    lcd.print("TT");
    lcd.setCursor(10,0);
    lcd.print(throw_time);
    lcd.setCursor(0,1);
    lcd.print("NUM VALS:");
    lcd.setCursor(10,1);
    lcd.print(acc_counter);

    while(1)
    {}
  }

//  if(!radio.write(&sumAcc, sizeof(sumAcc)))
//  {
//    failed = true;
//  }
  delay(100);

    double throw_time = 12;
  
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("TS:");
//    lcd.setCursor(4,0);
//    lcd.print(throw_speed);
//    lcd.setCursor(7,0);
//    lcd.print("TT");
//    lcd.setCursor(10,0);
//    lcd.print(throw_time);
//    lcd.setCursor(0,1);
//    lcd.print("NUM VALS:");
//    lcd.setCursor(10,1);
//    lcd.print(acc_counter);


 
//  Serial.print(x);
//  Serial.print(" ");
//  Serial.print(y);
//  Serial.print(" ");
//  Serial.print(z);
//  Serial.print("   ");
//  Serial.println(sumAcc);

  
//  Serial.print("   ");
//  Serial.println(throw_speed);
//  delay(500);
}
