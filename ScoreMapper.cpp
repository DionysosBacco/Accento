
#include <stdio.h>
#include "ScoreMapper.h"
#include "ARMusicalObject.h"
#include "ARNote.h"
#include "ARMusicalTag.h"
#include "ARRest.h"
#include "ARMusicalVoice.h"
#include "ARMusicalVoiceState.h"
#include "ARTie.h"
#include "secureio.h"
#include "GUIDOInternal.h"
#include "ARMusic.h"
#include "ARIntens.h"
#include "GuidoTypes.h"

ScoreMapper::ScoreMapper(ARMusicalVoice* voice) : TimeUnwrap(voice), fTicks(60), fDone(false), has_stacatto(false), has_tie(false), fChord(false), intensity(70) {}

int ScoreMapper::Ticks(TYPE_DURATION duration) {
    if (!duration.getNumerator() || !duration.getDenominator())
        return 0;
    return int((float)duration * 4 * fTicks);
}

void ScoreMapper::Rest(const ARMusicalObject *ev) {
    std::shared_ptr<GuidoInfos> guido(new GuidoInfos);
    guido->voiceNum = (ev ? ev->getVoiceNum() : 0);
    guido->voiceNum = ev->getVoiceNum();
    const ARRest  *ar = dynamic_cast<const ARRest *>(ev);
    guido->duration = ar->getDuration();
    guido->numerator = ar->getDuration().getNumerator();
    guido->denominator = ar->getDuration().getDenominator();
    guido->midiPitch = -1;
    if(has_tie)
        has_tie = false;
    if(has_stacatto)
        has_stacatto = false;
    score_infos.emplace_back(std::move(guido));
}

void ScoreMapper::Note(const ARMusicalObject *ev) {
    std::shared_ptr<GuidoInfos> notes(new GuidoInfos);
    notes->voiceNum = ev->getVoiceNum();
    const ARNote *ar= dynamic_cast<const ARNote *>(ev);
    notes->midiPitch = ar->getMidiPitch();
    notes->pitch = ar->getPitch();
    notes->detune = ar->getDetune();
    notes->duration = ar->getDuration();
    notes->numerator = ar->getDuration().getNumerator();
    notes->denominator = ar->getDuration().getDenominator();
    notes->intensity = intensity;
    if(has_tie) {
        notes->hasTie = has_tie;
        has_tie = false;
    }
    if(has_stacatto) {
        notes->hasStacatto = has_stacatto;
        has_stacatto = false;
    }
    score_infos.emplace_back(std::move(notes));
}

void ScoreMapper::IntensChange(const ARMusicalObject *ev) {
    const ARIntens * i = dynamic_cast<const ARIntens*>(ev);
    if (i) {
        if (i->getText() == "ppp")			intensity = 20;
        else if (i->getText() == "pp")		intensity = 30;
        else if (i->getText() == "p")		intensity = 50;
        else if (i->getText() == "mf")		intensity = 70;
        else if (i->getText() == "f")		intensity = 90;
        else if (i->getText() == "ff")		intensity = 110;
        else if (i->getText() == "fff")		intensity = 120;
    }
}

void ScoreMapper::AtPos(const ARMusicalObject *ev, EventType type) {
    if(fDone)
        return;
    switch (type) {
        case TimeUnwrap::kNote:
            Note(ev);
            break;
        case TimeUnwrap::kRest:
            Rest(ev);
            break;
        case TimeUnwrap::kFine:
            fDone = true;
            break;
        case TimeUnwrap::kStaccato:
            has_stacatto = true;
            break;
        case TimeUnwrap::kTie:
            has_tie = true;
            break;
        case TimeUnwrap::kIntens:
            IntensChange(ev);
            break;
        default:
            break;
    }
}

void ScoreMapper::Event(const ARMusicalObject * ev, EventType type) {}

void ScoreMapper::Clear() {
    score_infos.clear();
}

std::vector<std::shared_ptr<GuidoInfos>> ScoreMapper::getInfos() {
    return std::move(score_infos);
}

void ScoreMapper::printInformation(std::ostream &os) const {
    for(int i = 0; i<score_infos.size(); i++)
        os<<"scoreinfos size: "<<score_infos.size()<<" Voice: "<<score_infos.at(i)->voiceNum<<" Type: "<<score_infos.at(i)->type <<" MidiPitch: "<<score_infos.at(i)->midiPitch<<" Pitch: "<<score_infos.at(i)->pitch<<" Duration: "<<score_infos.at(i)->duration<<" Numerator: "<<score_infos.at(i)->numerator<<" Denominator: "<<score_infos.at(i)->denominator<<" hasTie: "<<score_infos.at(i)->hasTie<< " hasStacatto: "<<score_infos.at(i)->hasStacatto<<" Intensity: "<<score_infos.at(i)->intensity<<" Detune: "<<score_infos.at(i)->detune <<std::endl;
}

/*void ScoreMapper::getInformation(std::vector<std::unique_ptr<GuidoInfos> > _output) {
 
 }*/

static std::vector<std::shared_ptr<GuidoInfos>> getVoice(ARMusicalVoice * voice) {
    ScoreMapper mapper(voice);
    //voice->browse(mapper);
    voice->browse(mapper);
    return mapper.getInfos();
    //mapper.printInformation(os);
}
GuidoErrCode GuidoScoreMapper(const ARHandler ar, std::vector<std::vector<std::shared_ptr<GuidoInfos>>> &map) {
    if(ar ==0)
        return guidoErrInvalidHandle;
    ARMusic * arMusic = ar->armusic;
    if( arMusic == 0 )
        return guidoErrInvalidHandle;
    GuidoPos pos = arMusic->GetHeadPosition();
    ARMusicalVoice * voice = arMusic->GetNext(pos);
    if (voice) {
        while (voice) {
            map.emplace_back(getVoice(voice));
            if(pos)
                voice = arMusic->GetNext(pos);
            else
                break;
            
        }
        return guidoNoErr;
    }
    else
        return guidoErrActionFailed;
}