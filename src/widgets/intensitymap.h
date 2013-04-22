#ifndef INTENSITYMAP_H
#define INTENSITYMAP_H

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"

#include "event.h"
#include "iblock.h"



class IntensityMap : public Fl_Widget
{
	public:
		IntensityMap(std::string msg,Event::simInfo info,int X, int Y, int W, int H,const char *L);
		void binEvent(Event::dataEvent devent);
		void calculateIlevel(double thresshold);
		void recursiveIlevelCalc(int Xpx, int Ypx, std::string key, Event::dataEvent event);
		void normalizeIntensityLevels(double maxValue, double minValue);
		void showMap();

		//find initial proccessing data
		void calculateMaxIntensity();
		double getMaxIntensity();
		void setMaxIntensity(double i);

		//inherited from Fl_Widget:
		void draw();

	private:
		std::list<Event::dataEvent> dataEvents;
		std::list<Event::dataEvent>::iterator dataItr;
		
		//std::unordered_map<std::string>::iterator iBlockItr;
		std::unordered_map<std::string, IBlock*> iBlocks; /*<! contains all iBlocks @see IBlock one for each pixel*resolution with key "x,y"  */
		std::unordered_set<std::string> visitedBlocks;

		std::list<double> normalizedIntensityLevels;	
		std::list<double>::iterator itIlevels;

		double maxIntensity; /*<! highest possible intensity level. */

		std::string msg;

		lua_State* L_State;
		Event::simInfo info;
		double xcf; /*<! conversion x px to area width*/
		double ycf; /*<! conversion y px to area height */

		double thresshold;
		int resolution;		

};	



#endif // INTENSITYMAP_H


