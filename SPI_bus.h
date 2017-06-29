/* 
 * File:   SPI_bus.h
 * Author: mdreamer
 *
 * Created on May 20, 2017, 8:51 PM
 */

#ifndef SPI_BUS_H
#define	SPI_BUS_H
#include "config.h"
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

class SPI_bus {
public:
    SPI_bus();
    SPI_bus(const SPI_bus& orig);
    virtual ~SPI_bus();
    int startSPI();
    int get_piezo_reading(int _slave, int _channel);
private:
    void wrap_datagram(char _datagram[3], int _channel);
    int unwrap_datagram(char _datagram[3]);
};

#endif	/* SPI_BUS_H */

