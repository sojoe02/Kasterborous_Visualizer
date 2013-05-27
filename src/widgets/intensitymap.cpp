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
#include "physics/phys.h"




IntensityMap::IntensityMap(std::string luafile, std::string msg,Event::simInfo info,int i, unsigned long long intensityPeriod
		,int X, int Y, int W, int H, const char *L)
	:luafile(luafile),msg(msg),info(info),mapID(i), intensityPeriod(intensityPeriod),maxIntensity(0),Fl_Widget(X,Y,W,H,L){

	//printf("%s\n", L);

	L_State = luaL_newstate();
	luaL_openlibs(L_State);

	lua_register(L_State, "l_getMersenneFloat", l_getMersenneFloat);
	lua_register(L_State, "l_getMersenneInteger", l_getMersenneInteger);
	lua_register(L_State, "l_debug", l_debug);

	//loadfile:
	if(luaL_loadfile(L_State, luafile.c_str()) || lua_pcall(L_State,0,0,0)){
		printf("error : %s \n", lua_tostring(L_State, -1));
	}
	xcf = double(w()) / info.areaX;
	ycf = double(h()) / info.areaY;
	label(msg.c_str());

	//resolution = 5;
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
		fl_color(FL_BLACK);
		for(int i = 0; i < w(); i+=resolution){
			fl_line(x()+i,y(),x()+i,y()+h());
		}
		for(int j = 0; j < h(); j+=resolution){
			fl_line(x(),y()+j, x()+w(), y()+j);
		}
	}
	if(Utility::show_Location){
		char buffer[20];
		//first draw the positions of all active Autons:
		for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){
			fl_color(FL_WHITE);


			sprintf(buffer,"%d", ((*dataItr).originID));

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
			fl_color(FL_BLACK);
			fl_draw(buffer, pxx+x()-10,pxy+y()-10);

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

	writeDynamicMaps();

	dynamicsMap.clear();

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
	resolution = Utility::resolution;
	thresshold = thress;	
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
	Utility::setDProgressMinMax(1,(int)dataEvents.size());
	printf("dataEvents size %i \n", (int)dataEvents.size());
	char msgBuffer[150];

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

		recursiveDlevelCalc(event.propagationSpeed, event.duration, event.activationTime, event.originX, event.originY, event.originX*xcf, event.originY*ycf, buffer, event.table);

		visitedBlocks.clear();

		if(i % 10 == 0){	
			sprintf(msgBuffer, "IntensityMap # %i, recursive calculations: " ,mapID);
			Utility::incrementDProgress(10,msgBuffer,1);
		}
	}
	//writeDynamicMaps();
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
				recursiveIlevelCalc(originX, originY, Xpx-resolution, Ypx, new_key, table);
			}
		}
		if(Ypx+resolution <= h()){
			//Go South:
			sprintf(buffer,"%i,%i",(Xpx)/resolution,(Ypx+resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveIlevelCalc(originX, originY, Xpx, Ypx+resolution, new_key, table);
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
void IntensityMap::recursiveDlevelCalc(double propagationSpeed, double eventDuration, unsigned long long activationTime,double originX,double originY, int Xpx, int Ypx, const char* key, const char* table){

	lua_settop(L_State,0);

	lua_getglobal(L_State, "processFunction");
	lua_pushnumber(L_State, double(Xpx)/xcf);
	lua_pushnumber(L_State, double(Ypx)/ycf);
	lua_pushstring(L_State, table);

	if(lua_pcall(L_State,3,1,0)!=LUA_OK)
		printf("error on calling processfunction : %s\n,",
				lua_tostring(L_State,-1));

	double tmpI = lua_tonumber(L_State,-1); 

	/* Dynamics map updating:
	*/
	//std::unordered_map<std::string, colorLevels>::iterator dynamicsMapItr;

	dynamicsMapItr = dynamicsMap.find(key); 

	if(dynamicsMapItr != dynamicsMap.end()){
		//int dynamicAmount = (intensityPeriod / info.timeResolution)/0.01;
		//
		//double distance = 40;
		double distance = sqrt( pow((originX-Xpx/xcf), 2) + pow((originY-Ypx/ycf),2) );

		unsigned long long arrivalTMU = distance / (propagationSpeed / info.timeResolution);

		int index = (activationTime - intensityPeriod * mapID)/(0.01*info.timeResolution) + arrivalTMU/(0.01*info.timeResolution);
		//index = index/(mapID+1);
		//printf("activation time is %llu , arrivalTMU is %llu, distance %f, maps periodoffset: %llu\n", activationTime, arrivalTMU,distance,intensityPeriod*(mapID+1));
		//convert to greyscale:
		double tmpGrey = (tmpI /Utility::max_intensity * 255);
		int stretchOfTime = int(eventDuration / 10);
		if(stretchOfTime == 0) stretchOfTime = 1;

		//taking duration into account:
		for(int i = 0; i < stretchOfTime; i++){		
			//printf("greyscale is, %i index is, %i, max index, %i\n", int(tmpGrey), stretchOfTime, (int)eventDuration);
			if( index+i < (*dynamicsMapItr).second.size() -1
					&&(*dynamicsMapItr).second.at(index + i) < tmpGrey){

				(*dynamicsMapItr).second.at(index+i) = int(tmpGrey);

			}
		}

	} 

	if(tmpI > thresshold){
		std::string new_key;
		//std::string new_table = "";
		//new_table = table;
		char buffer[40];

		if(Xpx+resolution <= w()){
			//Go East:
			sprintf(buffer,"%i,%i", (Xpx+resolution)/resolution, (Ypx)/resolution);
			new_key =buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(propagationSpeed, eventDuration, activationTime, originX, originY, Xpx+resolution, Ypx, buffer, table);
			}
		}
		if(Xpx-resolution >=0){
			//Go West:
			sprintf(buffer,"%i,%i",(Xpx-resolution)/resolution,(Ypx)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(propagationSpeed, eventDuration, activationTime, originX, originY,Xpx-resolution, Ypx, buffer, table);
			}
		}
		if(Ypx+resolution <= h()){
			//Go South:
			sprintf(buffer,"%i,%i",(Xpx)/resolution,(Ypx+resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(propagationSpeed, eventDuration, activationTime, originX, originY, Xpx, Ypx+resolution, buffer, table);
			}
		}
		if(Ypx-resolution >=0 ){
			//Go North:
			sprintf(buffer, "%i,%i",(Xpx)/resolution,(Ypx-resolution)/resolution);
			new_key = buffer;
			if(visitedBlocks.find(new_key) == visitedBlocks.end() ){
				visitedBlocks.insert(new_key);
				recursiveDlevelCalc(propagationSpeed, eventDuration, activationTime, originX, originY, Xpx, Ypx-resolution, buffer, table);
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
				image[j/resolution][i/resolution] = png::rgb_pixel(color,color,color);
			}
		}
		if( k % 10 == 0){
			sprintf(msgBuffer, "IntensityMap # %i, is writing dynamic map: " ,mapID);
			Utility::incrementDProgress(10,msgBuffer,2);
		}
		char filename[50];
		sprintf(filename, "img/%d.png",k+1);//+((int)(intensityPeriod*0.01)*mapID));
		image.write(filename);
	}
}

int IntensityMap::l_debug(lua_State *L){
	//Utility::printmsg(lua_tostring(L,-1));
	//printf(lua_tostring(L,-1));
	return 0;
}

int IntensityMap::l_getMersenneFloat(lua_State *L){
	double low = lua_tonumber(L,-2);
	double high = lua_tonumber(L, -1);

	double number = Phys::getMersenneFloat(low,high);

	lua_pushnumber(L,number);
	return 1;
}

int IntensityMap::l_getMersenneInteger(lua_State *L){
	uint64_t low = lua_tonumber(L,-2);
	uint64_t high = lua_tonumber(L, -1);

	uint64_t number = Phys::getMersenneInteger(low,high);
	lua_pushnumber(L,number);
	return 1;
}
