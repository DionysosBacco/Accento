//
//  MidiInput.hpp
//  Accento
//
//  Created by Luis Valdivia on 18.04.17.
//
//

#ifndef MidiInput_hpp
#define MidiInput_hpp
#include <iostream>
#include <portmidi.h>
#include <porttime.h>
#include <vector>
//#include <stdio>

struct MidiInfo {
    long messNumber;
    long status;
    long noteNumber;
    long velocity;
    long timeStamp;
};

class MidiInput {
public:
    MidiInput();
    ~MidiInput();
    void init();
    void openStream();
    void recordStream(bool t);
    void closeStream();
    void printInfo();
    void filterVector();
    void printFinalInfo();
    void clean();
    std::vector<MidiInfo> getFinalInfo();
private:
    int cnt, length, totalIn, deviceNum, iterator;
    long status, data1, data2;
    int command;
    const PmDeviceInfo *info;
    PmError retval;
    PmEvent *event;
    PmStream *midi;
    std::vector<MidiInfo> midi_info;
    std::vector<MidiInfo> final_info;
    uint32_t filter;
};


#endif /* MidiInput_hpp */
