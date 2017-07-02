#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <bcm2835.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "notes.cpp"
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "RS-232/rs232.h"
#include "config.h"
#include "SPI_bus.h"

Note notes_list[MAX_LEVEL][MAX_RING];
 
Mix_Chunk wavs[MAX_NOTES];
// Our wave file
Mix_Chunk *wave = NULL;
// Our music file
Mix_Music *music = NULL;

SPI_bus spi_sensors;

int
  cport_nr=22,        /* /dev/ttyS0 (COM1 on windows) */
  bdrate=9600;       /* 9600 baud */

unsigned char buf[4096];

char mode[]={'8','N','1',0};
  
// check sum data check up 
void calc_checksum(char payload_check[])
{
  for (int i = 0; i < (MSG_SIZE - 1); i++) // don't calc the checksum itself
  {
    payload_check[MSG_SIZE - 1] += payload_check[i];
  }   
}

//Write the data to Serial
void *DataToSerial(void *threadrag)
{
    //int send_d = (int*)threadrag;    
    int send_d = (int)threadrag;
    if (debug)
        cout << "data thread started sending" << send_d <<endl;
    
    char buffer [50];
    int n=send_d;
    sprintf (buffer, "%d", n);
    printf ("[%s] is a to uart\n",buffer);
    
    //std::sprintf(str, "%d", *send_d);
    //std::cout << str;
    
    RS232_cputs(cport_nr, buffer);
    printf("sent: %s\n", buffer);
    //usleep(1000000);
    if (debug)
        cout << "data thread ended" << endl;
    //pthread_exit(NULL);
}
//Function that plays the wav on its own thread - to be used as the sound player
void *PlayNote(void *play_note_idx)
{
   long wav_num;
   wav_num = (long)play_note_idx;
   if (wav_num < MAX_NOTES)
   {
        if (debug)
            cout << "playing sound number "<< wav_num << "in thread" << endl;
        int ret_ch = Mix_PlayChannel(-1, &wavs[wav_num], 0);
        if ( ret_ch == -1 )
         {
            if (debug)
                std::cout <<"failed Mix_PlayChannel"<<endl;
         }
        //let it finish playing
        while ( Mix_Playing(ret_ch) ) ;
   }
   else 
       if (debug)
           cout << "From thread note player: the note number exceed the MAX_NOTES" << endl;
    // free the resource?
    //Mix_FreeChunk(wave);
   pthread_exit(NULL);
}

//loads and config the SDL sound module - as of now without background music
int loadSDL()
{
    if (debug)
        std::cout <<"Loading SDL..."<<endl;
    
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        if (debug)
            std::cout <<"failed init_sdl"<<endl;
        return -1;
    }
    
    if(Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        if (debug)
            std::cout << "cannot Mix_init: "<< Mix_GetError() << endl;
        //return -1; 
    }

    //Initialize SDL_mixer 
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0 ) 
    {
        if (debug)
        {
            std::cout <<"failed SDL_mixer"<<endl;
            printf("Mix_OpenAudio: %s\n", Mix_GetError());
        }
        return -1; 
            
    }
    Mix_Volume(1,MIX_MAX_VOLUME);
    //while(1);
    Mix_AllocateChannels(24);

    // Load our sound effect
    wave = Mix_LoadWAV(WAV_PATH);
    if (wave == NULL)
    {
        if (debug)
        {
            std::cout <<"failed load sound on loadSDL, path: "<< WAV_PATH <<endl;
            printf("Mix_OpenAudio: %s\n", Mix_GetError());
        }
        return -1;
    }

    // Load our music
  
    music = Mix_LoadMUS(MUS_PATH);
    if (music == NULL)
    {
        std::cout <<"failed load music (MUS_PATH)"<<endl;
        return -1;
    }
    Mix_VolumeMusic((MIX_MAX_VOLUME/8));
    if ( Mix_PlayChannel(-1, wave, 0) == -1 )
    {
        if (debug)
            std::cout <<"failed Mix_PlayChannel"<<endl;
        return -1;
    }

    //let it finish playing
    while ( Mix_Playing(-1) ) ;  
    
    // Load our music
    if ( Mix_PlayMusic( music, -1) == -1 )
            return -1;

    //while ( Mix_PlayingMusic() ) ;
}

/* starts the SPI interface module, also configures the GPIO pins that will allow
 * control over the ChipEnable pins

int startSPI()
{
    
    if (!bcm2835_init())
    {
        return -1;
    }
    
    // Set the all the pins to be an output and hold it high for SPI use
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
}
*/

/*
 * /oading the index file that contains all the names of the sound files 
    and location under the following structure: [filename] [level] [location]
    i.e. 39148__jobro__piano-ff-001.wav 3 2
 */
int loadNotesFile()
{
    string STRING; //temp var to read the lines
    ifstream infile; //file handler

    //prompt to user
    if (debug)
        cout << "reading notes.txt"  << endl;

    
    int iNotes=0;
    
    int iLevelCounter = 0;
    int iRingCounter = 0;
    infile.open ("notes.txt");
    Note_Location temp_location;
    while (!infile.eof())
    {
        getline(infile,STRING); // Saves the line in STRING.
        
        istringstream iss(STRING);
        vector<string> tokens;
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
        notes_list[iLevelCounter][iRingCounter].setNotePath("../sounds/" + tokens[0]);

        
        istringstream convert(tokens[1]);
        if ( !(convert >> temp_location.Level) ) //give the value to 'Result' using the characters in the stream
            temp_location.Level = -1;             //if that fails set 'Result' to 0

        istringstream convert2(tokens[2]);
        if ( !(convert2 >> temp_location.Number) ) //give the value to 'Result' using the characters in the stream
            temp_location.Number = -1;             //if that fails set 'Result' to 0

        notes_list[iLevelCounter][iRingCounter].setLocation(temp_location);
        if (debug)
            cout << notes_list[iLevelCounter][iRingCounter].getNotePath() << " " << temp_location.Level << " " << temp_location.Number << endl;

        // Load our sound effect
        wave = Mix_LoadWAV(notes_list[iLevelCounter][iRingCounter].getNotePath().c_str());
        //after the path had been set load the actual file to memory
        //notes_list[iLevelCounter][iRingCounter].setNoteSound();
                
        if (wave == NULL)
        {
            if (debug)
                std::cout <<"failed load sound loadNotesFile"<< notes_list[iLevelCounter][iRingCounter].getNotePath() << endl;
            return -1;
        }
        //sets the new wav sound file the the notes sound array
        wavs[iNotes] = *wave;
        iNotes++;
        iLevelCounter++;
        iLevelCounter = iLevelCounter % MAX_LEVEL;
        iRingCounter++;
        iRingCounter = iRingCounter % MAX_RING;
        // if case that the file contains more note that bottles
        if (iNotes > MAX_NOTES)
        {
            if (debug)
                std::cout << "too many notes in file... exiting" << endl;
            return -1;
        }
    }
    infile.close();
}


int main(void)
{
    srand (time(NULL));
    if (debug)
        std::cout << "Initializing SpaceCorn..." << endl;
    //sound "test" not using SDL
    
    //system("killall aplay"); //making sure no other sound is running

    //system("omxplayer " + INIT_SC);
    
    //wait 3 sec to finish play the mp3 file
    //usleep(5000000);
    
    //try to start the bus - if failed kill the program
    if (spi_sensors.startSPI() == -1)
        return -1;
    
    // try to load sound
    if (loadSDL() == -1)
        return -1;
    
    // load files
    if (loadNotesFile() == -1)
        return -1;
    
    // check communication line
    if(RS232_OpenComport(cport_nr, bdrate, mode))
    {
        if (debug)
            cout << "Can not open comport..." << endl;
        return(0);
    }
    if (debug)
        cout << "open comport opened..."<<endl;
    
    //thread vars
    pthread_t threads[MAX_NOTES];
    pthread_t thread1;
    
    int rc;
    int i_thread=0;

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
    
    int i = 20;
    int a2dVal = 0;         // the real outcome result of the return value after summing all the 3 bytes
    int a2dChannel = 0;     //the channel that is sampled
    
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
    int iSPIdev=0; //from SPI0 to 9 = 10 devices
    
    /* The cyclic logic sampling:
     * Cycle through all the bottles/piezo sensors and sample them. 
     * Stores the ADC in an array to be decided if it should be played - threshold 
     * dependent(can be configured/changed). 
     */
    char seqCounter=0; //just a running number to keep the checksum changing
                     //even if all the other data byte are remaining the same
                     //this char var will overflow-cycle from 255 -> 0 if incremented
    int tempi;

    char buffer [MSG_SIZE]; //payload
    //resent the LEDs of the corn using 80d (0x50h) command
    buffer[0] = 115;  // Start byte = 's'
    buffer[1] = 80;
    buffer[2] = 80;
    buffer[3] = 0;    // seq
    buffer[4] = 19;    // seq

    // calc check sum
    //for (int k = 0 ; k < MSG_SIZE-1 ; k++)
    //    buffer[MSG_SIZE-1] += buffer[k];
    
    RS232_cputs(cport_nr, buffer);

    while (1)
    {
        //cyclic reading 0 to 9 = total 10 SPI devices/slaves
        for (iSPIdev = 0; iSPIdev < MAX_LEVEL; iSPIdev++)
        {
            //cyclic reading 0 to 7 = total 8 channels
            for (a2dChannel = 0; a2dChannel < MAX_RING; a2dChannel++)
            {
                // get reading from the SPI bus / piezo sensors array
                a2dVal = spi_sensors.get_piezo_reading(iSPIdev,a2dChannel);
                
                if (notes_list[iSPIdev][a2dChannel].tryPlaying(a2dVal))
                {
                    
                    // add to queue to play]
                    //usleep(10000);
                    if (debug)
                        cout << "Result: " << a2dVal << " SPI: " << iSPIdev << " Channel: " << a2dChannel <<endl;
                    
                    char buffer [MSG_SIZE]; //payload
                    //byte 0 - start byte
                    //byte 1 - command - addressble LED = 50 / UV light = 60
                    //byte 2 - data - No# of LED/ UV LED
                    //byte 3 - data (TBD) for future use if we'll have time? control colours?
                    //byte 4 - data (TBD)
                    //byte 5 - data (TBD)
                    //byte 6 - running num seq
                    //byte 7 - end byte
                    //byte 8 - checksum
                    
                    // The sensor's number being triggered. Each level is contains 
                    // 8 sensors per SPI device
                    int senNum = (iSPIdev*10) + a2dChannel; 
                    
                    buffer[0] = 115;  // Start byte = 's'
                    buffer[1] = 50;
                    buffer[2] = (char)senNum;
                    buffer[3] = seqCounter;
                    buffer[4] = 0;    // Payload checksum
                    
                    
                    // calc check sum
                    for (int k = 0 ; k < MSG_SIZE-1 ; k++)
                        buffer[MSG_SIZE-1] += buffer[k];
                    
                    if (debug)
                        cout << "sent: " << (int)buffer[0] << ", " << (int)buffer[1] << ", " 
                             << (int)buffer[2] << ", " << (int)buffer[3] << "," << (int)buffer[4] <<endl;
                    //sprintf (buffer, "%d", n);
                    //printf ("[%s] is a to uart\n",buffer);
                    
                    ///send through the UART
                    RS232_cputs(cport_nr, buffer);
                    seqCounter++; //increase sequence counter by 1 every transmission
                    //printf("sent: %s\n", buffer);
                    
                    i_thread ++;
                    i_thread = i_thread % (MAX_NOTES-1);
                    
                    if (debug)
                        cout << "starting to play sound thread" << endl;
                    int note_num_play = iSPIdev * 8 + a2dChannel;
                            
                    rc = pthread_create(&threads[i_thread], NULL, PlayNote, (void *)note_num_play);
                    pthread_detach(threads[i_thread]);
                    if (rc)
                    {
                        if (debug)
                            cout << "Error:unable to create thread," << rc << endl;
                        exit(-1);   
                    }
                    
                }
            }
            // sleep between readings. I'm not sure this is necessary... 
            usleep(100);
        }
    }

    if (debug)
        std::cout << "Exiting SpaceCorn..." << endl;;

    // clean up our resources
    Mix_FreeChunk(wave);
    //Mix_FreeMusic(music);

    // quit SDL_mixer
    Mix_CloseAudio();
    /*
    bcm2835_spi_end();
    bcm2835_close();
     */
    pthread_exit(NULL);
    
    return 0;
}
