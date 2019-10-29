/* AD9850SPI_Test.ino

   F.Zia (K2ZIA) 2018_0101

   AD985XSPI library usage:
   DDS object and following functions are defined in the library

   // assign DDS module pins: W_CLK, FQ_UD, RESET
   DDS.begin(W_CLK_pin, FQ_UD_pin, RESET_pin)     

   // set DDS output to desired frequency in HZ with phase = 0
   DDS.setfreq(frequency) 

   // calibrate DDS module by using the value assigned to trimFreq
   DDS.calibrate(trimFreq) 

   // put DDS module into power down mode
   DDS.down()

   // wake-up DDS module from power down mode
   // this function is optional since calling DDS.setfreq() will also wake up DDS module
   DDS.up()        
*/

#include <AD985XSPI.h>
#include <SPI.h>

// uncomment folowing line for AD9850 module
#define DDS_TYPE 0
// uncomment folowing line for AD9851 module
// #define DDS_TYPE 1

AD985XSPI DDS(DDS_TYPE);

// Arduino UNO pins used
// const int W_CLK_PIN = 13;
// const int FQ_UD_PIN = 9;
// const int RESET_PIN = 8;

// Arduino MEGA pins used
const int W_CLK_PIN = 52;
const int FQ_UD_PIN = 48;
const int RESET_PIN = 49;

// put your frequency calibration value here
// as measured with an accurate frequency counter
const double trimFreq = 125000786; 

double freq1 = 7000000;
double freq2 = 14000000;
double freq3 = 21000000;

void setup() {
  DDS.begin(W_CLK_PIN, FQ_UD_PIN, RESET_PIN);

  // uncomment the folowing line to apply frequency calibration
//  DDS.calibrate(trimFreq); 
}

void loop() {
  DDS.setfreq(freq1);
  delay(5000);
  DDS.setfreq(freq2);
  delay(5000);
  DDS.setfreq(freq3);
  delay(5000);
}
