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
	
	// Control Signals (From FSM)
	sc_signal <bool> b_register_load;
	sc_signal <bool> c_register_load;
	sc_signal <bool> t_register_load;
	sc_signal <bool> u_register_load;
	sc_signal <bool> a0_register_load;
	sc_signal <bool> a1_register_load;
	sc_signal <bool> a1_out_register_load;
	
	sc_signal <bool> LT_if_1;
	sc_signal <bool> LT_if_2;
	
	// Intermediate Signals
	
	sc_signal <NN_DIGIT> breg_to_bsplitter;
	sc_signal <NN_DIGIT> bsplitter_bhigh;
	sc_signal <NN_DIGIT> bsplitter_blow;
	
	sc_signal <NN_DIGIT> creg_to_csplitter;
	sc_signal <NN_DIGIT> csplitter_chigh;
	sc_signal <NN_DIGIT> csplitter_clow;
	
	sc_signal <NN_DIGIT> a0_multiplier_out;
	sc_signal <NN_DIGIT> a1_multiplier_out;
	sc_signal <NN_DIGIT> t_multiplier_out;
	sc_signal <NN_DIGIT> u_multiplier_out;
	
	
	sc_signal <NN_DIGIT> a0_register_in;
	sc_signal <NN_DIGIT> a1_register_in;
	sc_signal <NN_DIGIT> a1_out_register_in;
	sc_signal <NN_DIGIT> t_register_in;
	sc_signal <NN_DIGIT> u_register_in;
	
	sc_signal <NN_DIGIT> a0_register_out;
	sc_signal <NN_DIGIT> a1_register_out;
	sc_signal <NN_DIGIT> a1_out_register_out;
	sc_signal <NN_DIGIT> t_register_out;
	sc_signal <NN_DIGIT> u_register_out;
	
	
	// adder output temp sigs
	sc_signal <NN_DIGIT> t_u_adder_out;      
	sc_signal <NN_DIGIT> a1_offset_adder_out;
	sc_signal <NN_DIGIT> a0_u_adder_out;    
	sc_signal <NN_DIGIT> a1_offset_2_adder_out;
	// sc_signal <NN_DIGIT> a1_hht_adder_out;  // same as a1_out_register_in
	
	sc_signal <NN_DIGIT> a1_if_multiplexer_out;
	sc_signal <NN_DIGIT> a1_out_multiplexer_out;
	
	// Offset values for adders
	NN_DIGIT offset_if_1 = 0b000100000000;
	NN_DIGIT offset_if_2 = 0b000000000001;
	
	// T shifts
	sc_signal <NN_DIGIT> t_left_shifted;
	sc_signal <NN_DIGIT> t_highhalfed;
	
	highhalf t_highhalf_module;
	t_highhalf_module.input(t_register_out);    t_highhalf_module.output(t_left_shifted);
	tohighhalf t_tohighhalf_module;
	t_tohighhalf_module.input(t_register_out);    t_tohighhalf_module.output(t_highhalfed);
	
	
	//Registers
	async_reg b_register, c_register, t_register, u_register, a0_register, a1_register, a1_out_register;
	
	b_register.clock(hw_clock);        b_register.input(in_data_1);                 b_register.load(b_register_load);              b_register.output(breg_to_bsplitter);
	c_register.clock(hw_clock);        c_register.input(in_data_2);                 c_register.load(c_register_load);              c_register.output(creg_to_csplitter);
	t_register.clock(hw_clock);        t_register.input(t_register_in);             t_register.load(t_register_load);              t_register.output(t_register_out);
	u_register.clock(hw_clock);        u_register.input(u_register_in);             u_register.load(u_register_load);              u_register.output(u_register_out);
	a0_register.clock(hw_clock);       a0_register.input(a0_register_in);           a0_register.load(a0_register_load);            a0_register.output(a0_register_out);
	a1_register.clock(hw_clock);       a1_register.input(a1_register_in);           a1_register.load(a1_register_load);            a1_register.output(a1_register_out);
	a1_out_register.clock(hw_clock);   a1_out_register.input(a1_out_register_in);   a1_out_register.load(a1_out_register_load);    a1_out_register.output(a1_out_register_out);
	
	
	// Splitters
	splitter_32bit bsplitter, csplitter;
	
	bsplitter.input(breg_to_bsplitter);   bsplitter.highoutput(bsplitter_bhigh);   bsplitter.lowoutput(bsplitter_blow);
	csplitter.input(creg_to_csplitter);   csplitter.highoutput(csplitter_chigh);   csplitter.lowoutput(csplitter_clow);
	
	
	// Multipliers
	two_in_multiplier a0_multiplier, a1_multiplier, t_multiplier, u_multiplier;
	
	a0_multiplier.input1(bsplitter_blow);   a0_multiplier.input2(csplitter_clow);   a0_multiplier.output(a0_multiplier_out);
	a1_multiplier.input1(bsplitter_bhigh);  a1_multiplier.input2(csplitter_chigh);  a1_multiplier.output(a1_multiplier_out);
	t_multiplier.input1(bsplitter_blow);    t_multiplier.input2(csplitter_chigh);   t_multiplier.output(t_multiplier_out);
	u_multiplier.input1(bsplitter_bhigh);   u_multiplier.input2(csplitter_clow);    u_multiplier.output(u_multiplier_out);
	
	
	// Multiplexers
	two_in_multiplexer a0_multiplexer, a1_multiplexer, t_multiplexer, u_multiplexer, a1_if_multiplexer, a1_out_multiplexer
	
	a0_multiplexer.input1(a0_u_adder_out);              a0_multiplexer.input2(a0_multiplier_out);           a0_multiplexer.output(a0_register_in);
	a1_multiplexer.input1(a1_if_multiplexer_out);       a1_multiplexer.input2(a1_multiplier_out);           a1_multiplexer.output(a1_register_in);
	t_multiplexer.input1(t_u_adder_out);                t_multiplexer.input2(t_multiplier_out);             t_multiplexer.output(t_register_in);
	u_multiplexer.input1(t_left_shifted);               u_multiplexer.input2(u_multiplier_out);             u_multiplexer.output(u_register_in);
	a1_if_multiplexer.input1(a1_register_out);          a1_if_multiplexer.input2(a1_offset_adder_out);      a1_if_multiplexer.output(a1_if_multiplexer_out);
	a1_out_multiplexer.input1(a1_offset_2_adder_out);   a1_out_multiplexer.input2(a1_if_multiplexer_out);   a1_out_multiplexer.output(a1_out_multiplexer_out):
	
	
	// Adders
	two_in_adder t_u_adder, a1_offset_adder, a0_u_adder, a1_offset_2_adder, a1_hht_adder;
	
	t_u_adder.input1(u_register_out);              t_u_adder.input2(t_register_out);                   t_u_adder.output(t_u_adder_out);        
	a1_offset_adder.input1(a1_register_out);       a1_offset_adder.input2(offset_if_1);                a1_offset_adder.output(a1_offset_adder_out);
	a0_u_adder.input1(a0_register_out);            a0_u_adder.input2(u_register_out);                  a0_u_adder.output(a0_u_adder_out);
	a1_offset_2_adder.input1(offset_if_2);         a1_offset_2_adder.input2(a1_if_multiplexer_out);    a1_offset_2_adder.output(a1_offset_2_adder_out);                     // From the if mux
	a1_hht_adder.input1(a1_out_multiplexer_out);   a1_hht_adder.input2(t_highhalfed);                  a1_hht_adder.output(a1_out_register_in);                        // Not actually t, but highhalf t
	
	
	// Comparators (Only care about less than.
	LT_comparator comparator_if_1, comparator_if_2;
	
	comparator_if_1.input1(t_u_adder_out);    comparator_if_1.input2(u_register_out);   comparator_if_1.LT(LT_if_1);
	comparator_if_2.input1(a0_u_adder_out);   comparator_if_2.input2(u_register_out);   comparator_if_2.LT(LT_if_2);
  
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
