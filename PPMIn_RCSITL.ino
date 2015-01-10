/* ---------------------------------------------------------------------------

** PPMIn_RCSITL.pde
** Decode PPM signal on pin 8 and send over Serial at 115200 baudrate
**
** Author: Daniel van den Ouden
**          Modified by Daniel Nugent to interface with SITL
** -------------------------------------------------------------------------*/

#include <PPMIn.h>
#include <Timer1.h>


#define CHANNELS 8

uint16_t g_values[CHANNELS];                  // output buffer for PPMIn
uint8_t  g_workIn[PPMIN_WORK_SIZE(CHANNELS)]; // we need to have a work buffer for the PPMIn class

rc::PPMIn g_PPMIn(g_values, g_workIn, CHANNELS);

void setup()
{
  
        Serial.begin(115200);
	// Initialize timer1, this is required for all features that use Timer1
	// (PPMIn/PPMOut/ServoIn/ServoOut)
	rc::Timer1::init();
	
	// We use pin 8 as PPM input pin
	pinMode(8, INPUT);
	
	// We use pin change interrupts to detect changes in the signal
	// If you're unfamiliar with how this works, please look up some
	// article or tutorial on the subject.
	
	// only allow pin change interrupts for PB0 (digital pin 8)
	PCMSK0 = (1 << PCINT0);
	
	// enable pin change interrupt 0
	PCICR = (1 << PCIE0);
	
	// set a timeout (default 500 milliseconds)
	g_PPMIn.setTimeout(1000);
	
	// start listening
	g_PPMIn.start();
}


void loop()
{
	// update incoming values
	g_PPMIn.update();
	
        //send valid signals
	if (g_PPMIn.isStable())
	{
		for(int i = 0; i < CHANNELS; i++){
                    Serial.print(g_values[i]);
                    if(i == CHANNELS - 1){
                      Serial.println();
                    }
                    else{
                      Serial.print(", ");
                    }
                }
	}
        // signal has been lost (no new valid frames for 'timeout' milliseconds)
	else if (g_PPMIn.isLost())
	{
                for(int i = 0; i < CHANNELS; i++){
                    Serial.print(0);
                    if(i == CHANNELS - 1){
                      Serial.println();
                    }
                    else{
                      Serial.print(", ");
                    }
                }
	}
}


// Interrupt handling code below, this needs cleaning

static uint8_t lastB = 0; // last read value of PINB

// Pin change port 0 interrupt
ISR(PCINT0_vect)
{
	uint8_t newB = PINB;
	
	// we've hardcoded the bitmask here (1 << 0), the portable way to get it is
	// digitalPinToBitMask(pinnr), but you really don't want to call that in
	// the interrupt handler.
	// we test if the specified pin has changed (not really necessary, but
	// think about what would happen if you were monitoring multiple pins)
	if ((lastB & (1 << 0)) != (newB & (1 << 0)))
	{
		// and we tell PPMIn the pin has changed
		g_PPMIn.pinChanged(newB & (1 << 0));
	}
	
	lastB = newB;
}
