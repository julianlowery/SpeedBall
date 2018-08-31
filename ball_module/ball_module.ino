#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// device address
#define ACC_ADDRESS 0x53

// define register numbers from data sheet
#define DEV_ID         0x00 // not needed
#define THRESH_TAP     0x1D // threshold value for tap interupts
#define OFFSET_X       0x1E // offset on x axis
#define OFFSET_Y       0x1F // offset on y axis
#define OFFSET_Z       0x20 // offset on z axis
#define DUR            0x21 // time duration an event must be above THRESH_TAP to qualify as tap event
#define LATENT         0x22 // time gap between tap detection and time window start
#define WINDOW         0x23 // tap window
#define THRESH_ACTl    0x24 // threshold value for detecting activity
#define THRESH_INACT   0x25 // threshold value for detecting inactivity
#define TIME_INACT     0x26 // time that acceleration must be less than THRESH_INACT for inactivity to be declared
#define ACT_INACT_CTL  0x27 // enable/disable axes from detecting activity or inactivity, ac/dc
#define THRESH_FF      0x28 // threshold value for free-fall detection
#define TIME_FF        0x29 // min time values of all axes must be less than THRESH_FF to generate free-fall interupt
#define TAP_AXES       0x2A // enable/disable axes from tap detection
#define ACT_TAP_STATUS 0x2B // bits that indicate first axis involved in a tap or activity event
#define BW_RATE        0x2C // low power, bandwidth, and output data rate
#define POWER_CTL      0x2D // bits for power control
#define INT_ENABLE     0x2E // bits to enable interupts
#define INT_MAP        0x2F // send interupts to INT1 or INT2
#define INT_SOURCE     0x30 // bits to indicate triggering of an event
#define DATA_FORMAT    0x31 // bytes to format data of all axes
#define DATA_X_LSB     0x32 // x axis data, least significant bit
#define DATA_X_MSB     0x33 // x axis data, most significant bit
#define DATA_Y_LSB     0x34 // y axis data, least significant bit
#define DATA_Y_MSB     0x35 // y axis data, most significant bit
#define DATA_Z_LSB     0x36 // z axis data, least significant bit
#define DATA_Z_MSB     0x37 // z axis data, most significant bit
#define FIFO_CTL       0x38 // set data queue (FIFO) mode
#define FIFO_STATUS    0x39 // data queue status

// create RF24 object and initialize CE, CSN pins
RF24 radio(2, 3);

// "pipe" through which the nrf24l01 modules communicate
const byte pipe[6] = "00001";

// slave select pin
int SS_nrf = 3;

// array of bytes for reading from adxl
byte values[6];

// x y z axis measurements
int16_t xRaw, yRaw, zRaw = 0;
float x, y, z = 0;
float sumAcc = 0;

// array to hold acceleration values
float acc_values[100];

// counter for indexing through acc_values
int counter = 0;

// time values for speed calculation
int t1, t2 = 0;

// velocity used as final calculation
int throw_speed = 0;

// --------------------------------------------------------- SPI functions ------------------------------------------------------

//void writeTo(byte reg, byte val, int slave_select)
//{
//  // slave select low
//  digitalWrite(slave_select, LOW);
//  // transfer register to write to
//  SPI.transfer(reg);
//  // write val
//  SPI.transfer(val);
//  // slave select high
//  digitalWrite(slave_select, HIGH);
//}
//
//byte read_reg(byte reg, int slave_select)
//{
//  // read: set msb of reg address
//  reg = reg | 0b10000000;
//  // slave select low
//  digitalWrite(slave_select, LOW);
//  // transfer reg address to be read from
//  SPI.transfer(reg);
//  // read byte
//  byte returnValue = SPI.transfer(0x00);
//  // slave select high
//  digitalWrite(slave_select, HIGH);
//  return returnValue;
//}
//
//// a function allowing for consecutive reads from two sequential registers
//int read_two_reg(byte reg, int slave_select)
//{
//  // read: set msb of reg address
//  byte regAddress = 0b10000000 | reg;
//  // multi-byte read: set bit 6
//  regAddress = 0b01000000 | regAddress;
//
//  // slave select low
//  digitalWrite(slave_select, LOW);
//  // transfer register address to be read
//  SPI.transfer(regAddress);
//  // read bytes, shift msb left 8
//  int returnValue = SPI.transfer(0x00) | SPI.transfer(0x00) << 8;
//  // slave select high
//  digitalWrite(slave_select, HIGH);
//  return returnValue;
//}

// -------------------------------------------------------------- I2C FUNCTIONS -------------------------------------------------------------

void read_six_reg(int reg, int x, int y, int z)
{
  // begin transmission at device
  Wire.beginTransmission(ACC_ADDRESS);
  // queue bytes for transmission to the slave (writing desired register)
  Wire.write(reg);
  // end transmission with the slave and and transmit the bytes queued in write()
  Wire.endTransmission();

  // request bytes from the slave
  Wire.beginTransmission(ACC_ADDRESS);
  Wire.requestFrom(ACC_ADDRESS, 6);
  
  int i = 0;
  while(Wire.available())
  {
    values[i] = Wire.read();
    i++;
  }

  x = ((int)values[1]) << 8 | values[0];
  y = ((int)values[3]) << 8 | values[2];
  z = ((int)values[5]) << 8 | values[4];
}


// ------------------------------------------------------------- MAIN --------------------------------------------------------------
void setup()
{
  // set output mode for slave select pins
  pinMode(SS_nrf, OUTPUT);
  digitalWrite(SS_nrf, HIGH);

  // initialize SPI bus and set mode (clock polarity 1, clock phase 1)
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);

  // initialize serial monitor
  Serial.begin(9600);

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




//  // put adxl345 into measurement mode
//  writeTo(POWER_CTL, 0b00001000, SS_acc);
//  // set range to +/-4G
//  writeTo(DATA_FORMAT, 0b00000001, SS_acc);
//  // set output data rate to 100Hz
//  writeTo(BW_RATE, 0b00001010, SS_acc);




//  radio.begin();
//  radio.openWritingPipe(pipe);
//  // set power output level to minimum
//  radio.setPALevel(RF24_PA_MIN);
//  // set to write
//  radio.stopListening();
}

void loop()
{
  // read acceleration data from each axis
  read_six_reg(DATA_X_LSB, x, y, z);

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
  if(sumAcc > 100)
  {
    // get initial time
    t1 = millis();
    // set counter to 0
    counter = 0;
    // stay in loop while ball is being accelerated
    while(sumAcc > 4)
    {
      // keep acceleration values in array and increment counter
      acc_values[counter] = sumAcc;
      counter++;
    }
    // get final time
    t2 = millis();
    // add all acceleration values from array
    int acc_vals_sum = 0;
    for(int index = 0; index < counter; index++)
    {
      acc_vals_sum += acc_values[index];
    }
    // calculate average acceleration, time, and final velocity of throw
    int acc_avg = acc_vals_sum / counter;
    int throw_time = (t1-t2) * 1000;
    throw_speed = acc_avg * throw_time;

    // send data to display module
    radio.write(&throw_speed, sizeof(throw_speed));
  }
  
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(z);
  Serial.print("   ");
  Serial.print(sumAcc);
  Serial.print("   ");
  Serial.println(throw_speed);
}

























