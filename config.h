/* 
 * File:   config.h
 * Author: mdreamer
 *
 * Created on August 25, 2016, 8:20 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

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

#define MAX_NOTES	80
#define MAX_LEVEL	10
#define MAX_RING	8
    
// Just for playing an example file for tryouts:
// hardcoded path - not fom the txt file
#define WAV_PATH "../sounds/39148__jobro__piano-ff-001.wav"
#define MUS_PATH "../sounds/Wonderland_background.ogg"

// A threshold value for the ADC reading in order deal with the SNR
#define piezoThreshold 20

#define PIN0 RPI_BPLUS_GPIO_J8_07 // pin#07 on SC pcb level 0 (GPIO 4)
#define PIN1 RPI_BPLUS_GPIO_J8_11 // pin#11 on SC pcb level 1 (GPIO ??)
#define PIN2 RPI_BPLUS_GPIO_J8_13 // pin#13 on SC pcb level 2 (GPIO ??)
#define PIN3 RPI_BPLUS_GPIO_J8_15 // pin#15 on SC pcb level 3 (GPIO ??)
#define PIN4 RPI_BPLUS_GPIO_J8_29 // pin#29 on SC pcb level 4 (GPIO ??)
#define PIN5 RPI_BPLUS_GPIO_J8_31 // pin#31 on SC pcb level 5 (GPIO ??)
#define PIN6 RPI_BPLUS_GPIO_J8_33 // pin#33 on SC pcb level 6 (GPIO ??)
#define PIN7 RPI_BPLUS_GPIO_J8_35 // pin#35 on SC pcb level 7 (GPIO ??)
#define PIN8 RPI_BPLUS_GPIO_J8_37 // pin#37 on SC pcb level 8 (GPIO ??)
#define PIN9 RPI_BPLUS_GPIO_J8_36 // pin#36 on SC pcb level 9 (GPIO ??)

//for debug mode
#define debug true

//for vocal alerts
#define vocal false

#define SC_START_PATH "../sounds/Boot/spacecorn_init.mp3"
#define LED_START_PATH "../sounds/Boot/spacecorn_light.mp3"
#define SOUNDS_START_PATH "../sounds/Boot/spacecorn_sound.mp3"
#define COMM_TEST_PATH "../sounds/Boot/spacecorn_driver_test.mp3"
#define COMM_ACK_PATH "../sounds/Boot/spacecorn_ack.mp3"
#define SYS_TEST_PATH "../sounds/Boot/spacecorn_system_test.mp3"
#define SC_SHUTDOWN_PATH "../sounds/Boot/spacecorn_shutdown_received.mp3"
#define LED_END_PATH "../sounds/Boot/spacecord_led_deactivate.mp3"
#define SOUND_END_PATH "../sounds/Boot/spacecord_sound_deactivate.mp3"
#define SC_END_PATH "../sounds/Boot/spacecorn_shutdown.mp3"


//byte 0 - start byte
//byte 1 - command - addressble LED/UV light
//byte 2 - data - No# of LED/ UV LED
//byte 3 - data (TBD) for future use if we'll have time? contol colours?
//byte 4 - data (TBD)
//byte 5 - data (TBD)
//byte 6 - running num seq
//byte 7 - end byte
//byte 8 - checksum
#define MSG_SIZE  9

using namespace std;

const int CSpin[MAX_LEVEL] = {
    PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN8, PIN9
};

// debounce value of the knocks
const int msThreshold = 100;

//signal to noise ratio/threshold for the knock sensors
const int snrThreshold = 3;

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

