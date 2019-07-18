
#include "systemc.h"
#include "digit.h"
#include "dh_hw_mult.h"
#include <iostream>


void dh_hw_mult::process_hw_mult()
	{
	    //cout << "process_hw_mult: Entered" << endl;
	    //NN_DIGIT a[2], b, c, t, u;
	    //NN_HALF_DIGIT bHigh, bLow, cHigh, cLow;
		
	    state = WAIT_STATE;
	    hw_mult_done.write(0);
	    
		while(true){
		  
			switch(state)
			{
				case WAIT_STATE:
					cout << "--------------------------------------------------" << endl;
					//cout << "process_hw_mult: WAIT_STATE" << endl;
					if(hw_mult_enable.read() == true)
					{
						b_register_load.write(true);
						c_register_load.write(true);
						
						// Re-init the control signals to all low.
						t_register_load.write(false);
						u_register_load.write(false);
						a0_register_load.write(false);
						a1_register_load.write(false);
						a1_out_register_load.write(false);
						
						a0_multiplexer_cont.write(false);
						a1_multiplexer_cont.write(false); 
						t_multiplexer_cont.write(false); 
						u_multiplexer_cont.write(false); 
						
						// clear output signals
						out_data_low.write(0);
						out_data_high.write(0);
						
						state = ES_STAGE_1;
					}
					break;
					
				case ES_STAGE_1:
					cout << "process_hw_mult: ES_STAGE_1" << endl;
					//cout << "Data: b = " << in_data_1 << ", c = " << in_data_2 << endl;
					//cout << "Data: b_reg = " << breg_to_bsplitter << ", c_reg = " << creg_to_csplitter << endl;
					// Ignore the input
					b_register_load.write(false);
					c_register_load.write(false);
					
					// Load in the new multiplication data.
					t_register_load.write(true);
					u_register_load.write(true);
					a0_register_load.write(true);
					a1_register_load.write(true);
					
					state = ES_STAGE_2;
					
					break;
				
				case ES_STAGE_2:
					cout << "process_hw_mult: ES_STAGE_2" << endl;
					cout << "Data: b_reg = " << breg_to_bsplitter.read() << ", c_reg = " << creg_to_csplitter.read() << endl;
					cout << " bHigh = " << bsplitter_bhigh.read() << ", bLow = " << bsplitter_blow.read() << endl;
					cout << " cHigh = " << csplitter_chigh.read() << ", cLow = " << csplitter_clow.read() << endl;
					// Deassert Reg Values
					// Note: t is still high to do the t += u calculation for the if statement and store it.
					u_register_load.write(false);
					a0_register_load.write(false);
					a1_register_load.write(false);
					
					a1_multiplexer_cont.write(true);
					t_multiplexer_cont.write(true); 
					u_multiplexer_cont.write(true);
					
					state = ES_STAGE_3;
					
					break;
				
				case ES_STAGE_3:
					cout << "process_hw_mult: ES_STAGE_3" << endl;
					cout << " a[0] = " << a0_register_out.read() << ", a[1] = " << a1_register_out.read() << endl;
					cout << " t = " << t_register_out.read() << ", u = " << u_register_out.read() << endl;
					t_register_load.write(false);
	
					u_register_load.write(true);
					a1_register_load.write(true);

					state = ES_STAGE_4;
					
					break;
					
				case ES_STAGE_4:
					cout << "process_hw_mult: ES_STAGE_4" << endl;
					cout << "After if_1" << endl;
					cout << " a[0] = " << a0_register_out.read() << ", a[1] = " << a1_register_out.read() << endl;
					cout << " t = " << t_register_out.read() << ", u = " << u_register_out.read() << endl;
					//cout << "U_in1 = " << t_got_highhalf << ", U_in2 = " << u_multiplier_out << ", u_cont = " << u_multiplexer_cont << endl; 

					u_register_load.write(false);
					a1_register_load.write(false);
					a0_register_load.write(true);
					
					a0_multiplexer_cont.write(true);
					
					state = ES_STAGE_5;
					
					break;
				
				case ES_STAGE_5:
					cout << "process_hw_mult: ES_STAGE_5" << endl;
					cout << " a[0] = " << a0_register_out.read() << ", a[1] = " << a1_register_out.read() << endl;
					cout << " t = " << t_register_out.read() << ", u = " << u_register_out.read() << endl;
					
					a0_register_load.write(false);
					a1_out_register_load.write(true);
					wait();
					a1_out_register_load.write(false);
				
					state = OUTPUT_STATE;
					break;
					
				case OUTPUT_STATE:
				        cout << "process_hw_mult: OUTPUT_STATE" << endl;
					//a0_register_load.write(false);
					//a1_out_register_load.write(false);
					// Write outputs
					out_data_low.write(a0_register_out.read());
					out_data_high.write(a1_out_register_out.read());
					//wait();
					hw_mult_done.write(true); // assert multiplication is done
					state = FINISH_STATE;
					break;
					
					
				case FINISH_STATE:
				        cout << "process_hw_mult: FINISH_STATE" << endl;
					cout << " a[0] = " << a0_register_out.read() << ", a[1] = " << a1_out_register_out.read() << endl;
					cout << "out_data_low = " << out_data_low.read() << ", out_data_high = " << out_data_high.read() << endl;
					if(hw_mult_enable.read() == false)
					{
						hw_mult_done.write(false);
						state = WAIT_STATE;
						
					}
					break;
				
			
			} // end switch
			
			wait();
			
		}// end while true
		
			  
	}
