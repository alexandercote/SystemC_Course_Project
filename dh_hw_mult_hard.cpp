
#include "systemc.h"
#include "digit.h"
#include "dh_hw_mult.h"
#include <iostream>


void dh_hw_mult::process_hw_mult()
	{		
	    state = WAIT_STATE;
	    hw_mult_done.write(0);
	    int counter = 0;
	    
		while(true){
		  
			switch(state)
			{
				case WAIT_STATE:
					if(hw_mult_enable.read() == true)
					{
						b_register_load.write(true);
						c_register_load.write(true);
						
						// Re-init the control signals to all low.
						a0_register_load.write(false);
						a1_register_load.write(false);
						
						// clear output signals
						out_data_low.write(0);
						out_data_high.write(0);
						
						state = ES_STAGE_1;
					}
					break;
					
				case ES_STAGE_1:
					// Ignore the input
					b_register_load.write(false);
					c_register_load.write(false);
					
					// Load in the new multiplication and adder data.
					a0_register_load.write(true);
					a1_register_load.write(true);
					
					state = ES_STAGE_2;
					break;
				
				case ES_STAGE_2:
					// Deassert Reg Values
					a0_register_load.write(false);
					a1_register_load.write(false);
					
					state = OUTPUT_STATE;
					break;
				
				case OUTPUT_STATE:
					// Write outputs
					out_data_low.write(a0_register_out.read());
					out_data_high.write(a1_out.read());
					//wait();
					hw_mult_done.write(true); // assert multiplication is done
					state = FINISH_STATE;
					break;
					
				case FINISH_STATE:
					if(hw_mult_enable.read() == false)
					{
						hw_mult_done.write(false);
						state = WAIT_STATE;
						
					}
					
					counter++;
					if((counter % 1000) == 0)
					{
					    cout << "Counter = " << counter << endl;
					}
					break;
			
			} // end switch
			
			wait();
			
		}// end while true	  
	}
