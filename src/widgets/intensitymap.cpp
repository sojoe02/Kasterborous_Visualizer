/*
 * =====================================================================================
 *
 *       Filename:  intensitymap.cpp
 *
 *    Description:  
 *
 *        Created:  04/10/2013 12:52:52 PM
 *
 *         Author:  Soeren V. Jorgensen (SVJ), sojoe02@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>

#include "intensitymap.h"



IntensityMap::IntensityMap(const char* filename,int X, int Y, int W, int H, const char *L)
	:Fl_Widget(X,Y,W,H,L)
{

	L_State = luaL_newstate();
	luaL_openlibs(L_State);
	//loadfile:
	if(luaL_loadfile(L_State, filename) || lua_pcall(L_State,0,0,0)){
		printf("error : %s \n", lua_tostring(L_State, -1));
	}	

}

void IntensityMap::draw(){

}

/**
 * 
 */
void IntensityMap::binEvent(Event::dataEvent devent){
	dataEvents.push_back(devent);	
}

/**
 * 
 */
void IntensityMap::normalizeIntensityLevels(double maxValue, double minValue){
	for(itIlevels = intensityLevels.begin(); itIlevels != intensityLevels.end(); ++itIlevels){
		double cLevel = *itIlevels;
		double nLevel = (maxValue - minValue) / (cLevel - minValue);
		normalizedIntensityLevels.push_back(nLevel);
	}
}
