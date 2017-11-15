/*
	GUIDO Library
	Copyright (C) 2014	Thomas Coffy / IRCAM

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License (Version 2), 
	as published by the Free Software Foundation.
	A copy of the license can be found online at www.gnu.org/licenses.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.
*/

#include <iostream>
#include <sstream>
#include <string>

#include "openFrameworksDevice.h"
#include "openFrameworksSystem.h"
#include "GuidoComponent.h"
#include "GUIDOParse.h"
#include "GUIDOScoreMap.h"
#include "GUIDOEngine.h"
#include "GuidoMapCollector.h"
//#include "GuidoMapUtils.hpp"
using namespace std;


static openFrameworksSystem gSystem;
static openFrameworksDevice gDevice(&gSystem);

openFrameworksDevice* GuidoComponent::getDevice() {
	return &gDevice;
}

GuidoComponent::GuidoComponent () 
	: fARHandler(0), fGRHandler(0), fPage(0), fResizeToMusic(false), fScoreColor(0,0,0)
{
	GuidoGetDefaultLayoutSettings (&fSettings);
}

GuidoComponent::~GuidoComponent()
{
	if (fGRHandler) GuidoFreeGR (fGRHandler);
	if (fARHandler) GuidoFreeAR (fARHandler);
}


//==============================================================================
GuidoErrCode GuidoComponent::GuidoInit (const char* textfont, const char* guidofont)
{
	GuidoInitDesc desc;
	desc.graphicDevice = &gDevice;
	desc.musicFont	= guidofont;
	desc.textFont	= textfont;
	return  ::GuidoInit(&desc);
}

//==============================================================================
GuidoErrCode GuidoComponent::setGMNFile (const char* file )
{
	ARHandler arh;
	GuidoErrCode err = GuidoParseFile (file, &arh);
	if (err != guidoNoErr) return err;
	
	err = setARHandler (arh);
	if (err != guidoNoErr) GuidoFreeAR (arh);
	else {
		//File f(file);
		//fGMNCode = f.loadFileAsString ();
		ifstream outfile;
		outfile.open (file);
		outfile >> fGMNCode;
		outfile.close();
		fGMNFile = file;
	}
	return err;
}

//==============================================================================
GuidoErrCode GuidoComponent::setGMNCode (const char* gmnCode )
{
	ARHandler arh;

	GuidoParser* parser = GuidoOpenParser();
	arh = GuidoString2AR(parser, gmnCode);
	int l, c;
	const char *errstr;
	GuidoErrCode err = GuidoParserGetErrorCode(parser, l, c, &errstr);

	if (err != guidoNoErr) GuidoFreeAR (arh);
	else {
		fGMNCode = gmnCode;
		err = setARHandler (arh);
	}
	GuidoCloseParser(parser);
	return err;
}

//==============================================================================
GuidoErrCode GuidoComponent::setARHandler(ARHandler ar)
{
	GRHandler grh;
	GuidoErrCode err = GuidoAR2GR (ar, &fSettings, &grh);
	if (err == guidoNoErr) {
		if (fGRHandler) GuidoFreeGR (fGRHandler);
		fGRHandler = grh;
		fARHandler = ar;
		fPage = 1;
		if (fResizeToMusic) GuidoResizePageToMusic (fGRHandler);
		//draw();
	}
	else GuidoFreeAR (ar);
	return err;
}

//==============================================================================
void GuidoComponent::setScoreColor(const ofColor& color)
{
	if (fScoreColor != color) {
		fScoreColor = color;
		//draw();
	}
}

//==============================================================================
void GuidoComponent::setGuidoLayoutSettings(const GuidoLayoutSettings& layoutSettings)
{
	fSettings = layoutSettings;
	if (fGRHandler) {
		GuidoUpdateGR (fGRHandler, &fSettings);
		//draw();
	}
}

//==============================================================================
void GuidoComponent::setResizePageToMusic(bool isOn)
{
	if (fResizeToMusic != isOn) {
		fResizeToMusic = isOn;
		setARHandler (fARHandler);
	}
}

//==============================================================================
int GuidoComponent::pageCount() const
{
	return fGRHandler ? GuidoGetPageCount (fGRHandler) : 0;
}

//==============================================================================
int GuidoComponent::voiceCount() const
{
	return fARHandler ? GuidoCountVoices (fARHandler) : 0;
}

void GuidoComponent::setWidth(int w) {
	width = w;
}

void GuidoComponent::setHeight(int h) {
	height = h;
}

int GuidoComponent::getWidth() {
	return width;
}

int GuidoComponent::getHeight() {
	return height;
}

void GuidoComponent::cursor(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    path.clear();
    path.newSubPath();
    path.moveTo(p1.x, p1.y);
    path.lineTo(p2.x, p2.y);
    path.lineTo(p3.x, p3.y);
    path.lineTo(p1.x, p1.y);
    path.setFilled(true);
    path.setFillColor(ofColor::red);
    path.close();
   // path.draw();
    
}

void GuidoComponent::moveCursor(glm::vec2 p1) {
    path.moveTo(p1.x, p1.y);
}

void GuidoComponent::cursorDraw() {
    path.draw();
}

std::vector<std::pair<float, float>> GuidoComponent::computeCoordinates() {
    double dtemp;
    GuidoPageFormat pf;
    std::vector<float> temp;
    std::vector<float> ycoords;
    std::vector<std::pair<float, float>> coords;
    float x,y;
    int counter = 0;
    GuidoGetPageFormat(fGRHandler, 1, &pf);
    cout<<"GC: width:"<<width<<", height:"<<height<<", pf.w:"<<pf.width<<", pf.h:"<<pf.height<<endl;
   /* if(!fGRHandler) {
        std::cerr<<"noGrHanderl"<<std::endl;
        return;
    }*/
    guido::GuidoMapCollector mapCol(fGRHandler, kGuidoSystemSlice);
    //GuidoBarMap barMap(fGRHandler, kGuidoBar);
    int n = GuidoGetPageCount(fGRHandler);
    for(int page = 1; page<=n; page++) {
       // barMap.process(page, width, height,  &_outmap);
        mapCol.process(page, pf.width, pf.height, &_outmap_2);
    }
    //outmap[0].second.GetYPos(float *inYPos)
    
    for(int i = 0; i<_outmap_2.size(); i++) {
        //temp.emplace_back(_outmap_2.at(i).second.left);
        //temp.emplace_back(_outmap_2.at(i).second.right);
        _outmap_2.at(i).second.GetXPos(&x);
        _outmap_2.at(i).second.GetYPos(&y);
        temp.emplace_back(x);
        ycoords.emplace_back(y);
    }
    cout<<"ycoords size:"<<ycoords.size()<<", tempSize:"<<temp.size()<<endl;
    for(int i = 0; i<temp.size()-1; i++) {
       // if(i > 0 && i%2==0)
         //   counter++;
        if(temp.at(i+1)> temp.at(i)) {
            dtemp = temp.at(i+1) - temp.at(i);
            dtemp /= 4;
        }
        coords.emplace_back(std::make_pair(temp.at(i), ycoords.at(i)));
        coords.emplace_back(std::make_pair(temp.at(i)+dtemp, ycoords.at(i)));
        coords.emplace_back(std::make_pair(temp.at(i) + (dtemp*2), ycoords.at(i)));
        coords.emplace_back(std::make_pair(temp.at(i) + (dtemp*3), ycoords.at(i)));
    }
    coords.emplace_back(std::make_pair(temp.at(temp.size()-1), ycoords.at(ycoords.size()-1)));
    _outmap.clear();
    _outmap_2.clear();
    return coords;
}

//==============================================================================
void GuidoComponent::draw (int x, int y, int w, int h)
{
	if (!fGRHandler) return;

	ofSetColor(fScoreColor);
	GuidoOnDrawDesc desc;
	desc.handle = fGRHandler;
	desc.hdc = &gDevice;
	desc.page = fPage;
	desc.scrollx = 0;
	desc.scrolly = 0;
	desc.isprint = false;
	desc.updateRegion.left = x;
	desc.updateRegion.top = y;
	desc.updateRegion.right = w;
	desc.updateRegion.bottom = h;
	desc.updateRegion.erase = true;
	desc.sizex = w;
	desc.sizey = h;

	//GuidoUpdateGR(fGRHandler, &fSettings);
	GuidoErrCode err = GuidoOnDraw (&desc);
	if (err != guidoNoErr)
		cerr << "error while painting: " << GuidoGetErrorString(err) << endl;
}

