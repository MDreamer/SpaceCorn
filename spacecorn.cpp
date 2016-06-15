/***********************************************************************
 * mcp3008SpiTest.cpp. Sample program that tests the mcp3008Spi class.
 * an mcp3008Spi class object (a2d) is created. the a2d object is instantiated
 * using the overloaded constructor. which opens the spidev0.0 device with
 * SPI_MODE_0 (MODE 0) (defined in linux/spi/spidev.h), speed = 1MHz &
 * bitsPerWord=8.
 *
 * call the spiWriteRead function on the a2d object 20 times. Each time make sure
 * that conversion is configured for single ended conversion on CH0
 * i.e. transmit ->  byte1 = 0b00000001 (start bit)
 *                   byte2 = 0b1000000  (SGL/DIF = 1, D2=D1=D0=0)
 *                   byte3 = 0b00000000  (Don't care)
 *      receive  ->  byte1 = junk
 *                   byte2 = junk + b8 + b9
 *                   byte3 = b7 - b0
 *    
 * after conversion must merge data[1] and data[2] to get final result
 *
 *
 *
 * *********************************************************************/
#include "SDL.h"
#include "SDL_mixer.h"
#include "mcp3008spi/mcp3008Spi.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
//#include "ws2812-rpi/ws2812-rpi.h"
#include "notes.cpp"
//#include "json/json.h"
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

#define WAV_PATH "../sounds/39148__jobro__piano-ff-001.wav"
//#define MUS_PATH "../sounds/39148__jobro__piano-ff-001.wav"
 
using namespace std;
 
Mix_Chunk wavs[MAX_NOTES];
// Our wave file
Mix_Chunk *wave = NULL;
// Our music file
Mix_Music *music = NULL;

void *PlayNote(void *threadid)
{
   
   cout << "playing sound in thread" << endl;
   int ret_ch = Mix_PlayChannel(-1, &wavs[(rand() % 68)], 0);
   if ( ret_ch == -1 )
    {
        std::cout <<"failed Mix_PlayChannel"<<endl;
    }
   //let it finish playing
   while ( Mix_Playing(ret_ch) ) ;
   pthread_exit(NULL);
}

int main(void)
{
    srand (time(NULL));
    // quit SDL_mixer
        //Mix_CloseAudio();


        // Initialize SDL.
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
                std::cout <<"failed init_sdl"<<endl;
                return -1;
        }


        //Initialize SDL_mixer 
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) 
        {
            std::cout <<"failed SDL_mixer"<<endl;
            printf("Mix_OpenAudio: %s\n", Mix_GetError());
                return -1; 
        }
        Mix_AllocateChannels(24);
        
        // Load our sound effect
        wave = Mix_LoadWAV(WAV_PATH);
        if (wave == NULL)
        {
            std::cout <<"failed load sound"<<endl;
            return -1;
        }

        // Load our music
        //music = Mix_LoadMUS(MUS_PATH);
        //if (music == NULL)
        //        return -1;
        
        if ( Mix_PlayChannel(-1, wave, 0) == -1 )
        {
            std::cout <<"failed Mix_PlayChannel"<<endl;
            return -1;
        }

        //let it finish playing
        while ( Mix_Playing(-1) ) ;  
        
        //if ( Mix_PlayMusic( music, -1) == -1 )
        //        return -1;

        //while ( Mix_PlayingMusic(-1) ) ;

        // clean up our resources
        //Mix_FreeChunk(wave);
        //Mix_FreeMusic(music);

        // quit SDL_mixer
        //Mix_CloseAudio();


    //system("killall omxplayer.bin");
    std::cout << "Initializing SpaceCorn..." << endl;
    
    string STRING;
    ifstream infile;
  
    // loading the index file that contains all the names of the sound files 
    // and location under the following structure: "filename, level, location"
    // i.e. 39148__jobro__piano-ff-001.wav 3 2
    
    cout << "reading notes.txt"  << endl;
    
    Note notes_list[MAX_NOTES];
    int iNotes=0;
    infile.open ("notes.txt");
    Note_Location temp_location;
    while (!infile.eof())
    {
        
        getline(infile,STRING); // Saves the line in STRING.
        //cout<<STRING; // Prints our STRING.
        istringstream iss(STRING);
        vector<string> tokens;
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
        notes_list[iNotes].setNotePath("../sounds/" + tokens[0]);
        
        //temp_location.Level = (tokens[1]);
        //temp_location.Number = (tokens[2]);
        istringstream convert(tokens[1]);
        if ( !(convert >> temp_location.Level) ) //give the value to 'Result' using the characters in the stream
            temp_location.Level = -1;             //if that fails set 'Result' to 0
        
        istringstream convert2(tokens[2]);
        if ( !(convert2 >> temp_location.Number) ) //give the value to 'Result' using the characters in the stream
            temp_location.Number = -1;             //if that fails set 'Result' to 0
        
        notes_list[iNotes].setLocation(temp_location);
        cout << notes_list[iNotes].getNotePath() << " " << temp_location.Level << " " << temp_location.Number << endl;
        
        // Load our sound effect
        wave = Mix_LoadWAV(notes_list[iNotes].getNotePath().c_str());
        if (wave == NULL)
        {
            std::cout <<"failed load sound "<< notes_list[iNotes].getNotePath() << endl;
            return -1;
        }
        wavs[iNotes] = *wave;
        iNotes++;
        if (iNotes >= MAX_NOTES)
        {
            std::cout << "too many notes in file... exiting" << endl;
            return -1;
        }
    }
    infile.close();
    
    
    
    //NeoPixel *n=new NeoPixel(24);

    //while(true) n->effectsDemo();
    //  delete n;
    
    //thread vars
    pthread_t threads[MAX_NOTES];
    pthread_t thread1;
    int rc;
    int i_thread=0;
    
    mcp3008Spi a2d("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
    int i = 20;
    int a2dVal = 0;         // the real outcome result of the return value after summing all the 3 bytes
    int a2dChannel = 0;     //the channel that is sampled
    unsigned char data[3];  //data buffer - used for send&recv
 /*
    for( i_thread=0; i_thread < MAX_NOTES; i_thread++ )
    {
            sleep(1);
            cout << "From Main thread creating thread, " << i_thread << endl;
            rc = pthread_create(&threads[i_thread], NULL, 
                      PlayNote, (void *)i_thread);
            if (rc)
            {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
            }
    }
*/		
    //rc = pthread_create(&thread1, NULL, PlayNote, (void *)i_thread);
    //i_thread++;
    //pthread_exit(NULL);
    int to=0;
    while(to < 6)
    {
        a2dChannel  = a2dChannel % 7;    //cyclic reading 0 to 7 = total 8 channels
                                        //this the place of the sensor in the level
                
        data[0] = 1;  //  first byte transmitted -> start bit
        data[1] = 0b10000000 |( ((a2dChannel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
        data[2] = 0; // third byte transmitted....don't care
 
        a2d.spiWriteRead(data, sizeof(data) );
 
        //data[0] first byte of the response - don't care
        a2dVal = 0;
        a2dVal = (data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
        a2dVal |=  (data[2] & 0xff);
        usleep(100);
	if (a2dVal>50)
        {
            cout << "starting to play sound thread" << endl;
             
            rc = pthread_create(&threads[i_thread], NULL, PlayNote, (void *)i_thread);
            if (rc)
            {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
            }
            // add to queue to play]
            cout << "The Result is: " << a2dVal << endl;
            i_thread ++;
            i_thread = i_thread % (MAX_NOTES-1);
            //to++;
            //"debounce" of 100ms
            usleep(100000);
        }
    }
 
    
    std::cout << "Exiting SpaceCorn..." << endl;;
    
    // clean up our resources
    Mix_FreeChunk(wave);
    //Mix_FreeMusic(music);

    // quit SDL_mixer
    Mix_CloseAudio();
    
    pthread_exit(NULL);
    
    
    return 0;
}
