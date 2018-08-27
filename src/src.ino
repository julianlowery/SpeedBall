#include <SPI.h>

// define register numbers from data sheet

#define DEV_ID         0x00 // device address not needed for SPI
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

// --------------------------------------------------------- SPI functions ------------------------------------------------------

void writeTo(byte reg, byte val)
{
  // slave select low
  digitalWrite(SS, LOW);
  // transfer register to write to
  SPI.transfer(reg);
  // write val
  SPI.transfer(val);
  // slave select high
  digitalWrite(SS, HIGH);
}

byte readByte(byte reg)
{
  // read: set msb of reg address
  reg = reg | 0b10000000;
  // slave select low
  digitalWrite(SS, LOW);
  // transfer reg address to be read from
  SPI.transfer(reg);
  // read byte
  byte returnValue = SPI.transfer(0x00);
  // slave select high
  digitalWrite(SS, HIGH);
  return returnValue;
}

// a function allowing for consecutive reads from two sequential registers
int read_two_reg(byte reg)
{
  // read: set msb of reg address
  byte regAddress = 0b10000000 | reg;
  // multi-byte read: set bit 6
  regAddress = 0b01000000 | regAddress;

  // slave select low
  digitalWrite(SS, LOW);
  // transfer register address to be read
  SPI.transfer(regAddress);
  // read bytes, shift msb left 8
  int returnValue = SPI.transfer(0x00) | SPI.transfer(0x00) << 8;
  // slave select high
  digitalWrite(SS, HIGH);
  return returnValue;
}


// ------------------------------------------------------------- MAIN --------------------------------------------------------------
void setup()
{
  // slave select pin
  int SS = 10;
  // set output mode for slave select pin
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  // initialize SPI bus and set mode (clock polarity 1, clock phase 1)
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);

  // initialize serial monitor
  Serial.begin(9600);
}

void loop()
{
  

}

























