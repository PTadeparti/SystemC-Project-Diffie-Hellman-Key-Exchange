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

    void bonus_process() {
        NN_DIGIT t[2], c;
        NN_HALF_DIGIT aHigh, cLow, cHigh;
        while (1) {
            if (ready.read()) {
                t[0] = T0.read();
                t[1] = T1.read();
                c = C.read();
                aHigh = AH.read();
                cHigh = (NN_HALF_DIGIT) HIGH_HALF(c);
                cLow = (NN_HALF_DIGIT) LOW_HALF(c);

                while ((t[1] > cHigh) || ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF(cLow)))) {
                    if ((t[0] -= TO_HIGH_HALF(cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF(cLow)) t[1]--;
                    t[1] -= cHigh;
                    aHigh++;
                }
                T0new.write(t[0]);
                T1new.write(t[1]);
                AHnew.write(aHigh);
            }
            wait();
        }
    }

    SC_CTOR(bonusPart) {
        SC_CTHREAD(bonus_process, clock.pos());
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





