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
 
  for (;;)
  {
	  wait(); 
      switch(state)
      {
		  case WAIT_STATE:
	          loadInput.write(0);
		      loadOutput.write(0);
		      readyBP.write(0);
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
		      readyBP.write(1);    	      
		      state = OUTPUT_STATE;
		      break;
	
		  case OUTPUT_STATE:
		      loadOutput.write(1);
		      state = FINISH_STATE;
		      break;
	
		  case FINISH_STATE:
		      loadInput.write(0);
		      loadOutput.write(0);
		      readyBP.write(0);
		      hw_done.write(true);	
		      hw_enable.read() == 0 ? state = WAIT_STATE : state = FINISH_STATE;
	          break;

      
	  }
	  
	wait();
   }	  	  
}

