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
                bool tryPlaying(int hit);
                //void Note::setNoteSound();
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
                // average sample value that from it it will consider a hit
                // every cycle the sensor is being sampled and the reading it being averaged
                // if it is above the noise level times 2 (x2) it considered as a hit, if not
                // it is being dt in 0.1 to 0.9 ratio
                float noise;
                //the sound chunk to play/mix
                Mix_Chunk sound_data;
};


Note::Note(){}
Note::Note(string _note_name, string _note_path)
{
	note_name = _note_name;
	note_path = _note_path;
}
/*
void Note::setNoteSound()
{
    sound_data = Mix_LoadWAV(note_path);
    // Our wave file
    Mix_Chunk *temp_wave = NULL;
    if (temp_wave == NULL)
        {
            std::cout <<"failed load sound "<< note_path << endl;
            return;
        }
}
*/
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

bool Note::tryPlaying(int hit)
{
    // if the hit was 3 time stringer than the noise
    if (hit > noise*snrThreshold && hit > piezoThreshold)
    {
        //if (debug)
        //    cout << "hit = " << hit << " noise = " << noise << endl;
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int temp_ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
        
        //if (debug)
        //    cout << "temp_ms = " << temp_ms << " Note::getTimeStamp() = " << Note::getTimeStamp() << endl;
        // if it has been over 100ms since the last hit
        if (abs(temp_ms - Note::getTimeStamp()) > 100)
        {
            if (debug)
                cout << "HIT (in tryPlaying)" << endl;
            setTimeStamp(); //call time for base line compare
            noise = (noise * 0.9) + (hit * 0.1);
            return true;
            
        }
        else //it played in the last 100ms - do nothing - so avg won't be affected
        {
            return false;
        }
    }
    else    //this his wasn't strong enough, calc avg
    {
        noise = (noise * 0.9) + (hit * 0.1);
        return false;
    }
    
}
void *PlayNote(Note note_to_play)
{
	//TODO: before each play - check if the note has been played during 
        //the last  debounce_threshold ms
	return NULL;
}
