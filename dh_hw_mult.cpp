#include "systemc.h"
#include "digit.h"
#include "dh_hw_mult.h"


void dh_hw_mult::process_hw_mult()
	{
	    cout << "process_hw_mult: Entered" << endl;
	    NN_DIGIT a[2], b, c, t, u;
	    NN_HALF_DIGIT bHigh, bLow, cHigh, cLow;
		
	    state = WAIT_STATE;
	    hw_mult_done.write(0);
	    
		while(true){
			switch(state)
			{
				case WAIT_STATE:
					//cout << "process_hw_mult: WAIT_STATE" << endl;
					if(hw_mult_enable.read() == true)
					{
						state = ES_MULT_LOAD;
						b_register_load.write(true);
						c_register_load.write(true);
					}
					//wait(); // Figure out where to put this....
					break;
					
				case ES_MULT_LOAD:
					// Ignore the input
					b_register_load.write(false);
					c_register_load.write(false);
					
					// Load in the new multiplication data.
					t_register_load.write(true);
					u_register_load.write(true);
					a0_register_load.write(true);
					a1_register_load.write(true);
					state = ES_DEASSERT_1;
					
					// Would need to ensure multiplexer control signals are false here, 
					// but they are initialized as so, so no need to do it again.
					break;
				
				case ES_DEASSERT_1:
					// Deassert Reg Values
					t_register_load.write(false);
					u_register_load.write(false);
					a0_register_load.write(false);
					a1_register_load.write(false);
					state = ES_PROCESS_IF_1;
					break;
				
				case ES_PROCESS_IF_1:
					t_register_load.write(true);
					u_register_load.write(true);
					a0_register_load.write(true);
					a1_register_load.write(true);
					
					a1_multiplexer_cont.write(true);
					t_multiplexer_cont.write(true); 
					u_multiplexer_cont.write(true);
					state = ES_DEASSERT_2;
					break;
					
				case ES_DEASSERT_2:
					// Deassert Reg Values
					t_register_load.write(false);
					u_register_load.write(false);
					a0_register_load.write(false);
					a1_register_load.write(false);
					state = ES_PROCESS_IF_2;
					break;
				
				case ES_PROCESS_IF_2:
					a0_register_load.write(true);
					a1_out_register_load.write(true);
				
					a0_multiplexer_cont.write(true);
					state = OUTPUT_STATE;
					break;
				
				/*
				case EXECUTE_STATE:
				    //cout << "process_hw_mult: EXECUTE_STATE" << endl;
					// Read inputs	
					b = in_data_1.read();
					c = in_data_2.read();
					
					// Original code from NN_DigitMult()...		
					bHigh = (NN_HALF_DIGIT)HIGH_HALF (b);
					bLow = (NN_HALF_DIGIT)LOW_HALF (b);
					cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
					cLow = (NN_HALF_DIGIT)LOW_HALF (c);

					a[0] = (NN_DIGIT)bLow * (NN_DIGIT)cLow;
					t = (NN_DIGIT)bLow * (NN_DIGIT)cHigh;
					u = (NN_DIGIT)bHigh * (NN_DIGIT)cLow;
					a[1] = (NN_DIGIT)bHigh * (NN_DIGIT)cHigh;
				  
					if ((t += u) < u) a[1] += TO_HIGH_HALF (1);
					u = TO_HIGH_HALF (t);
				  
					if ((a[0] += u) < u) a[1]++;
					a[1] += HIGH_HALF (t);
					
					state = OUTPUT_STATE;
					break;
				*/	
					
				case OUTPUT_STATE:
				        //cout << "process_hw_mult: OUTPUT_STATE" << endl;
					// Write outputs
					out_data_low.write(a0_register_out);
					out_data_high.write(a1_out_register_out);
					
					hw_mult_done.write(true); // assert multiplication is done
					state = FINISH_STATE;
					break;
					
					
				case FINISH_STATE:
				        //cout << "process_hw_mult: FINISH_STATE" << endl;
					if(hw_mult_enable.read() == false)
					{
						hw_mult_done.write(false);
						state = WAIT_STATE;
						
					}
					//wait();
					break;
				
			
			} // end switch
			wait();
		}// end while true
		
			  
	}
