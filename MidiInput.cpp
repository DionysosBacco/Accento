//
//  MidiInput.cpp
//  Accento
//
//  Created by Luis Valdivia on 18.04.17.
//
//

#include "MidiInput.hpp"
#define NOTEON 0x90
#define NOTEOFF 0x80
#define MIDI_CODE_MASK  0xf0

MidiInput::MidiInput() : deviceNum(-1), totalIn(0), iterator(1) {}

MidiInput::~MidiInput() {
    Pm_Terminate();
}

void MidiInput::init() {
    event = (PmEvent *)malloc(sizeof(PmEvent)); //allocate memory for the event
    Pm_Initialize();
}

void MidiInput::closeStream() {
    std::cout<<"close Midi Stream"<<std::endl;
    Pm_Close(midi);
}

void MidiInput::openStream() {
    for (unsigned i = 0; i < Pm_CountDevices(); i++) {
        info = Pm_GetDeviceInfo(i);
        std::cout<<"Midi Devices: "<<i<<", "<<info->name<<", "<<info->interf<<std::endl;
    }
    deviceNum = 0;
    Pt_Start(1, 0, 0); /* timer started w/millisecond accuracy */
    retval = Pm_OpenInput(&midi, deviceNum, NULL, 512, NULL, NULL);
    if(retval != pmNoError) {
        std::cerr<<"ERRRRORRR, cann't open device number: "<<deviceNum<<std::endl;
    }
}

void MidiInput::recordStream(bool t) {
    int i = 0;
    if(t) {
        status = Pm_Poll(midi);
        length = Pm_Read(midi, event, sizeof(long));
        command = status & MIDI_CODE_MASK;
        //if (length>0 && (command == NOTEON || command == NOTEOFF)) {
        //
        //if ((long)Pm_MessageData2(event[0].message>0)) {
            midi_info.emplace_back(MidiInfo{iterator, (long) Pm_MessageStatus(event[0].message), (long)Pm_MessageData1(event[0].message), (long)Pm_MessageData2(event[0].message), (long)event[0].timestamp });
            iterator++;
       // }
        
    }
    //closeStream();
}

void MidiInput::printInfo() {
    std::cout<<"printINFO: "<<midi_info.size()<<std::endl;
    for(int i = 0; i<midi_info.size(); i++) {
        if(midi_info.at(i).velocity > 0)
            std::cout<<"midiinfos: "<<midi_info.at(i).messNumber<<", "<<midi_info.at(i).status<<", "<<midi_info.at(i).timeStamp<<", "<<midi_info.at(i).noteNumber<<", "<<midi_info.at(i).velocity<<std::endl;
    }
}

void MidiInput::filterVector() {
    for(int i = 0; i<midi_info.size(); i++) {
        if(midi_info.at(i).velocity > 0 && midi_info.at(i).timeStamp != midi_info.at(i+1).timeStamp)
            final_info.emplace_back(MidiInfo {midi_info.at(i).messNumber, midi_info.at(i).status, midi_info.at(i).noteNumber, midi_info.at(i).velocity, midi_info.at(i).timeStamp });
    }
}

void MidiInput::printFinalInfo() {
    std::cout<<"FinalInfo: "<<final_info.size()<<std::endl;
    for(int i = 0; i<final_info.size(); i++)
        std::cout<<"finalMidiInfos: "<<final_info.at(i).messNumber<<", "<<final_info.at(i).status<<", "<<final_info.at(i).timeStamp<<", "<<final_info.at(i).noteNumber<<", "<<final_info.at(i).velocity<<std::endl;
}

void MidiInput::clean() {
    midi_info.clear();
    final_info.clear();
}

std::vector<MidiInfo> MidiInput::getFinalInfo() {
    return final_info;
}
