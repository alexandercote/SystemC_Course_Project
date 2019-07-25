#include "systemc.h"
#include "digit.h"

// Used to seperate inputs B and C into their high and low parts.
SC_MODULE(splitter_32bit)
{
    sc_in  <NN_DIGIT>      input;
    sc_out <NN_DIGIT>      highoutput, lowoutput;

    void split()
    {
		lowoutput.write(input.read() & MAX_NN_HALF_DIGIT);
		highoutput.write((input.read() >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT);
    }

    SC_CTOR (splitter_32bit)
    {
        SC_METHOD(split);
        sensitive << input;
    }
};


SC_MODULE(sync_reg)
{
	sc_in_clk         clock;
	sc_in  <NN_DIGIT> input;
	sc_in  <bool>     load;
	sc_out <NN_DIGIT> output;
	
	void reg_update()
	{
		while(true)
		{
			if(load.read() == true)
			{
				output.write(input.read());
			}
			wait();
		}
	}
	
	SC_CTOR (sync_reg)
	{
		SC_CTHREAD(reg_update, clock.pos());
	}
};


SC_MODULE(two_in_adder)
{
    sc_in  <NN_DIGIT> input1, input2;
    sc_out <NN_DIGIT> output;

    void add()
    {
        output.write( input1.read() + input2.read() );
    }

    SC_CTOR (two_in_adder)
    {
        SC_METHOD(add);
        sensitive << input1 << input2;
    }
};


SC_MODULE(two_in_multiplier)
{
    sc_in  <NN_DIGIT> input1, input2;
    sc_out <NN_DIGIT> output;

    void multiply()
    {
        output.write( input1.read() * input2.read() );
    }

    SC_CTOR (two_in_multiplier)
    {
        SC_METHOD(multiply);
        sensitive << input1 << input2;
    }
};


SC_MODULE(two_in_multiplexer)
{
    sc_in  <NN_DIGIT> input1, input2;
    sc_in  <bool>     control;
    sc_out <NN_DIGIT> output;

    void multiplex()
    {
	if(control.read() == true) output.write(input1.read());
	else                       output.write(input2.read());
    }
    SC_CTOR (two_in_multiplexer)
    {
        SC_METHOD(multiplex);
        sensitive << input1 << input2 << control;
    }
};


SC_MODULE(LT_comparator)
{
	sc_in  <NN_DIGIT> input1, input2;
	sc_out <bool> LT; //, EQ, GT;
	
	void compare()
	{
		if (input1.read() <  input2.read()) LT.write(true);
		else                                LT.write(false);
	}
	
	SC_CTOR (LT_comparator)
	{
		SC_METHOD(compare);
		sensitive << input1 << input2;
	}
};


SC_MODULE(tohighhalf)
{
	sc_in  <NN_DIGIT> input;
	sc_out <NN_DIGIT> output;
	
	void tohighhalf_func()
	{
		output.write(input.read() << 16);
	}
	
    SC_CTOR (tohighhalf)
    {
        SC_METHOD(tohighhalf_func);
        sensitive << input;
    }
	
};


SC_MODULE(highhalf)
{
	sc_in  <NN_DIGIT> input;
	sc_out <NN_DIGIT> output;
	
	void highhalf_func()
	{
		output.write((input.read() >> 16) & 0xFFFF);
	}
	
	SC_CTOR (highhalf)
	{
	    SC_METHOD(highhalf_func);
	    sensitive << input;
	}
};



