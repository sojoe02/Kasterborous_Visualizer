#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include <list>
#include <vector>
#include <string>

#include "intensitymap.h"
#include "ui.h"

class UI;
class MapHandler{

	public : 
		MapHandler(UI *ui);
		/**
		 * Parse all the data and put the data events in a linked list.
		 * @param dataFile filename of the datafile containing all event data.
		 */
		void parseData(std::string dataFile);
		void setProcessVariables(std::string fname, double thress, int stepS);

		int binData(int timeUnit);

		/**
		 * Do rekursive calculations on every intensityMap.
		 */
		void calculateIntensityLevels(double thressHold, double maxIntensity, double minIntensity);

	private :
		Event::simInfo dataInfo;
		std::list<Event::dataEvent> dataEvents;
		std::vector<IntensityMap> intensityMaps;
		UI *ui;

		//data processing variables:
		std::string filename;
		double thresshold;
		int stepSize;

};


#endif // MAPHANDLER_H
