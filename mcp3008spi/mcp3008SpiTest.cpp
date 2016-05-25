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
#include "mcp3008Spi.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_NOTES	8
 
using namespace std;
 
void *PlayNote(void *threadid)
{
   
   execlp("/usr/bin/omxplayer", " ", "39216__jobro__piano-ff-088.wav", NULL);
   pthread_exit(NULL);
}

int main(void)
{
	//thread vars
	pthread_t threads[MAX_NOTES];
	int rc;
	int i_thread;

	mcp3008Spi a2d("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
	int i = 20;
        int a2dVal = 0;
	int a2dChannel = 0;
        unsigned char data[3];
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
    while(1)
    {
        data[0] = 1;  //  first byte transmitted -> start bit
        data[1] = 0b10000000 |( ((a2dChannel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
        data[2] = 0; // third byte transmitted....don't care
 
        a2d.spiWriteRead(data, sizeof(data) );
 
        a2dVal = 0;
                a2dVal = (data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
                a2dVal |=  (data[2] & 0xff);
        usleep(100);
	if (a2dVal>50)
        	cout << "The Result is: " << a2dVal << endl;
        i--;
    }
	//pthread_exit(NULL);

    return 0;
}
