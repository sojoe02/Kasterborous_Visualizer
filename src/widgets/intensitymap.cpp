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
#include <math.h>

#include "intensitymap.h"
#include "utility.h"
#include "png++/png.hpp"



IntensityMap::IntensityMap(std::string msg,Event::simInfo info,int i, unsigned long long intensityPeriod
		,int X, int Y, int W, int H, const char *L)
	:msg(msg),info(info),mapID(i), intensityPeriod(intensityPeriod),maxIntensity(0),Fl_Widget(X,Y,W,H,L){

	//printf("%s\n", L);

	L_State = luaL_newstate();
	luaL_openlibs(L_State);
	//loadfile:
	if(luaL_loadfile(L_State, info.luaFileName) || lua_pcall(L_State,0,0,0)){
		printf("error : %s \n", lua_tostring(L_State, -1));
	}
	xcf = double(w()) / info.areaX;
	ycf = double(h()) / info.areaY;
	label(msg.c_str());

	resolution = 5;
}

void IntensityMap::draw(){

	std::unordered_map<std::string, IBlock>::iterator itr = iBlocks.begin();
	for(; itr !=iBlocks.end(); itr++){
		IBlock iblock = itr->second; 
		for(int i = 0; i < resolution;i++){
			fl_color(iblock.getColor());
			fl_line(iblock.getPosX(),iblock.getPosY()+i,
					iblock.getPosX()+resolution,iblock.getPosY()+i);
		}	
	}
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
}

/**
 * Visit all data events and calculate the origins intensity
 * level for each of the events. 
 * This is assumed to be maximum intensity level. The higest found will be saved on * the in the maxIntensity variable. 
 */
void IntensityMap::calculateMaxIntensity(){
	resolution = Utility::resolution;
	//initialize all the iblocks!
	int z = 0;
	for(int i = 0; i < w(); i+=resolution){
		for(int j = 0; j < h(); j+=resolution){
			z++;
			char buffer[40];
			sprintf(buffer, "%i,%i", i/resolution, j/resolution);
			IBlock iblock(i+x(), j+y());
			std::string key = buffer;
			iBlocks.insert(std::pair<std::string,IBlock>(key,iblock));
		}
	}
	printf("%i\n", z);
	maxIntensity = 0;




	for(dataItr = dataEvents.begin(); dataItr != dataEvents.end();dataItr++){

		Event::dataEvent event = *dataItr;

		lua_getglobal(L_State, "initAuton");
		lua_pushnumber(L_State, event.originX);
		lua_pushnumber(L_State, event.originY);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);

		if(lua_pcall(L_State,5,0,0)!=LUA_OK)
			printf("error on calling initAuton : %s\n,",
					lua_tostring(L_State,-1));


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

		Utility::incrementProgress(1);
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
void IntensityMap::calculateIlevel(double thress){

	thresshold = thress;
	printf("%f \n",thresshold);
	resolution = Utility::resolution;


	
	
	int  i;
	//Then do the recursive calls:
	for(i = 0,dataItr = dataEvents.begin()
			;dataItr != dataEvents.end()
			;dataItr++, i++){

		char buffer[40];
		Event::dataEvent event = *dataItr;
		lua_settop(L_State,0);

		lua_getglobal(L_State, "initAuton");
		lua_pushnumber(L_State, event.originX);
		lua_pushnumber(L_State, event.originY);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);

		if(lua_pcall(L_State,5,0,0)!=LUA_OK)
			printf("error on calling initAuton : %s\n,",
					lua_tostring(L_State,-1));

		sprintf(buffer,"%i,%i",int(event.originX*xcf/resolution), 
				int(event.originY*ycf/resolution));

		std::string new_key = buffer;
		visitedBlocks.insert(new_key);		
		std::string table = event.table;

		recursiveIlevelCalc(event.originX, event.originY, event.originX*xcf, event.originY*ycf
				, new_key,table);

		visitedBlocks.clear();

		if(i % 10 == 0)	
			Utility::incrementProgress(10);
	}

	//writeDynamicMaps();

	//dynamicsMap.clear();

	//--------------------------------------------

	printf("Calculating min and max values\n");

	std::unordered_map<std::string,IBlock>::iterator itr;

	double c_max = 0;
	double a_max = 0;
	double f_max = 0;
	double f_min = 1E+37;
	double a_min = 1E+37;
	double c_min = 1E+37;

	for(itr = iBlocks.begin(); itr !=iBlocks.end(); itr++){
		double a,c,f;
		(*itr).second.getBlockValues(c,f,a);
		if(c_max < c) c_max = c;
		if(a_max < a) a_max = a;
		if(f_max < f) f_max = f;
		if(c_min > c && c > 0) c_min = c;
		if(a_min > a && a > 0) a_min = a;
		if(f_min > f && f > 0) f_min = f;			
	}

	double values[] = {c_max,f_max,a_max,
		c_min,f_min,a_min};
	Utility::addMaxMinValues(values);
}

void IntensityMap::calculateDlevel(double thress){

		/*
	 * Initiate the Dynamic map containers:
	 */
	int dynamicAmount = (intensityPeriod / info.timeResolution)/0.01;
	unsigned long long mapActivation = mapID * intensityPeriod;


	//generate the dynamic map container:
	for(int i = 0; i < w(); i+=resolution){
		for(int j = 0; j < h(); j+=resolution){

			colorLevels colors;

			for(int k =0 ; k<dynamicAmount; k++){
				colors.push_back(0);
			}

			char buffer[40];
			sprintf(buffer, "%i,%i", i/resolution, j/resolution);
			std::string key = buffer;
			dynamicsMap.insert(std::pair<std::string,colorLevels>(key,colors));
		}
	}
	printf("number of dynamic maps are, %i, activation TMU is, %llu \n"
			, dynamicAmount, mapActivation);
	/* ---------------------------------------------- */
	Utility::setDProgressMinMax(1,dataEvents.size());
	char msgBuffer[50];

	int  i;
	//Then do the recursive calls:
	for(i = 0,dataItr = dataEvents.begin()
			;dataItr != dataEvents.end()
			;dataItr++, i++){

		char buffer[40];
		Event::dataEvent event = *dataItr;
		lua_settop(L_State,0);

		lua_getglobal(L_State, "initAuton");
		lua_pushnumber(L_State, event.originX);
		lua_pushnumber(L_State, event.originY);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);
		lua_pushnumber(L_State, 0);

		if(lua_pcall(L_State,5,0,0)!=LUA_OK)
			printf("error on calling initAuton : %s\n,",
					lua_tostring(L_State,-1));

		sprintf(buffer,"%i,%i",int(event.originX*xcf/resolution), 
				int(event.originY*ycf/resolution));

		std::string new_key = buffer;
		visitedBlocks.insert(new_key);		
		std::string table = event.table;

		recursiveDlevelCalc(0.05, event.activationTime, event.originX, event.originY, event.originX*xcf, event.originY*ycf
				, new_key,table);

		visitedBlocks.clear();

		if(i % 10 == 0){	
			sprintf(msgBuffer, "IntensityMap # %i, recursive calculations: " ,mapID);
			Utility::incrementDProgress(10,msgBuffer,1);
		}
	}
	writeDynamicMaps();
	dynamicsMap.clear();
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
void IntensityMap::recursiveIlevelCalc(double originX,double originY, int Xpx, int Ypx, std::string key, std::string table){
	lua_settop(L_State,0);

	lua_getglobal(L_State, "processFunction");
	lua_pushnumber(L_State, double(Xpx)/xcf);
	lua_pushnumber(L_State, double(Ypx)/ycf);
	lua_pushstring(L_State, table.c_str());

	if(lua_pcall(L_State,3,1,0)!=LUA_OK)
		printf("error on calling processfunction : %s\n,",
				lua_tostring(L_State,-1));

	double tmpI = lua_tonumber(L_State,-1);

	blockItr = iBlocks.find(key);

	if(blockItr != iBlocks.end()){
		(*blockItr).second.addIntensityLevel(tmpI);
	} //else printf("key doesn't exist, \t %s\n", key.c_str());

	if(tmpI > thresshold){
		std::string new_key;
		char buffer[40];
		if(Xpx+resolution <= w()){
			//Go East:
			sprintf(buffer,"%i,%i", (Xpx+resolution)/resolution, (Ypx)/resolution);
			new_key =buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveIlevelCalc(originX, originY, Xpx+resolution, Ypx, new_key, table);
			}
		}
		if(Xpx-resolution >=0){
			//Go West:
			sprintf(buffer,"%i,%i",(Xpx-resolution)/resolution,(Ypx)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveIlevelCalc(originX, originY,Xpx-resolution, Ypx, new_key, table);
			}
		}
		if(Ypx+resolution <= h()){
			//Go South:
			sprintf(buffer,"%i,%i",(Xpx)/resolution,(Ypx+resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveIlevelCalc(originX, originY, Xpx, Ypx+resolution, new_key,table);
			}
		}
		if(Ypx-resolution >=0 ){
			//Go North:
			sprintf(buffer, "%i,%i",(Xpx)/resolution,(Ypx-resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveIlevelCalc(originX, originY, Xpx, Ypx-resolution, new_key, table);
			}
		}
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
void IntensityMap::recursiveDlevelCalc(double eventDuration, unsigned long long activationTime,double originX,double originY, int Xpx, int Ypx, std::string key, std::string table){
	lua_settop(L_State,0);

	lua_getglobal(L_State, "processFunction");
	lua_pushnumber(L_State, double(Xpx)/xcf);
	lua_pushnumber(L_State, double(Ypx)/ycf);
	lua_pushstring(L_State, table.c_str());

	if(lua_pcall(L_State,3,1,0)!=LUA_OK)
		printf("error on calling processfunction : %s\n,",
				lua_tostring(L_State,-1));

	double tmpI = lua_tonumber(L_State,-1);

	/* Dynamics map updating:
	*/
	dynamicsMapItr = dynamicsMap.find(key); 

	if(dynamicsMapItr != dynamicsMap.end()){
		//int dynamicAmount = (intensityPeriod / info.timeResolution)/0.01;
		double distance = sqrt( pow((originX-Xpx/xcf), 2) + pow((originY-Ypx/ycf),2) );
		unsigned long long arrivalTMU = distance / (343.2 / info.timeResolution);

		int index = activationTime/(0.01*info.timeResolution) + arrivalTMU/(0.01*info.timeResolution);
		index = index/(mapID+1);
		//printf("activation time is %llu , arrivalTMU is %llu, distance %f, maps periodoffset: %llu\n", activationTime, arrivalTMU,distance,intensityPeriod*(mapID+1));
		//convert to greyscale:
		double tmpGrey = (tmpI /Utility::max_intensity * 255);

		//taking duration into account:
		int stretchOfTime = int (eventDuration / 0.01);
		for(int i = 0; i < stretchOfTime; i++){
			if( index+i < (*dynamicsMapItr).second.size() -1
					&& (*dynamicsMapItr).second.at(index + i) < tmpGrey){
				(*dynamicsMapItr).second.at(index+i) = int(tmpGrey);
				//printf("greyscale is, %i index is, %i, max index, %i\n", int(tmpGrey), index, stretchOfTime);
			}
		}

	}


	if(tmpI > thresshold){
		std::string new_key;
		char buffer[40];
		if(Xpx+resolution <= w()){
			//Go East:
			sprintf(buffer,"%i,%i", (Xpx+resolution)/resolution, (Ypx)/resolution);
			new_key =buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(eventDuration, activationTime,originX, originY, Xpx+resolution, Ypx, new_key, table);
			}
		}
		if(Xpx-resolution >=0){
			//Go West:
			sprintf(buffer,"%i,%i",(Xpx-resolution)/resolution,(Ypx)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(eventDuration, activationTime,originX, originY,Xpx-resolution, Ypx, new_key, table);
			}
		}
		if(Ypx+resolution <= h()){
			//Go South:
			sprintf(buffer,"%i,%i",(Xpx)/resolution,(Ypx+resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(eventDuration, activationTime,originX, originY, Xpx, Ypx+resolution, new_key,table);
			}
		}
		if(Ypx-resolution >=0 ){
			//Go North:
			sprintf(buffer, "%i,%i",(Xpx)/resolution,(Ypx-resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(eventDuration, activationTime,originX, originY, Xpx, Ypx-resolution, new_key, table);
			}
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

void IntensityMap::writeDynamicMaps(){

	int dynamicAmount = (intensityPeriod / info.timeResolution)/0.01;
	//write the dynamics to pictures on the HDD:
	char buffer[50];
	char msgBuffer[50];
	
	Utility::setDProgressMinMax(1,dynamicAmount);
	Utility::printmsg(msgBuffer);

	for(int k = 0; k<dynamicAmount; k++){
		png::image<png::rgb_pixel> image(w()/resolution,h()/resolution);
		for(int i = 0; i < w(); i+=resolution){
			for(int j = 0; j < h(); j+=resolution){					
				sprintf(buffer, "%i,%i", i/resolution, j/resolution);
				std::string key = buffer;
				dynamicsMapItr = dynamicsMap.find(key);
				unsigned char color = (*dynamicsMapItr).second.at(k);
				image[i/resolution][j/resolution] = png::rgb_pixel(192,color,color);
			}
		}
		if( k % 10 == 0){
			sprintf(msgBuffer, "IntensityMap # %i, is writing dynamic map: " ,mapID);
			Utility::incrementDProgress(10,msgBuffer,2);
		}
		char filename[50];
		sprintf(filename, "img/%d.png",k);//+((int)(intensityPeriod*0.01)*mapID));
		image.write(filename);
	}
}
