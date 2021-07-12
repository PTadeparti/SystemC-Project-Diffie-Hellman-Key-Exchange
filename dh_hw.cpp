#include "systemc.h"
#include "digit.h"
#include "dh_hw.h"

#define WAIT_STATE 0
#define INPUT_STATE 1
#define EXECUTE_STATE_R 2
#define EXECUTE_STATE_B 3
#define OUTPUT_STATE 4
#define FINISH_STATE 5
int state = WAIT_STATE;

void dh_hw::process_hw()
{

  NN_DIGIT t[2], c;
  NN_HALF_DIGIT aHigh, cLow, cHigh;
 
  for (;;)
  {
    wait();
	  
    t[0] = RPout1.read();
    t[1] = RPout2.read();
    c = cin.read();
    aHigh = aHighin.read();
    cHigh = (NN_HALF_DIGIT)HIGH_HALF(c);
    cLow = (NN_HALF_DIGIT)LOW_HALF(c);


      switch(state)
      {
	  case WAIT_STATE:
              loadInput.write(0);
	      loadOutput.write(0);
	      hw_done.write(false);
	      hw_enable.read() == true ? state = INPUT_STATE : state = WAIT_STATE;
	      break;

	  case INPUT_STATE:
	      loadInput.write(1);
	      state = EXECUTE_STATE_R;
	      break;

	  case EXECUTE_STATE_R:
	      wait();
	      state = EXECUTE_STATE_B;
              break;

	  case EXECUTE_STATE_B:
	    
     	      /*** Begin: Bonus part (optional: Extra Datapath + Extra Control) ***/
              while ((t[1] > cHigh) || ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
		    if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow)) t[1]--;
         	    t[1] -= cHigh;
         	    aHigh++;
      	      }
     	      /*** End: Bonus part ***/

	      // Write outputs
	      R0outsig.write(t[0]);
      	      R1outsig.write(t[1]);
	      R2outsig.write(aHigh);
	     
	      state = OUTPUT_STATE;
	      break;

	  case OUTPUT_STATE:
	      loadOutput.write(1);
	      state = FINISH_STATE;
	      break;

	  case FINISH_STATE:
	      loadInput.write(0);
	      loadOutput.write(0);
	      hw_done.write(true);	
	      hw_enable.read() == 0 ? state = WAIT_STATE : state = FINISH_STATE;
              break;

      
	  }
	wait();
   }	  	  
}

