/* 
 * File:   CommLine.cpp
 * Author: mdreamer
 * 
 * Created on August 10, 2016, 11:10 AM
 */

#include "CommLine.h"

CommLine::CommLine() {
}

CommLine::CommLine(const CommLine& orig) {
}
// c'tor for creating the serial comm with the right configuration
CommLine::CommLine(int _cport_nr, int _bdrate, char _mode[])
{
    // play indication sound
    system("omxplayer ../sounds/Wonderland_background.mp3");
    //COMM_TEST_PATH
    if(RS232_OpenComport(_cport_nr, _bdrate, _mode))
    {
        if (debug)
            std::cout << "Can not open comport..." << std::endl;
        return(0);
    }
    if (debug)
        cout << "open comport opened..."<<endl;
    // TODO: create a loop check for testing the comm lines
}
CommLine::~CommLine()
{}

void CommLine::CloseCommLine(int _cport_nr) 
{
    // TODO: create a sound for that
    RS232_CloseComport(_cport_nr)
    ~CommLine();
}


