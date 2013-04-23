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
	int state = Utility::map_StateMachine();	

	if(state = Utility::map_Cumulative){
		fl_color(Utility::getCumulativeColor
				(cumulativeIlvl));
	}else if(state = Utility::map_Freq){
		fl_color(Utility::getFreqColor(frequency));
	}else if(state = Utility::map_Average){
		fl_color(Utility::getAvgColor(averageIlvl));
	}

	for(int i = 0; i<h();i++){
		fl_line(x(),y()+i,w(),h());
	}	
}

void IBlock::addIntensityLevel(double ilvl){
	frequency++;
	cumulativeIlvl += ilvl;
	averageIlvl = cumulativeIlvl / double(frequency);	
}

/**
 * Retrieve block values in an array;
 */
void IBlock::getBlockValues(double &c, double &f, double &a){
	c= cumulativeIlvl;
	f= frequency;
	a= averageIlvl;
}
