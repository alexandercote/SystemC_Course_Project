#include "systemc.h"
#include "digit.h"
#include <iostream>

#ifndef _DH_HW_MULT_H_
#define _DH_HW_MULT_H_ 1


#define WAIT_STATE 0
#define EXECUTE_STATE 1
#define OUTPUT_STATE 2
#define FINISH_STATE 3

SC_MODULE (dh_hw_mult)
{
	sc_in<bool> hw_mult_enable; 
	sc_in_clk hw_clock;
	
	sc_in<NN_DIGIT> in_data_1;
	sc_in<NN_DIGIT> in_data_2;
	sc_out<NN_DIGIT> out_data_low;
	sc_out<NN_DIGIT> out_data_high;
	
	sc_out<bool> hw_mult_done;
	
	UINT2 state;

	//void process_hw_mult();
  
	void process_hw_mult()
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
						state = EXECUTE_STATE;
					}
					//wait(); // Figure out where to put this....
					break;
					
					
				case EXECUTE_STATE:
				    cout << "process_hw_mult: EXECUTE_STATE" << endl;
					// Read inputs	
					b = in_data_1.read();
					c = in_data_2.read();
					cout << "Data: b = " << b << ", c = " << c << endl;
					// Original code from NN_DigitMult()...		
					bHigh = (NN_HALF_DIGIT)HIGH_HALF (b);
					bLow = (NN_HALF_DIGIT)LOW_HALF (b);
					cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
					cLow = (NN_HALF_DIGIT)LOW_HALF (c);
					cout << " bHigh = " << bHigh << ", bLow = " << bLow << endl;
					cout << " cHigh = " << cHigh << ", cLow = " << cLow << endl;

					a[0] = (NN_DIGIT)bLow * (NN_DIGIT)cLow;
					t = (NN_DIGIT)bLow * (NN_DIGIT)cHigh;
					u = (NN_DIGIT)bHigh * (NN_DIGIT)cLow;
					a[1] = (NN_DIGIT)bHigh * (NN_DIGIT)cHigh;
					
					cout << " a[0] = " << a[0] << ", a[1] = " << a[1] << endl;
					cout << " t = " << t << ", u = " << u << endl;
					
					if ((t += u) < u) a[1] += TO_HIGH_HALF (1);
					u = TO_HIGH_HALF (t);
					
					cout << "After if_1" << endl;
					cout << " a[0] = " << a[0] << ", a[1] = " << a[1] << endl;
					cout << " t = " << t << ", u = " << u << endl;
				  
					if ((a[0] += u) < u) a[1]++;
					a[1] += HIGH_HALF (t);
					
					cout << "After if_2" << endl;
					cout << " a[0] = " << a[0] << ", a[1] = " << a[1] << endl;
					cout << " t = " << t << ", u = " << u << endl;
					
					state = OUTPUT_STATE;
					break;
					
					
				case OUTPUT_STATE:
				        //cout << "process_hw_mult: OUTPUT_STATE" << endl;
					// Write outputs
					out_data_low.write(a[0]);
					out_data_high.write(a[1]);
					
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
  
	SC_CTOR (dh_hw_mult)
	{
	    SC_CTHREAD (process_hw_mult, hw_clock.pos());
	    state = WAIT_STATE;
	}
  
};

#endif /* end _DH_HW_MULT_H_ */


/*

for (;;) {  
	  
			if (hw_mult_enable.read() == true) 
			{	

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
				
			// Hardware multiplication delay = 100 ns
			wait (100, SC_NS);
			
			// Write outputs
			out_data_low.write(a[0]);
			out_data_high.write(a[1]);
				
			}

			wait();		// wait for a change of hw_mult_enable	

	  }
	  
	  */
