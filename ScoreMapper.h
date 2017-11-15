

#ifndef ScoreMapper_h
#define ScoreMapper_h
#include "GSystemOSX.h" //Attebcion, only for OSX
#include "GUIDOEngine.h"
#include "GUIDOScoreMap.h"
#include "VGDevice.h"
//#include "Guido2MIDI.h"
#include "GuidoMapCollector.h"
#include "TimeUnwrap.h"
#include "GUIDOTypes.h"
#include "GObject.h"
#include "defines.h"
//#include "/Users/luisvaldivia/Projekt/Guido/guidolib/src/engine/lib/FontManager.h"
#include <memory>

//class ARMusicalObject;
//class ARMusicalVoice;
//Tieend and tiebegin!!
typedef struct {
    TYPE_DURATION duration;
    int voiceNum = 0;
    int staffNum = 0;
    int midiPitch = 0;
    int pitch = 0;
    int intensity = 0;
    int numerator = 0;
    int denominator = 0;
    GuidoElementType type = kEmpty;
    float detune = 0;
    bool hasTie = false;
    bool hasStacatto = false;
    long dur_mili = 0;
    bool is_right = true;
    std::string noteN;
} GuidoInfos;

class ScoreMapper : public TimeUnwrap {
private:
    int intensity;
    bool has_tie;
    bool has_stacatto;
    bool fChord;
    int fTicks;
    bool fDone;
    std::vector<std::shared_ptr<GuidoInfos>> score_infos;
    int	Ticks (TYPE_DURATION duration);
    void IntensChange(const ARMusicalObject * ev);   
protected:
    virtual void	Event (const ARMusicalObject * ev, EventType type);
    virtual void	Note (const ARMusicalObject * ev);
    virtual void    Rest (const ARMusicalObject * ev);
public:
    ScoreMapper(ARMusicalVoice* voice);
    virtual ~ScoreMapper() {}
    virtual     void AtPos (const ARMusicalObject * ev, EventType type);
    void Clear();
    std::vector<std::shared_ptr<GuidoInfos>> getInfos();
    void printInformation (std::ostream& os) const;
};

GuidoErrCode GuidoScoreMapper(const ARHandler ar, std::vector<std::vector<std::shared_ptr<GuidoInfos>>> &map);

#endif /* ScoreMapper_h */
