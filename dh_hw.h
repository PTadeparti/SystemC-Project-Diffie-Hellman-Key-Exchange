#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "datapath_modules.h"

SC_MODULE (dh_hw)
{
  sc_in_clk clock;
  sc_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_in <NN_HALF_DIGIT> from_sw3;

  sc_signal<NN_DIGIT> t0,t1,t0in,t1in,cin, R0outsig,R1outsig;
  sc_signal<NN_HALF_DIGIT> aHighin, R2outsig, cLowSig, cHighSig;

  sc_out <NN_DIGIT> to_sw0, to_sw1;
  sc_out <NN_HALF_DIGIT> to_sw2;
  
  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done,loadInput,loadOutput;

  reg<NN_DIGIT> R0in,R1in,R2in;
  reg<NN_HALF_DIGIT> R3in;

  reg<NN_DIGIT> R0out,R1out;
  reg<NN_HALF_DIGIT> R2out;

	// Internal signals for ease of use
	sc_signal<NN_DIGIT> u,v,utoHH,uHH,uLH,s1out,s2out,s3out,s4out,mux1out,muxin1,muxin2,maxNNDIGIT,RPout1,RPout2;

	sc_signal<bool> c1out;

	
	splitter<NN_HALF_DIGIT> spl1;
	multiplier<NN_HALF_DIGIT> m1,m2;
	TO_HH to_hh;
	splitter<NN_DIGIT> spl2;
	subtractor s1,s2;
	comparator c1;
	multiplexer mux1;
	subtractor s3,s4,s5;

 // bonus BP;
   

  void process_hw();
  
  SC_CTOR (dh_hw) : R0in("R0in"), R1in("R1in"), R2in("R2in"), R3in("R3in"), R0out("R0out"), R1out("R1out"), R2out("R2out"), 
	spl1("spl1"), m1("m1"), m2("m2"), to_hh("to_hh"), spl2("spl2"), s1("s1"), s2("s2"), c1("c1"),mux1("mux1"),s3("s3"),s4("s4"),s5("s5")

  {

    
    R0in.a(from_sw0); R0in.load(loadInput); R0in.clock(clock); R0in.out(t0in);
    R1in.a(from_sw1); R1in.load(loadInput); R1in.clock(clock); R1in.out(t1in);
    R2in.a(from_sw2); R2in.load(loadInput); R2in.clock(clock); R2in.out(cin);
    R3in.a(from_sw3); R3in.load(loadInput); R3in.clock(clock); R3in.out(aHighin);
    
    // Set signals for CHigh/CLow, u,v
    spl1.a(cin); spl1.outHH(cHighSig); spl1.outLH(cLowSig);
    m1.a(aHighin); m1.b(cHighSig); m1.out(v);
    m2.a(aHighin); m2.b(cLowSig); m2.out(u); 

    // Signals for If statement
    to_hh.a(u); to_hh.out(utoHH);
    s1.a(t0in); s1.b(utoHH); s1.out(RPout1);
    maxNNDIGIT = MAX_NN_DIGIT; 
    s2.a(maxNNDIGIT); s2.b(utoHH); s2.out(s2out);
    c1.a(RPout1); c1.b(s2out); c1.out(c1out);
    muxin1.write(1); muxin2.write(0); 
    mux1.a(muxin1); mux1.b(muxin2); mux1.s(c1out); mux1.out(mux1out); 
    s3.a(t1in); s3.b(mux1out); s3.out(s3out);

    // Signals for output t[1]
    spl2.a(u); spl2.outHH(uHH); spl2.outLH(uLH);
    s4.a(s3out); s4.b(uHH); s4.out(s4out); 
    s5.a(s4out); s5.b(v); s5.out(RPout2);		

    R0out.a(R0outsig); R0out.load(loadOutput); R0out.clock(clock); R0out.out(to_sw0);
    R1out.a(R1outsig); R1out.load(loadOutput); R1out.clock(clock); R1out.out(to_sw1);
    R2out.a(R2outsig); R2out.load(loadOutput); R2out.clock(clock); R2out.out(to_sw2);

    SC_CTHREAD (process_hw, clock.pos()); 
  }
  
};

#endif /* end _DH_HW_H_ */
