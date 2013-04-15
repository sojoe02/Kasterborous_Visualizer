#ifndef INTENSITYMAP_H
#define INTENSITYMAP_H

#include <list>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "event.h"

#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"


class IntensityMap : public Fl_Widget
{
	public:
		IntensityMap(const char* filename,int X, int Y, int W, int H, const char *L);
		void binEvent(Event::dataEvent devent);
		void calculateIntensityLevels();
		void normalizeIntensityLevels(double maxValue, double minValue);
		void showMap();

		//inherited from Fl_Widget:
		void draw();

	private:
		std::list<Event::dataEvent> dataEvents;
		std::list<Event::dataEvent>::iterator itDataEvents;
		
		std::list<double> intensityLevels;
		std::list<double> normalizedIntensityLevels;	
		std::list<double>::iterator itIlevels;
		lua_State* L_State;

};	



#endif // INTENSITYMAP_H


