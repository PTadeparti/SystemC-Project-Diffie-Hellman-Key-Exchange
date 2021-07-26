#ifndef _DATAPATH_MODULES_H
#define _DATAPATH_MODULES_H 1

#include "systemc.h"
#include "digit.h"

SC_MODULE(adder) {
    sc_in<NN_DIGIT> a, b;
    sc_out<NN_DIGIT> out;

    void add() {
        out.write(a.read() + b.read());
    }

    SC_CTOR(adder) {
        SC_METHOD(add);
        sensitive << a << b;
    }
};

SC_MODULE(subtractor) {
    sc_in<NN_DIGIT> a, b;
    sc_out<NN_DIGIT> out;

    void subtract() {
        out.write(a.read() - b.read());
    }

    SC_CTOR(subtractor) {
        SC_METHOD(subtract);
        sensitive << a << b;
    }
};

template <typename T>
SC_MODULE(equality) {
    sc_in<NN_DIGIT> a;
	sc_in<T> b;
    sc_out<bool> out;

    void runEquals() {
        out.write(a.read() == b.read());
    }

    SC_CTOR(equality) {
        SC_METHOD(runEquals);
        sensitive << a << b;
    }
};

SC_MODULE(OR) {
    sc_in<NN_DIGIT> a, b;
    sc_out<bool> out;

    void runOr() {
       (a == 1 || b == 1) ? out.write(1) : out.write(0);
    }

    SC_CTOR(OR) {
        SC_METHOD(runOr);
        sensitive << a << b;
    }
};

SC_MODULE(AND) {
    sc_in<NN_DIGIT> a, b;
    sc_out<bool> out;

    void runAnd() {
       (a == 1 && b == 1) ? out.write(1) : out.write(0);
    }

    SC_CTOR(AND) {
        SC_METHOD(runAnd);
        sensitive << a << b;
    }
};

template <typename T>
SC_MODULE(multiplier) {
    sc_in<T> a, b;
    sc_out<NN_DIGIT> out;

    void multiply() {
        out.write((NN_DIGIT) a.read() * (NN_DIGIT) b.read());
    }

    SC_CTOR(multiplier) {
        SC_METHOD(multiply);
        sensitive << a << b;
    }
};

SC_MODULE(comparator) {
    sc_in<NN_DIGIT> a, b;
    sc_out<bool> out;

    void compare() {
        a < b ? out.write(1) : out.write(0);
    }

    SC_CTOR(comparator) {
        SC_METHOD(compare);
        sensitive << a << b;
    }
};

template <typename T>
SC_MODULE(reg) {
    sc_in<T> a;
    sc_in_clk clock;
    sc_in<bool> load;
    sc_out<T> out;

    void send() {
        while (1) {
            if (load.read()) {
                out.write(a);
            }
            wait();
        }
    }

    SC_CTOR(reg) {
        SC_CTHREAD(send, clock.pos());
    }
};

SC_MODULE(multiplexer) {
    sc_in<NN_DIGIT> a, b;
    sc_in<bool> s;
    sc_out<NN_DIGIT> out;

    void multiplex() {
        s.read() ? out.write(b) : out.write(a);
    }

    SC_CTOR(multiplexer) {
        SC_METHOD(multiplex);
        sensitive << a << b << s;
    }
};

template <typename T>
SC_MODULE(splitter) {
    sc_in<NN_DIGIT> a;
    sc_out<T> outHH, outLH;

    void split() {
        outHH.write((T) HIGH_HALF(a));
        outLH.write((T) LOW_HALF(a));
    }

    SC_CTOR(splitter) {
        SC_METHOD(split);
        sensitive << a;
    }
};

SC_MODULE(TO_NNDIGIT) {
    sc_in<NN_HALF_DIGIT> a;
    sc_out<NN_DIGIT> out;

    void doTO_NNDIGIT() {
        out.write((NN_DIGIT)()a.read());
    }

    SC_CTOR(TO_NNDIGIT) {
        SC_METHOD(doTO_NNDIGIT);
        sensitive << a;
    }
};

SC_MODULE(TO_HH) {
    sc_in<NN_DIGIT> a;
    sc_out<NN_DIGIT> out;

    void doTO_HH() {
        out.write(TO_HIGH_HALF(a));
    }

    SC_CTOR(TO_HH) {
        SC_METHOD(doTO_HH);
        sensitive << a;
    }
};

SC_MODULE(requiredPart) {
    // External Signals
    sc_in_clk clock;
    sc_in<NN_DIGIT> t0, t1, c;
    sc_in<NN_HALF_DIGIT> aHigh;
    sc_out <NN_DIGIT> out1, out2;    

    // Internal signals for ease of use
    sc_signal<NN_DIGIT> u, v, utoHH, uHH, uLH, s1out, s2out, s3out, s4out, mux1out, muxin1, muxin2, maxNNDIGIT;
    sc_signal<NN_HALF_DIGIT> cLow, cHigh;
    sc_signal<bool> c1out;

    splitter<NN_HALF_DIGIT> spl1;
    multiplier<NN_HALF_DIGIT> m1, m2;
    TO_HH to_hh;
    splitter<NN_DIGIT> spl2;
    subtractor s1, s2;
    comparator c1;
    multiplexer mux1;    
    subtractor s3, s4, s5;

    SC_CTOR(requiredPart) : spl1("spl1"), m1("m1"), m2("m2"), to_hh("to_hh"), spl2("spl2"), s1("s1"), s2("s2"), c1("c1"), mux1("mux1"), s3("s3"), s4("s4"), s5("s5") 
    {
        // Set signals for CHigh/CLow, u,v
        spl1.a(c);
        spl1.outHH(cHigh);
        spl1.outLH(cLow);
        m1.a(aHigh);
        m1.b(cHigh);
        m1.out(v);
        m2.a(aHigh);
        m2.b(cLow);
        m2.out(u);

        // Signals for If statement
        to_hh.a(u);
        to_hh.out(utoHH);
        s1.a(t0);
        s1.b(utoHH);
        s1.out(out1);
        maxNNDIGIT = MAX_NN_DIGIT;
        s2.a(maxNNDIGIT);
        s2.b(utoHH);
        s2.out(s2out);
        c1.a(out1);
        c1.b(s2out);
        c1.out(c1out);
        muxin1.write(1);
        muxin2.write(0);
        mux1.a(muxin1);
        mux1.b(muxin2);
        mux1.s(c1out);
        mux1.out(mux1out);
        s3.a(t1);
        s3.b(mux1out);
        s3.out(s3out);

        // Signals for output t[1]
        spl2.a(u);
        spl2.outHH(uHH);
        spl2.outLH(uLH);
        s4.a(s3out);
        s4.b(uHH);
        s4.out(s4out);
        s5.a(s4out);
        s5.b(v);
        s5.out(out2);

    }
};

SC_MODULE(bonusPart) {
    // External Signals
    sc_in_clk clock;
    sc_in<NN_DIGIT> T0, T1, C;
    sc_in<NN_HALF_DIGIT> AH;
    sc_in<bool> ready;
    sc_out <NN_DIGIT> T0new, T1new;
    sc_out<NN_HALF_DIGIT> AHnew;
    sc_out<bool> bonusDone;
    
     // Internal signals for ease of use
     
    sc_signal<NN_DIGIT> addin, cLowHH, c1out, s2out, s3out, s4out, mux1out, muxin1, muxin2, maxNNDIGIT,mux1out,cHighNNDIGIT;
    sc_signal<NN_HALF_DIGIT> cLow, cHigh;
    sc_signal<bool> c1out,eq1out c2out,and1out,or1out,c3out;

	// Modules for loop
    splitter<NN_HALF_DIGIT> spl1;       
    TO_HH to_hh;   
    comparator c1,c2;
    equality<NN_HALF_DIGIT> eq1;
    AND and1;
    OR or1;    
    
    // Modules for computation
    adder a1; 
    multiplexer mux1;
    TO_NNDIGIT tn1;
    subtractor s1,s2,s3,s4;
    comparator c3;
    

    SC_CTOR(bonusPart) : spl1("spl1"), to_hh("to_hh"),c1("c1"), c2("c2"), eq1("eq1"), and1("and1"), or1("or1"), 
	a1("a1"),  mux1("mux1"), tn1("tn1"), s1("s1"), s2("s2"),  s3("s3"), s4("s4"), c3("c3")
	{
    	// Loop Initialization    	
    	spl1.a(C); spl1.outHH(cHigh); spl1.outLH(cLow);
    	to_hh.a(cLow); to_hh.out(cLowHH);
    	c1.a(T1); c1.b(cHigh); c1.out(c1out);
    	eq1.a(T1); eq1.b(cHigh); eq1.out(eq1out);
    	c2.a(T0); c2.b(cLowHH); c2.out(c2out);
    	and1.a(eq1out); and1.b(c2out); and1.out(and1out);
    	or1.a(c1out); or1.b(and1out); or1.out(bonusDone);
    	
    	// Computation module initialization
    	addin.write(1);
    	a1.a(AH); a1.b(addin); a1.out(AHnew);
    	s1.a(T0); s1.b(cLowHH); s1.out(T0new);
    	s2.a(maxNNDIGIT); s2.b(cLowHH); s2.out(s2out);
    	c3.a(s2out); c3.b(T0new); c3.out(c3out);
    	muxin1.write(0); muxin2.write(1);
    	mux1.a(muxin1); mux1.b(muxin2); mux1.s(c3out); mux1.out(mux1out);
    	s3.a(T1); s3.b(mux1out); s3.out(s3out);
    	tn1.a(cHigh); tn1.out(cHighNNDIGIT);
    	s4.a(s3out); s4.b(cHighNNDIGIT); s4.out(T1new);  	
    	
       
    }
};

SC_MODULE(datapath) {
    
    // Input Signals from Software and FSM
    sc_in_clk clock;
    sc_in<NN_DIGIT> from_sw0, from_sw1, from_sw2;
    sc_in<NN_HALF_DIGIT> from_sw3;
    sc_in<bool> loadInput, loadOutput, readyBP;

    // Output Signals to Software
    sc_out<NN_DIGIT> to_sw0, to_sw1;
    sc_out<NN_HALF_DIGIT> to_sw2;
    sc_out<bool> BPdone;

    sc_signal<NN_DIGIT> t0inRP, t1inRP, cin, t0outRP, t1outRP, t0outBP, t1outBP;
    sc_signal<NN_HALF_DIGIT> aHighin, aHighOutBP;


    reg<NN_DIGIT> R0in, R1in, R2in;
    reg<NN_HALF_DIGIT> R3in;
    reg<NN_DIGIT> R0out, R1out;
    reg<NN_HALF_DIGIT> R2out;

    requiredPart RP;
    bonusPart BP;

    SC_CTOR(datapath) : R0in("R0in"), R1in("R1in"), R2in("R2in"), R3in("R3in"), R0out("R0out"), R1out("R1out"), R2out("R2out"), RP("RP"), BP("BP") {
        R0in.a(from_sw0);
        R0in.load(loadInput);
        R0in.clock(clock);
        R0in.out(t0inRP);
        R1in.a(from_sw1);
        R1in.load(loadInput);
        R1in.clock(clock);
        R1in.out(t1inRP);
        R2in.a(from_sw2);
        R2in.load(loadInput);
        R2in.clock(clock);
        R2in.out(cin);
        R3in.a(from_sw3);
        R3in.load(loadInput);
        R3in.clock(clock);
        R3in.out(aHighin);

        RP.t0(t0inRP);
        RP.t1(t1inRP);
        RP.c(cin);
        RP.aHigh(aHighin);
        RP.clock(clock);
        RP.out1(t0outRP);
        RP.out2(t1outRP);
        BP.T0(t0outRP);
        BP.T1(t1outRP);
        BP.C(cin);
        BP.AH(aHighin);
        BP.clock(clock);
        BP.ready(readyBP);
        BP.T0new(t0outBP);
        BP.T1new(t1outBP);
        BP.AHnew(aHighOutBP);
        BP.bonusDone(BPdone);

        R0out.a(t0outBP);
        R0out.load(loadOutput);
        R0out.clock(clock);
        R0out.out(to_sw0);
        R1out.a(t1outBP);
        R1out.load(loadOutput);
        R1out.clock(clock);
        R1out.out(to_sw1);
        R2out.a(aHighOutBP);
        R2out.load(loadOutput);
        R2out.clock(clock);
        R2out.out(to_sw2);

    }
};

#endif 





