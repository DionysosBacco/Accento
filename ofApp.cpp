#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel("Pd", OF_LOG_VERBOSE);
    ofSetWindowTitle("Acccccennnttttooooo");
    //guido.GuidoComponent();
    //path to fonts has to be setted manually
    guido.GuidoInit("/Users/luisvaldivia/Projekt/Guido/guidolib-1.60-src/environments/opengl/applications/GuidoOpenGL/GuidoGL/times.ttf","/Users/luisvaldivia/Projekt/Guido/guidolib-1.60-src/environments/opengl/applications/GuidoOpenGL/GuidoGL/guido2.ttf");
    guido.setGMNFile("/Users/luisvaldivia/Projekt/Guido/guidolib-1.60-src/gmn-examples/complex_files/bach.gmn");//path to guido file
    //guido.setGMNFile("/Users/luisvaldivia/Projekt/Metro/Metro-PD/GMN/invention-2-bach.gmn");
    
    guido.setScoreColor(ofColor::black);
    guido.setResizePageToMusic(true);
    guido.setSize(500,700);
    ofBackground(ofColor::white);
    showCursor = false;
    counter = 1;
    upbeat = 0;
    int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    int numInputs = 1;
    int numOutputs = 2;
    // setup OF sound stream
    ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    // allocate pd instance handles
    if(!pd.init(2, numInputs, 44100, ticksPerBuffer, false)) {
        std::cerr<<"PD library no initialisation"<<std::endl;
        //OF_EXIT_APP(1);
    }
    // audio processing on
    pd.start();
    pd.openPatch("/Users/luisvaldivia/OF/of_v0.9.8_osx_release/apps/myApps/Accento/src/pd/metro-main.pd"); //path to pd patch
    pd.subscribe("metro-bang");
    pd.addReceiver(*this);
    //midi
    //midi = std::make_shared<MidiInput>();
    //midi->init();
    record = false;
    isDraw = false;
}

void ofApp::printScoreInformation() {
    for(int i = 0; i<score_info.size(); i++)
        for(int j = 0; j<score_info.at(i).size(); j++)
            cout<<"scoreinfos size: "<<score_info.size()<<", at(i).size:"<<score_info.at(i).size()<<" Voice: "<<score_info.at(i).at(j)->voiceNum<<" Type: "<<score_info.at(i).at(j)->type <<" MidiPitch: "<<score_info.at(i).at(j)->midiPitch<<" Pitch: "<<score_info.at(i).at(j)->pitch<<" Duration: "<<score_info.at(i).at(j)->duration<<" Numerator: "<<score_info.at(i).at(j)->numerator<<" Denominator: "<<score_info.at(i).at(j)->denominator<<" hasTie: "<<score_info.at(i).at(j)->hasTie<< " hasStacatto: "<<score_info.at(i).at(j)->hasStacatto<<" Intensity: "<<score_info.at(i).at(j)->intensity<<" Detune: "<<score_info.at(i).at(j)->detune <<", isRight:"<<score_info.at(i).at(j)->is_right <<endl;
}

void ofApp::getScoreInformation() {
    if(isDraw) {
        GuidoScoreMapper(guido.getARHandler(), score_info);
    }
}

void ofApp::resetCounter() {
    counter = 1;
    upbeat = 0;
    coords.clear();
}

void ofApp::processPlayedInformation() {
    for(int i = 0; i<score_info.size(); i++)
        for(int j = 0; j<score_info.at(i).size(); j++){
            if(j<finalInfo.size() && score_info.at(i).at(j)->midiPitch != finalInfo.at(j).noteNumber) {
                score_info.at(i).at(j)->is_right = false;
            }
            else if(j>= finalInfo.size()) {                
                score_info.at(i).at(j)->is_right = false;
            }
        }
}

//--------------------------------------------------------------
void ofApp::update(){
    /*if(pd.isQueued()) {
        // process any received messages, if you're using the queue
        pd.receiveMessages();
    }*/
    /*if(record)
        midi->recordStream(true);*/
}

void ofApp::startMetronome() {
    cout<<"Start Metronome"<<endl;
    pd.sendBang("switch");
    //midi->openStream();
    //midi->recordStream(true);
    record = true;
    //update();
}

void ofApp::stop() {
    //pd.stop();
    cout<<"Stop"<<endl;
    pd.sendBang("switch");
    record = false;
    //midi->recordStream(false);
    //midi->filterVector();
    //midi->printFinalInfo();
    //midi->printInfo();
    //midi->closeStream()
    showCursor = 0;
    coords.clear();
    //pd.closePatch("pd/metro-main");
    //pd.stop();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    guido.draw(0,0,500,700);
    isDraw = true;
    if(showCursor) {
        //cout<<"drawCursor"<<endl;
        guido.cursorDraw();
    }
    if(record) {
        //midi->recordStream(true);
    }
}

void ofApp::processCoordinates() {
    if(counter<coords.size()) {
        p1.x = coords[counter].first;
        p1.y = coords[counter].second;
        p2.x = p1.x -50;
        p2.y = p1.y -50;
        p3.x = p1.x + 50;
        p3.y = p1.y - 50;
        //cout<<"ProcessCoordinates x:"<<p1.x<<", y:"<<p1.y<<", p2.x:"<<p2.x<<", p2.y:"<<p2.y<<", p3.x:"<<p3.x<<", p3.y:"<<p3.y<<", coords.size:"<<coords.size()<<" counter:"<<counter<<endl;
        counter++;
        guido.cursor(p1, p2, p3);
    }
    else {
        counter = 0;
        stop();
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        coords = guido.computeCoordinates();
        cout<<"Reading coordinates"<<endl;
        p1.x = coords[0].first;
        p1.y = coords[0].second;
        p2.x = p1.x -50;
        p2.y = p1.y -50;
        p3.x = p1.x + 50;
        p3.y = p1.y -50;
        //cout<<"drawing Cursor at x:"<<p1.x<<", y:"<<p1.y<<", p2.x:"<<p2.x<<", p2.y:"<<p2.y<<", p3.x:"<<p3.x<<", p3.y:"<<p3.y<<", coords.size:"<<coords.size()<<" counter:"<<counter<<endl;
        
        guido.cursor(p1, p2, p3);
        
        showCursor = true;
    }
    else if(key == 'm')
        startMetronome();
    else if (key == 's')
        stop();
    else if (key == 'p') {
        getScoreInformation();
        printScoreInformation();
    }
    else if (key == 'a') {
        //finalInfo = midi->getFinalInfo();
        //processPlayedInformation();
        //printScoreInformation();
    }
    else if(key == 'e') {
        finalInfo.clear();
        coords.clear();
        score_info.clear();
    }
    
}

void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
    pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
    pd.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::print(const std::string& message) {
    cout << message << endl;
}

//--------------------------------------------------------------
void ofApp::receiveBang(const std::string& dest) {
    cout << "OF: bang " << dest << endl;
    if(upbeat < 4)
        upbeat++;
    else
        processCoordinates();
}

void ofApp::receiveFloat(const std::string& dest, float value) {
    cout << "OF: float " << dest << ": " << value << endl;
}

void ofApp::receiveSymbol(const std::string& dest, const std::string& symbol) {
    cout << "OF: symbol " << dest << ": " << symbol << endl;
}

void ofApp::receiveList(const std::string& dest, const pd::List& list) {
    cout << "OF: list " << dest << ": ";
    
    // step through the list
    for(int i = 0; i < list.len(); ++i) {
        if(list.isFloat(i))
            cout << list.getFloat(i) << " ";
        else if(list.isSymbol(i))
            cout << list.getSymbol(i) << " ";
    }
    
    // you can also use the built in toString function or simply stream it out
    // cout << list.toString();
    // cout << list;
    
    // print an OSC-style type string
    cout << list.types() << endl;
}

void ofApp::receiveMessage(const std::string& dest, const std::string& msg, const pd::List& list) {
    cout << "OF: message " << dest << ": " << msg << " " << list.toString() << list.types() << endl;
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
