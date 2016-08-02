/* 
 * File:   readkeysdata.h
 * Author: mdreamer
 *
 * Created on May 27, 2016, 10:49 PM
 */
#include <stdlib.h>
#include <string>

#ifndef READKEYSDATA_H
#define	READKEYSDATA_H

class KeysFile{
    KeysFile(const string &_filename);
    Note readFile(const &string _filename);
    
private:
    string filename;
};

#endif	/* READKEYSDATA_H */

