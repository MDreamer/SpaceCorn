#include "SDL.h"
#include "SDL_mixer.h"
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
#include <iostream>
#include <vector>

// Just for playing an example file for tryouts:
// hardcoded path - not fom the txt file
#define WAV_PATH "../sounds/39148__jobro__piano-ff-001.wav"
//#define MUS_PATH "../sounds/39148__jobro__piano-ff-001.wav"

// A threshold value for the ADC reading in order deal with the SNR
#define piezoThreshold 50

#define PIN0 RPI_BPLUS_GPIO_J8_03 // pin#2 on my pcb level 0 (I2C1 SDA)
#define PIN1 RPI_BPLUS_GPIO_J8_05 // pin#5 on my pcb level 1 (I2C1 SCL)
#define PIN2 RPI_BPLUS_GPIO_J8_07 // pin#7 on my pcb level 2 (GPIO 4)
#define PIN3 RPI_BPLUS_GPIO_J8_11 // pin#11 on my pcb level 3 (GPIO 17)
#define PIN4 RPI_BPLUS_GPIO_J8_13 // pin#13 on my pcb level 4 (GPIO 27)
#define PIN5 RPI_BPLUS_GPIO_J8_15 // pin#15 on my pcb level 5 (GPIO 22)
#define PIN6 RPI_BPLUS_GPIO_J8_29 // pin#29 on my pcb level 6 (GPIO 5)
#define PIN7 RPI_BPLUS_GPIO_J8_31 // pin#31 on my pcb level 7 (GPIO 6)
#define PIN8 RPI_BPLUS_GPIO_J8_33 // pin#33 on my pcb level 8 (GPIO 13)
#define PIN9 RPI_BPLUS_GPIO_J8_35 // pin#35 on my pcb level 9 (GPIO 19)

using namespace std;

const int CSpin[MAX_LEVEL] = {
    PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN8, PIN9
};
 
Mix_Chunk wavs[MAX_NOTES];
// Our wave file
Mix_Chunk *wave = NULL;
// Our music file
//Mix_Music *music = NULL;

//Function that plays the wav on its own thread - to be used as the sound player
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

//loads and config the SDL sound module - as of now without background music
int loadSDL()
{
    
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

    // Load our music - temporary disables - maybe will be used later
    // just keeping the code commented for later use..
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
    
    // Load our music - temporary disables - maybe will be used later
    // just keeping the code commented for later use..
    //if ( Mix_PlayMusic( music, -1) == -1 )
    //        return -1;

    //while ( Mix_PlayingMusic(-1) ) ;
}

/* starts the SPI interface module, also configures the GPIO pins that will allow
 * control over the ChipEnable pins
*/
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
    /*
    bcm2835_gpio_fsel(PIN0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PIN1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PIN2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(PIN0, HIGH);
    bcm2835_gpio_write(PIN1, HIGH);
    bcm2835_gpio_write(PIN2, HIGH);
    */
    // start the SPI
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); //Data comes in on falling edge
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); //250MHz / 256
    //No CS/CE, Control the SPI slave manually thru the GPIO pins
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); 
}

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
    cout << "reading notes.txt"  << endl;

    Note notes_list[MAX_NOTES];
    int iNotes=0;
    infile.open ("notes.txt");
    Note_Location temp_location;
    while (!infile.eof())
    {
        getline(infile,STRING); // Saves the line in STRING.
        
        istringstream iss(STRING);
        vector<string> tokens;
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
        notes_list[iNotes].setNotePath("../sounds/" + tokens[0]);

        
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
        //sets the new wav sound file the the notes sound array
        wavs[iNotes] = *wave;
        iNotes++;
        // if case that the file contains more note that bottles
        if (iNotes >= MAX_NOTES)
        {
            std::cout << "too many notes in file... exiting" << endl;
            return -1;
        }
    }
    infile.close();
}


int main(void)
{
    srand (time(NULL));
    
    char send_data[3] = {0x01,0x80,0x01};

    if (loadSDL() == -1)
        return -1;
    
    if (loadNotesFile() == -1)
        return -1;

    if (startSPI() == -1)
        return -1;
    system("killall omxplayer.bin"); //making sure no other sound is runnning
    std::cout << "Initializing SpaceCorn..." << endl;

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
    int to=0;
    /* The cyclic logic sampling:
     * Cycle through all the bottles/piezo sensors and sample them. 
     * Stores the ADC in an array to be decided if it should be played - threshold 
     * dependent(can be configured/changed). 
     * 
     * 
     */
    while (1)
    {
        //cyclic reading 0 to 9 = total 10 SPI devices/slaves
        for (iSPIdev = 0; iSPIdev < MAX_LEVEL; iSPIdev++)
        {
            //cyclic reading 0 to 7 = total 8 channels
            for (a2dChannel = 0; a2dChannel < MAX_NUMBER; a2dChannel++)
            {
                //prepare the datagram for sending
                send_data[0] = 1;  //  first byte transmitted -> start bit
                send_data[1] = 0b10000000 |( ((a2dChannel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
                send_data[2] = 1; // third byte transmitted....don't care
                
                //send the data + HIGH-LOW the relevant pin
                bcm2835_gpio_write(CSpin[iSPIdev], LOW);
                bcm2835_spi_transfern(send_data,3);
                bcm2835_gpio_write(CSpin[iSPIdev], HIGH);
                //usleep(100);
                //data[0] first byte of the response - don't care
                a2dVal = 0;
                a2dVal = (send_data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
                a2dVal |=  (send_data[2] & 0xff);
                //cout << (int)send_data[0] << "  " << (int)send_data[1] << "  " << (int)send_data[2] <<endl;
                //usleep(100);
                if (a2dVal>piezoThreshold)
                {
                    // add to queue to play]
                    usleep(10000);
                    cout << "Result: " << a2dVal << " SPI: " << iSPIdev << " Channel: " << a2dChannel <<endl;
                    i_thread ++;
                    i_thread = i_thread % (MAX_NOTES-1);
                    to++;

                    cout << "starting to play sound thread" << endl;
                     
                    rc = pthread_create(&threads[i_thread], NULL, PlayNote, (void *)i_thread);
                    if (rc)
                    {
                            cout << "Error:unable to create thread," << rc << endl;
                            exit(-1);
                    }
                    

                    
                    //"debounce" of 100ms   
                    //usleep(100000);
                }
            }
            //usleep(100000);
        }
    }


    std::cout << "Exiting SpaceCorn..." << endl;;

    // clean up our resources
    Mix_FreeChunk(wave);
    //Mix_FreeMusic(music);

    // quit SDL_mixer
    Mix_CloseAudio();

    bcm2835_spi_end();
    bcm2835_close();
    pthread_exit(NULL);

    return 0;
}
