//#ifndef __GuidoComponent__
//#define __GuidoComponent__
#pragma once

#include "ofMain.h"
#include "ofxPd.h"
#include "GuidoComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "MidiInput.hpp"
#include "ScoreMapper.h"

class ofApp : public ofBaseApp, public pd::PdReceiver {
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // audio callbacks
    void audioReceived(float * input, int bufferSize, int nChannels);
    void audioRequested(float * output, int bufferSize, int nChannels);
    
    // pd message receiver callbacks
    void print(const std::string& message);
    
    void receiveBang(const std::string& dest);
    void receiveFloat(const std::string& dest, float value);
    void receiveSymbol(const std::string& dest, const std::string& symbol);
    void receiveList(const std::string& dest, const pd::List& list);
    void receiveMessage(const std::string& dest, const std::string& msg, const pd::List& list);
    void resetCounter();
    void startMetronome();
    void stop();
    void printScoreInformation();
    void getScoreInformation();
    void processPlayedInformation();
private:
    void processCoordinates();
    GuidoComponent guido;
    bool showCursor;
    std::vector<std::pair<float, float>> coords;
    glm::vec2 p1;
    glm::vec2 p2;
    glm::vec2 p3;
    int counter;
    ofxPd pd;
    vector<float> scopeArray;
    vector<pd::Patch> instances;
    int upbeat;
    std::shared_ptr<MidiInput> midi;
    bool record, isDraw;
    std::vector<std::vector<std::shared_ptr<GuidoInfos>>> score_info;
    std::vector<MidiInfo> finalInfo;
};
//#endif
