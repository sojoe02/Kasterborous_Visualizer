#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include <list>
#include <string>

#include "intensitymap.h"

class MapHandler{

	public : 
		mapHandler();

		/**
		 * Parse all the data and put the data events in a linked list.
		 * @param dataFile filename of the datafile containing all event data.
		 */
		void parseData(std::string dataFile);

		/**
		 * Put data into number of intensity map, one pr step.
		 */
		void binData(int steps);

		/**
		 * Do rekursive calculations on every 
		 */
		void calculateIntensityLevels();
		void normalizeData();

	private :
		Event::simInfo dataInfo;
		std::list<Event::dataEvent> dataEvents;
		std::list<IntensityMap> intensityMaps;


};


#endif // MAPHANDLER_H
