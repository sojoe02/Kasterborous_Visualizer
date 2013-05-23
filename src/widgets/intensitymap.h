#ifndef INTENSITYMAP_H
#define INTENSITYMAP_H

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"

#include "event.h"
#include "iblock.h"



class IntensityMap : public Fl_Widget
{
	public:
		IntensityMap(std::string luafile, std::string msg,Event::simInfo info,int i, unsigned long long intensityPeriod
				,int X, int Y, int W, int H,const char *L);
		void binEvent(Event::dataEvent devent);

		void calculateIlevel(double thress);
		void calculateDlevel(double thress);

		void recursiveIlevelCalc(double originX, double originY,int Xpx, int Ypx, std::string key, std::string table);
		void recursiveDlevelCalc(double propagationSpeed, double eventDuration, unsigned long long activationTime, double originX, double originY,int Xpx, int Ypx, std::string key, std::string table);

		void showMap();

		//find initial proccessing data
		void calculateMaxIntensity();
		double getMaxIntensity();
		void setMaxIntensity(double i);

		//inherited from Fl_Widget:
		void draw();
		//void hide();
		//void show(); 

	private:

		void writeDynamicMaps();

		std::list<Event::dataEvent> dataEvents;
		std::list<Event::dataEvent>::iterator dataItr;
		
		//std::unordered_map<std::string>::iterator iBlockItr;
		std::unordered_map<std::string, IBlock> iBlocks; /*<! contains all iBlocks @see IBlock one for each pixel*resolution with key "x,y"  */
		std::unordered_map<std::string,IBlock>::iterator blockItr;
		std::unordered_set<std::string> visitedBlocks;
		std::list<double> normalizedIntensityLevels;	
		std::list<double>::iterator itIlevels;

		//Setting the containers for the dynamic part of the map:
		//typedef uchar rgb[3];
		typedef std::vector<unsigned char> colorLevels;
		std::unordered_map<std::string, colorLevels>::iterator dynamicsMapItr;
		std::unordered_map<std::string, colorLevels> dynamicsMap;

		double maxIntensity; /*<! highest possible intensity level. */

		std::string msg;
		std::string luafile;

		lua_State* L_State;
		Event::simInfo info;
		double xcf; /*<! conversion x px to area width*/
		double ycf; /*<! conversion y px to area height */

		double thresshold;
		int resolution;	
		unsigned long long intensityPeriod; /* <! Amount of TMUs this covers */
		int mapID;	/* <! What timeperiod this Intensity Map 0 is the first N is the last. */

};	



#endif // INTENSITYMAP_H


