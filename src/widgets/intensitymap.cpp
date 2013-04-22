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

	if(Utility::show_SectorGrid){
		fl_color(FL_WHITE);
		for(int i = 0; i < w(); i+=resolution){
			fl_line(x()+i,y(),x()+i,y()+h());
		}
		for(int j = 0; j < h(); j+=resolution){
			fl_line(x(),y()+j, x()+w(), y()+j);
		}
	}


	if(Utility::show_Location){
		fl_color(FL_BLACK);
		//first draw the positions of all active Autons:
		for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){

			int pxx = int((*dataItr).originX * xcf); 
			int pxy = int((*dataItr).originY * ycf);

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

	//fl_draw(msg.c_str(),msg.length(),x()+w()/2,y()+h());
	//	printf("%s",I.c_str());
}

/**
 * Visit all data events and calculate the origins intensity
 * level for each of the events. 
 * This is assumed to be maximum intensity level. The higest found will be saved on * the in the maxIntensity variable. 
 */
void IntensityMap::calculateMaxIntensity(){
	//initialize all the iblocks!
	for(int i = 0; i < w()+resolution; i+=resolution){
		for(int j = 0; j < h()+resolution; j+=resolution){
			char buffer[40];
			sprintf(buffer, "%i,%i", i, j);
			IBlock *iblock = new IBlock(i+x(),j+y(),resolution,resolution,"");
			iblock->hide();
			std::string key = buffer;
			//iBlocks.insert(std::pair<std::string,IBlock>(key,iblock));
			iBlocks[key] = iblock;	
		}
	}
	maxIntensity = 0;

	for(dataItr = dataEvents.begin(); dataItr != dataEvents.end()
			;dataItr++){
		Event::dataEvent event = *dataItr;
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

/**
 * Set maximum intensity level reached for each event origin.
 * @param i, maximum intensity level
 */
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

	//Then do the recursive calls:
	for(dataItr = dataEvents.begin(); dataItr != dataEvents.end()
			;dataItr++){
		char buffer[40];

		Event::dataEvent event = *dataItr;
		sprintf(buffer,"%i,%i",int(event.originX*xcf), int(event.originY*ycf));
		std::string new_key = buffer;
		visitedBlocks.insert(msg);

		recursiveIlevelCalc(event.originX*xcf,event.originY*ycf,new_key,
				*dataItr);
		visitedBlocks.clear();
	}
}


/**
 * Recursive call to calculate all intensitylvls above thresshold.
 * This function will visist all unvisited neighboring intensity blocks
 * calculate the intensity level for the event in question at intensityblocks location and
 * add that intensity level to the block.
 * @param Xpx, x pixel value.
 * @param Ypx, y pixel value.
 * @param key, hash map key which is "Xpx,Ypx" string.
 * @param event, the data event in question.
 */
void IntensityMap::recursiveIlevelCalc(int Xpx, int Ypx, std::string key, Event::dataEvent event){

	lua_getglobal(L_State, "processFunction");
	lua_pushnumber(L_State, double(Xpx)/xcf);
	lua_pushnumber(L_State, double(Ypx)/ycf);
	lua_pushstring(L_State, event.table);

	if(lua_pcall(L_State,3,1,0)!=LUA_OK)
		printf("error on calling processfunction : %s\n,",
				lua_tostring(L_State,-1));

	double tmpI = lua_tonumber(L_State,-1);

	iBlocks.find(key)->second->addIntensityLevel(tmpI);
	
	if(tmpI > thresshold){
		//Go East:
		char buffer[40];
		sprintf(buffer,"%i,%i", Xpx+resolution, Ypx);
		std::string new_key = buffer;
		if(visitedBlocks.find(new_key) == visitedBlocks.end() && (Xpx+resolution) < w()){
			visitedBlocks.insert(new_key);
			recursiveIlevelCalc(Xpx+resolution, Ypx, new_key,event);
		}
		//Go West:
		sprintf(buffer,"%i,%i",Xpx-resolution,Ypx);
		new_key = buffer;
		if(visitedBlocks.find(new_key) == visitedBlocks.end() && (Xpx-resolution) > 0){
			visitedBlocks.insert(new_key);
			recursiveIlevelCalc(Xpx-resolution, Ypx, new_key,event);
		}
		//Go South:
		sprintf(buffer,"%i,%i",Xpx,Ypx+resolution);
		new_key = buffer;
		if(visitedBlocks.find(new_key) == visitedBlocks.end() && (Ypx+resolution) < h()){
			visitedBlocks.insert(new_key);
			recursiveIlevelCalc(Xpx, Ypx-resolution, new_key,event);
		}
		//Go North:
		sprintf(buffer, "%i,%i",Xpx,Ypx-resolution);
		new_key = buffer;
		if(visitedBlocks.find(new_key) == visitedBlocks.end() && (Ypx-resolution) > 0){
			visitedBlocks.insert(new_key);
			recursiveIlevelCalc(Xpx, Ypx+resolution, new_key,event);
		}
	}
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
