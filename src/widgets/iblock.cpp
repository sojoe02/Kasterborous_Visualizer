/*
 * =====================================================================================
 *
 *       Filename:  iblock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 01:13:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "iblock.h"
#include "utility.h"

IBlock::IBlock(int X, int Y, int W, int H, const char *L)
	:cumulativeIlvl(0),frequency(0),averageIlvl(0),Fl_Button(X,Y,W,H,L){
	}

void IBlock::draw(){

	//fl_color(Utility::thressToRainbow(intensityLvl));

	for(int i = 0; i<h();i++){
		fl_line(x(),y()+i,w(),h());
	}	
}

void IBlock::addIntensityLevel(double ilvl){
	frequency++;
	cumulativeIlvl += ilvl;
	averageIlvl = cumulativeIlvl / double(frequency);	
}
