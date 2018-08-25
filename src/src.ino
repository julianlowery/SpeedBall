#include <SPI.h>

// slave select pin
int SS = 10;

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
#define THRESH_FF      0x29 // 

void setup()
{
  // set output mode for slave select pin
  pinMode(SS, OUTPUT);

}

void loop()
{


}
