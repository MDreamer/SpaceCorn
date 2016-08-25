/* 
 * File:   CommLine.h
 * Author: mdreamer
 *
 * Created on August 10, 2016, 11:10 AM
 */

#include "spacecorn.cpp"

#ifndef COMMLINE_H
#define	COMMLINE_H



class CommLine {
public:
    CommLine();
    CommLine(const CommLine& orig);
    // c'tor for opening the comm with the right syspath, baudrate and mode
    CommLine(int _cport_nr, int _bdrate, char _mode[]);
    //d'tor - gets called after CloseCommLine
    virtual ~CommLine();
    // closes the comm and calling the d'tor
    void CloseCommLine(int _cport_nr);
private:

};

#endif	/* COMMLINE_H */

