#ifndef INTENSITYMAP_H
#define INTENSITYMAP_H

#include <list>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "event.h"

#include <string>

#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"


class IntensityMap : public Fl_Widget
{
	public:
		IntensityMap(Event::simInfo info,int X, int Y, int W, int H, std::string I,const char *L);
		void binEvent(Event::dataEvent devent);
		void calculateIntensityLevels();
		void normalizeIntensityLevels(double maxValue, double minValue);
		void showMap();

		//find initial proccessing data
		void calculateMaxIntensity();
		double getMaxIntensity();

		//inherited from Fl_Widget:
		void draw();

	private:
		std::list<Event::dataEvent> dataEvents;
		std::list<Event::dataEvent>::iterator itDataEvents;
		
		std::list<double> intensityLevels;
		std::list<double> normalizedIntensityLevels;	
		std::list<double>::iterator itIlevels;

		double maxIntensity;
		std::string I;

		lua_State* L_State;
		Event::simInfo info;
		double xcf; /*<! conversion x px to area width*/
		double ycf; /*<! conversion y px to area height */
		

};	



#endif // INTENSITYMAP_H


