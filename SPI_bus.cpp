/* 
 * File:   SPI_bus.cpp
 * Author: mdreamer
 * 
 * Created on May 20, 2017, 8:51 PM
 */

#include "SPI_bus.h"
#include "config.h"
#include <bcm2835.h>
#include <iostream>
using namespace std;

/* starts the SPI interface module, also configures the GPIO pins that will allow
 * control over the ChipEnable pins
*/

SPI_bus::SPI_bus() {
}

void SPI_bus::wrap_datagram(char (&_datagram)[3], int _channel)
{
    char _send_data[3];
    //prepare the datagram for sending
    _datagram[0] = 1;  //  first byte transmitted -> start bit
    _datagram[1] = 0b10000000 |( ((_channel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
    _datagram[2] = 1; // third byte transmitted....don't care

}

int SPI_bus::unwrap_datagram(char _datagram[3])
{
   //data[0] first byte of the response - don't care
   int a2dVal = 0;
   a2dVal = (_datagram[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
   a2dVal |=  (_datagram[2] & 0xff); 
   
   return a2dVal;
}

/************************************************************
* call the bcm2835_spi_transfern function on the a2d object. Each time make sure
* that conversion is configured for single ended conversion on CH0
* i.e. transmit ->  byte1 = 0b00000001 (start bit)
*                   byte2 = 0b1000000  (SGL/DIF = 1, D2=D1=D0=0)
*                   byte3 = 0b00000000  (Don't care)
*      receive  ->  byte1 = junk
*                   byte2 = junk + b8 + b9
*                   byte3 = b7 - b0
*    
* after conversion must merge data[1] and data[2] to get final result
* *********************************************************************/
int SPI_bus::get_piezo_reading(int _slave, int _channel)
{
    //temporary var for handling the bus
    char _send_data[3];
    
    wrap_datagram(_send_data,_channel);
    
    //send the data + HIGH-LOW the relevant pin
    //CSpin is a lookup table of slave-pin mapping - check config file
    bcm2835_gpio_write(CSpin[_slave], LOW);
    bcm2835_spi_transfern(_send_data,3);
    bcm2835_gpio_write(CSpin[_slave], HIGH);
    
    return unwrap_datagram(_send_data);
}

int SPI_bus::startSPI()
{
    if (debug)
        cout << "Initializing SPI..." << endl;
    if (!bcm2835_init())
    {
        return -1;
    }
    
    // Set the all the pins to be an output and hold it high for SPI use
    //CSpin is a lookup table of slave-pin mapping - check config file
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        bcm2835_gpio_fsel(CSpin[i], BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(CSpin[i], HIGH);
    }
    
    // start/init the SPI
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); //Data comes in on falling edge
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); //250MHz / 256
    //No CS/CE, Control the SPI slave manually thru the GPIO pins
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); 
    if (debug)
        cout << "SPI is up!" << endl;
}
SPI_bus::SPI_bus(const SPI_bus& orig) {
}

SPI_bus::~SPI_bus() {
    //close the bus
    bcm2835_spi_end();
    bcm2835_close();
}

