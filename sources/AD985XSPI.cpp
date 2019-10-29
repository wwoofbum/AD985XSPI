/********************************************************************************************
 * Arduino library for AD9850 and AD9851
 * Created 23/08/2014
 * Christophe Caiveau f4goj@free.fr 
 * Upgrade to SPI Anthony F4GOH
 *
 * Updated Version: Farrukh Zia, K2ZIA 2018_0101 
 * - Updated library functions to comply with latest SPI library in IDE v1.8.5
 * - Implemented correct SPI bus initialization sequence
 * - Implemented correct AD985x initialization sequence
 * - Added support for AD9851 (30 MHz oscillator, 180 MHz system clock)
 * - Increased SPI-DDS bus speed to 8MHz (default) (other options are 4MHz and 2MHz)
 *
 * This library uses the Serial Peripheral Interface (SPI) 
 * to accelerate the update of the AD985x from 700µs in software serial to:
 * - 90µs (54µs for deltaphase calculation and 36µs for transfer) @ 2MHz SPI bus speed
 * - 68µs (54µs for deltaphase calculation and 14µs for transfer) @ 8MHz SPI bus speed (default)
 *
 * Use this library freely
 *
 * Hardware connections : 
 * W_CLK   -> D13 arduino UNO/NANO, D52 MEGA
 * FQ_UD   -> any pin except 10 and 12 UNO/NANO, 50 and 53 MEGA
 * DATA/D7 -> D11 arduino UNO/NANO, D51 MEGA
 * RESET   -> any pin except 10 and 12 UNO/NANO, 50 and 53 MEGA
 *
 * Functions :
 * DDS.begin(W_CLK pin, FQ_UD pin, RESET pin); initialize the output pins and master reset AD985x
 * DDS.calibrate(trimFreq); compensation of crystal oscillator frequency
 * DDS.setfreq(frequency); frequency in Hz
 * DDS.down(); power down mode reducing the dissipated power from 380mW to 30mW @5V
 * DDS.up(); wake-up the AD985x from power down mode
 *
 * AD9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * AD9851 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9851.pdf
 *
 *******************************************************************************************/

#include <AD985XSPI.h>
#include <SPI.h>

//#define SPIRate 2000000 // 2Mbit/s
//#define SPIRate 4000000 // 4Mbit/s
#define SPIRate 8000000 // 8Mbit/s
 
AD985XSPI::AD985XSPI(uint8_t ddsType) {
	// set ddsType
	DDS_TYPE = ddsType;
    // Ensure that the SPI hardware is initialised,
    // setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high
	SPI.begin();
}

void AD985XSPI::begin(int w_clk_pin, int fq_ud_pin, int reset_pin) {
	W_CLK = w_clk_pin;
	FQ_UD = fq_ud_pin;
	RESET = reset_pin;

	if (DDS_TYPE == 0) { // AD9850
		deltaphase = 0;
		phase = 0x00;
		calibFreq = 125000000;
	}	

	if (DDS_TYPE == 1) { // AD9851
		deltaphase = 0;
		phase = 0x01;
		calibFreq = 180000000;
	}

	pinMode(W_CLK, OUTPUT);
	pinMode(FQ_UD, OUTPUT);
	pinMode(RESET, OUTPUT);

	reSet();
}

void AD985XSPI::reSet() {
    pulse(RESET);   // (minimum 5 cycles of the system clock)
    pulse(W_CLK);   // enable serial loading mode
    pulse(FQ_UD);	// ... enable serial loading mode

    // To avoid possible false configuration settings, immediately set the frequency
    setfreq(1);
}

void AD985XSPI::setfreq(double freq) {
	deltaphase = freq * 4294967296.0 / calibFreq;
	update();
}

void AD985XSPI::update() {
	SPI.beginTransaction(SPISettings(SPIRate, LSBFIRST, SPI_MODE0));
	for (int i=0; i<4; i++, deltaphase>>=8) {
		SPI.transfer(deltaphase & 0xFF);
	}
    // The last byte contains configuration settings including phase
	SPI.transfer(phase & 0xFF);

	pulse(FQ_UD);      // Transfer the 40-bit control word into the DDS core
    SPI.endTransaction();
}

void AD985XSPI::calibrate(double trimFreq) {
	calibFreq = trimFreq;
}

void AD985XSPI::down() {
	pulse(FQ_UD);
	SPI.beginTransaction(SPISettings(SPIRate, LSBFIRST, SPI_MODE0));
	SPI.transfer(0x04);
    SPI.endTransaction();
	pulse(FQ_UD);
}

void AD985XSPI::up() {
	update();
}

void AD985XSPI::pulse(int pin) {
	digitalWrite(pin, HIGH);
	digitalWrite(pin, LOW);
}
