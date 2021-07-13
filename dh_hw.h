#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "datapath_modules.h"

SC_MODULE(dh_hw) {
    
    sc_in_clk clock;
    sc_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
    sc_in <NN_HALF_DIGIT> from_sw3;

    sc_out <NN_DIGIT> to_sw0, to_sw1;
    sc_out <NN_HALF_DIGIT> to_sw2;

    sc_in <bool> hw_enable;
    sc_out <bool> hw_done, loadInput, loadOutput, readyBP;

    datapath DP;

    void process_hw();

    SC_CTOR(dh_hw) : DP("datapath") 
    {

        DP.from_sw0(from_sw0);
        DP.from_sw1(from_sw1);
        DP.from_sw2(from_sw2);
        DP.from_sw3(from_sw3);
        DP.loadInput(loadInput);
        DP.loadOutput(loadOutput);
        DP.readyBP(readyBP);
        DP.clock(clock);

        DP.to_sw0(to_sw0);
        DP.to_sw1(to_sw1);
        DP.to_sw2(to_sw2);

        SC_CTHREAD(process_hw, clock.pos());
    }
};

#endif /* end _DH_HW_H_ */
