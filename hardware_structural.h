
#include "systemc.h"

// Used to seperate inputs B and C into their high and low parts.
SC_MODULE(splitter_32bit)
{
    sc_in  <NN_DIGIT>      input;
    sc_out <NN_HALF_DIGIT> highoutput, lowoutput;

    void split()
    {
		highout.write(input.read() & MAX_NN_HALF_DIGIT);
		lowout.write((input.read() >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT);
    }

    SC_CTOR (splitter_32bit)
    {
        SC_METHOD(split);
        sensitive << input;
    }

};


SC_MODULE(async_reg)
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
	
	SC_CTOR (async_reg)
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

    void float_add()
    {
        output.write( (input1.read() & control.read()) | (input2.read() & ~control.read()) );
    }

    SC_CTOR (two_in_multiplexer)
    {
        SC_METHOD(float_add);
        sensitive << input1 << input2 << control;
    }

};


SC_MODULE(LT_comparator)
{
	sc_in  <NN_DIGIT> input1, input2;
	sc_out <bool> LT; //, EQ, GT;
	
	void compare()
	{
		LT.write(false);
		//EQ.write(false);
		//GT.write(false);
		
		//if (input1.read() == input2.read()) EQ.write(true);
		//if (input1.read() >  input2.read()) GT.write(true);
		if (input1.read() <  input2.read()) LT.write(true);
	}
	
	SC_CTOR (comparator)
    {
        SC_METHOD(compare);
        sensitive << input1 << input2;
    }
};


SC_MODULE(highhalf)
{
	sc_in  <NN_DIGIT> input;
	sc_out <NN_DIGIT> output;
	
	void gethighhalf()
	{
		output.write(input.read() >> 16);
	}
	
	SC_CTOR (highhalf)
    {
        SC_METHOD(gethighhalf);
        sensitive << input;
    }
	
};


SC_MODULE(tohighhalf)
{
	sc_in  <NN_DIGIT> input;
	sc_out <NN_DIGIT> output;
	
	void tohighhalf()
	{
		output.write(input.read() << 16);
	}
	
	SC_CTOR (tohighhalf)
    {
        SC_METHOD(tohighhalf);
        sensitive << input;
    }
	
};



