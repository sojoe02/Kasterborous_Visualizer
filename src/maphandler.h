#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include <list>
#include <vector>
#include <string>

#include <FL/Fl_Group.H>

#include "intensitymap.h"
#include "ui.h"

class UI;
class MapHandler{

	public : 
		MapHandler(UI *ui, Fl_Group *mapTab);
		/**
		 * Parse all the data and put the data events in a linked list.
		 * @param dataFile filename of the datafile containing all event data.
		 */
		void parseData(const char* filename);
		void setProcessVariables(const char* fname, double thress, int stepS);
		
		void setThreadData();

		int binData(int timeStep, const char* L);
		/**
		 * Do rekursive calculations on every intensityMap.
		 */
		void findOriginPositions();
		void calcIntensityLevels();
		static void calculateIData(MapHandler *m, std::vector<IntensityMap*> idata);

	private :
		Event::simInfo dataInfo;
		std::list<Event::dataEvent> dataEvents;
		std::vector<IntensityMap *> intensityMaps;
		IntensityMap *activeIMap;

		typedef std::vector<IntensityMap *> thread_map;
		std::vector<thread_map> threadmap;
		int threadAmount = 6;
		

		UI *ui;
		//data processing variables:
		std::string filename;
		double thresshold;
		int stepSize;
		Fl_Group *mapTab;
		
};


#endif // MAPHANDLER_H
