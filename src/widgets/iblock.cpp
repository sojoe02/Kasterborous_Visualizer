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

IBlock::IBlock(int posx, int posy)
	:cumulativeIlvl(0),frequency(0),averageIlvl(0),
	x(posx),y(posy){
	}

Fl_Color IBlock::getColor(){
	int state = Utility::map_StateMachine();
	if(state == Utility::map_Cumulative){
		return Utility::getCumulativeColor(cumulativeIlvl);
	}else if(state == Utility::map_Freq){
		return Utility::getFreqColor(frequency);
	}else if(state == Utility::map_Average){
		return Utility::getAvgColor(averageIlvl);
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
	if(cumulativeIlvl>0) c= cumulativeIlvl;
	if(frequency>0) f = frequency;
	if(averageIlvl>0) a = averageIlvl;
}
