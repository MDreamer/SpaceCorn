#include <iostream>
#include <pthread.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <json/json.h>

using namespace std;

#define MAX_NOTES	80
#define MAX_LEVEL	10
#define MAX_NUMBER	8


//the location of the note on the tree
//the location is Level and number based. The Levels are being counted from the buttom up - starting form "0"
//anf the nubmer is the number of the bottle/note clockwise according to the numbers on the Level. 
//There are 8 (0-7) Numbers/Notes in each Level
//
//			0
//			|	
//
//              7               1
//		\		/
//
//	6				2
// 	-                               -
//			
//              5               3
// 		/		\
//
// 			4
//                      |
//
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
		int setLocation(Note_Location _loc);

	private:
		//the name-value of the note		
		string note_name;
		//the file path of the sound file
		string note_path;
		//location of the note on the tree
		Note_Location note_loc;
};



Note::Note(string _note_name, string _note_path)
{
	note_name = _note_name;
	note_path = _note_path;
}

int Note::setLocation(const Note_Location _loc)
{
	// if ther desired location of the note is bigger/smaller than the
	// when something is wrong...
	if (_loc.Level < 0 || _loc.Level > MAX_LEVEL-1 || _loc.Number < 0 || _loc.Number > MAX_NUMBER-1)
		return 0;
	else
		note_loc = _loc;

	return 1;
}

void *PlayNote(Note note_to_play)
{
	
	return NULL;
}
