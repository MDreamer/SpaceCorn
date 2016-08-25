#include <iostream>
#include <pthread.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include "config.h"

using namespace std;


struct Note_Location
{
	int Level;
	int Number;
};

class Note
{
	public:
		//c'tor
		Note(string _note_name, string _note_path);
                Note();
		int setLocation(Note_Location _loc);
                string getNotePath();
                void setNotePath(string _note_path);
                void setTimeStamp();
                long int getTimeStamp();
                bool canPlay();
	private:
		//the name-value of the note		
		string note_name;
		//the file path of the sound file
		string note_path;
		//location of the note on the tree
		Note_Location note_loc;
                // sound debouncing:
                // state the last time it was played/sampled after playing 
                // in ms when it is = 0 it means that its ready to be played
                // if its between 0 to 100ms it cannot be played and will
                // wait till the next check to be set. When it will go over 100ms
                // it will be set to 0 to indicate it is ready to be played
                long int lastPlayed_ms = 0;
};


Note::Note(){}
Note::Note(string _note_name, string _note_path)
{
	note_name = _note_name;
	note_path = _note_path;
}

void Note::setNotePath(const string _note_path)
{
    note_path = _note_path;
}

string Note::getNotePath()
{
    return note_path;
}
void Note::setTimeStamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    lastPlayed_ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
}
long int Note::getTimeStamp()
{
    return lastPlayed_ms;
}
int Note::setLocation(const Note_Location _loc)
{
	// if ther desired location of the note is bigger/smaller than the
	// when something is wrong...
	if (_loc.Level < 0 || _loc.Level > MAX_LEVEL-1 || _loc.Number < 0 || _loc.Number > MAX_RING-1)
		return 0;
	else
		note_loc = _loc;

	return 1;
}
bool Note::canPlay()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int temp_ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
    // if it has been over 100ms sence the last hit
    if (abs(Note::getTimeStamp() - temp_ms) > 100)
        return true;
    else
        return false;
}
void *PlayNote(Note note_to_play)
{
	//TODO: before each play - check if the note has been played during 
        //the last  debounce_threshold ms
	return NULL;
}
