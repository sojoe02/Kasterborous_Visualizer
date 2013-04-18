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



IntensityMap::IntensityMap(Event::simInfo info,int X, int Y, int W, int H, std::string I, const char *L)
	:info(info),maxIntensity(0), I(I),Fl_Widget(X,Y,W,H,L)
{

	L_State = luaL_newstate();
	luaL_openlibs(L_State);
	//loadfile:
	if(luaL_loadfile(L_State, info.luaFileName) || lua_pcall(L_State,0,0,0)){
		printf("error : %s \n", lua_tostring(L_State, -1));
	}

	xcf = w() / info.areaX;
	ycf = h() / info.areaY;
	

}

void IntensityMap::draw(){
	fl_color(FL_BLACK);
	//first draw the positions of all active Autons:
	for(itDataEvents = dataEvents.begin(); itDataEvents != dataEvents.end(); itDataEvents++){

		int pxx = int((*itDataEvents).originX * xcf); 
		int pxy = int((*itDataEvents).originY * ycf);
		
		fl_rect(pxx+x(),pxy+y(), 40,40);
	}
	//fl_draw(I.c_str(),x()/2+550,y());
//	printf("%s",I.c_str());
}

/**
 * Visit all data events and 
 */
void IntensityMap::calculateMaxIntensity(){

	maxIntensity = 0;

	for(itDataEvents = dataEvents.begin(); itDataEvents != dataEvents.end()
			;itDataEvents++){
		Event::dataEvent event = *itDataEvents;
		lua_getglobal(L_State, "processFunction");

		lua_pushnumber(L_State, event.originX);
		lua_pushnumber(L_State, event.originY);
		lua_pushstring(L_State, event.table);

		if(lua_pcall(L_State,3,1,0)!=LUA_OK)
			printf("error on calling processfunction : %s\n,",
					lua_tostring(L_State,-1));

		double tmpI = lua_tonumber(L_State,-1);

		if(tmpI > maxIntensity)
			maxIntensity = tmpI;
		//printf("max I is %f\n", tmpI);
	}
}

/**
 * Get the maximum intensity level
 * @see IntensityMap::getMaxIntensity
 * @return maximum intensity level
 */
double IntensityMap::getMaxIntensity(){
	return maxIntensity;
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
