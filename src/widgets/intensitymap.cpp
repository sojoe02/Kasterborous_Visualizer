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
#include "utility.h"



IntensityMap::IntensityMap(std::string msg,Event::simInfo info,int X, int Y, int W, int H, const char *L)
	:msg(msg),info(info),maxIntensity(0),Fl_Widget(X,Y,W,H,L){

	//printf("%s\n", L);

	L_State = luaL_newstate();
	luaL_openlibs(L_State);
	//loadfile:
	if(luaL_loadfile(L_State, info.luaFileName) || lua_pcall(L_State,0,0,0)){
		printf("error : %s \n", lua_tostring(L_State, -1));
	}
	xcf = double(w()) / info.areaX;
	ycf = double(h()) / info.areaY;
	//printf("%f, %f\n", xcf,ycf);
	label(msg.c_str());

	resolution = 5;
}

void IntensityMap::draw(){

	//Fl_Widget::draw();
	fl_color(FL_BLACK);
	fl_rect(x(),y(),w(),h());

	if(Utility::location){
		fl_color(FL_BLACK);
		//first draw the positions of all active Autons:
		for(itDataEvents = dataEvents.begin(); itDataEvents != dataEvents.end(); itDataEvents++){

			int pxx = int((*itDataEvents).originX * xcf); 
			int pxy = int((*itDataEvents).originY * ycf);

			int size = 15;
			for(int i = -size; i < size; i++){
				if(pxx+x()+i < w()+x() && pxx+x()+i > x() && pxy+y()-i < y()+h()
						&& pxy+y()-i > y())
					fl_point(pxx+x()+i, pxy+y()-i);

				if(pxy+y()+i < h()+y() && pxy+y()+i > y() && pxx+x()+i < x()+w())
					fl_point(pxx+x()+i, pxy+y()+i);
			}
		}
	}

	if(Utility::sectors){
		fl_color(FL_WHITE);
		for(int i = 0; i < w(); i+=resolution){
			fl_line(x()+i,y(),x()+i,y()+h());
		}
		for(int j = 0; j < h(); j+=resolution){
			fl_line(x(),y()+j, x()+w(), y()+j);
		}


	}
	//fl_draw(msg.c_str(),msg.length(),x()+w()/2,y()+h());
	//	printf("%s",I.c_str());
}

/**
 * Visit all data events and calculate the origins intensity
 * level for each of the events. 
 * This is assumed to be maximum intensity level. The higest found will be saved on * the in the maxIntensity variable. 
 */
void IntensityMap::calculateMaxIntensity(){

	//initialize the intensityLevel map:
	//for(int i = 0; i < w(); i++){
	/*for(int j = 0; j < h(); j++){
	  char buffer[50];
	  sprintf(buffer, "%i,%i", i, j);
	  std::string key = buffer;
	//intensityLevels.insert(std::pair<std::string,double>(key,0));
	intensityLevels[key] = 0;
	}*/
	//}

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

void IntensityMap::setMaxIntensity(double i){
	maxIntensity = i;
}

/**
 * Recursive algorithm which calculates all intensitylevels from origin until a set thresshold.
 * @param thresshold, minimum Intensity level to be acceptable
 * @param resolution, number of pixels squared that each intensity level covers.
 */
void IntensityMap::calculateIlevel(double thresshold){
	thresshold = thresshold;
	resolution = Utility::resolution;
	//initialize the sectors:i
	for(int i = 0; i < w(); i+= resolution){
		for(int j = 0; j < h() + 0.5; j+=resolution){
			char buffer[40];
			sprintf(buffer, "%i,%i", i, j);
			std::string key = buffer;
			//intensityLevels.insert(std::pair<std::string,double>(key,0));
			intensityLevels[key] = 0;	
		}
	}
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

		char buffer[40];

		int bx = int(event.originX/resolution +0.5);
		int by = int(event.originX/resolution +0.5);
		//find the block:
		sprintf(buffer,"%i,%i", bx, by);
		std::string key = buffer;
		//insert it into the intensitylvl
		intensityLevels[key] += tmpI; 		
	}

	//Then do the recursive calls:
}


/**
 * Add an event to for this IntensityMap.
 * @param devent data event to be added.
 */
void IntensityMap::binEvent(Event::dataEvent devent){
	dataEvents.push_back(devent);	
}

/**
 * 
 */
void IntensityMap::normalizeIntensityLevels(double maxValue, double minValue){
	/*  for(itIlevels = intensityLevels.begin(); itIlevels != intensityLevels.end(); ++itIlevels){
	    double cLevel = *itIlevels;
	    double nLevel = (maxValue - minValue) / (cLevel - minValue);
	    normalizedIntensityLevels.push_back(nLevel);
	    } */
}
