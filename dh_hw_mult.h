#include "systemc.h"
#include "digit.h"
#include <iostream>
#include "hardware_structural.h"

#ifndef _DH_HW_MULT_H_
#define _DH_HW_MULT_H_ 1


#define WAIT_STATE      0
#define EXECUTE_STATE   1
#define ES_MULT_LOAD    2
#define ES_DEASSERT_1   3
#define ES_PROCESS_IF_1 4
#define ES_DEASSERT_2   5
#define ES_PROCESS_IF_2 6
#define OUTPUT_STATE    7
#define FINISH_STATE    8

SC_MODULE (dh_hw_mult)
{
	sc_in<bool> hw_mult_enable; 
	sc_in_clk hw_clock;
	
	sc_in<NN_DIGIT> in_data_1;
	sc_in<NN_DIGIT> in_data_2;
	sc_out<NN_DIGIT> out_data_low;
	sc_out<NN_DIGIT> out_data_high;
	
	sc_out<bool> hw_mult_done;
	
	UINT4 state;
	
	void process_hw_mult();
	
	// STRUCTURAL INTERCONNECT OF EXECUTE STATE MULTIPLIER
	
	//Registers
	async_reg b_register, c_register, t_register, u_register, a0_register, a1_register, a1_out_register;
	
	
	// Register Clocks
	b_register.clock(hw_clock);
	c_register.clock(hw_clock);
	t_register.clock(hw_clock);
	u_register.clock(hw_clock);
	a0_register.clock(hw_clock);
	a1_register.clock(hw_clock);
	a1_out_register.clock(hw_clock);
	
	
	
  
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
