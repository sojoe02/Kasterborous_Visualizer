#ifndef INTENSITYMAP_H
#define INTENSITYMAP_H

#include <list>
#include "event.h"

class IntensityMap{
	public:
		void IntensityMap();
		void ~IntensityMap();
		void binEvent(Event::dataEvent devent);
		void calculateIntensityLevels();
		void normalizeIntensityLevels(double maxValue, double minValue);
		void showMap();

	private:
		std::list<Event::dataEvent> dataEvents;
		std::list<Event::dataEvent>::iterator itDataEvents;
		
		std::list<double> intensityLevels;
		std::list<double> normalizedIntensityLevels;	
		std::list<double>::iterator itILevels;
};	



#endif // INTENSITYMAP_H


