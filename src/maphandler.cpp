/*
 * =====================================================================================
 *
 *       Filename:  mapHandler.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/10/2013 02:04:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Soeren V. Jorgensen (SVJ), sojoe02@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include<iostream>
#include<fstream>
#include<stdio.h>

#include <thread>
#include <mutex>

#include"maphandler.h"

MapHandler::MapHandler(UI *ui, Fl_Group *mapTab)
	:ui(ui),filename("sdfawef"),mapTab(mapTab), thresshold(0),stepSize(0)
{

}

void MapHandler::parseData(const char* filename){

	std::ifstream file(filename, std::ifstream::binary);	
	file.read(reinterpret_cast<char*>(&dataInfo), sizeof(Event::simInfo));
	
	ui->printmsg("Loading datafile");
	ui->printmsg(dataInfo.luaFileName);

	while(!file.eof()){
	 	//char buffer[100];
		Event::dataEvent devent; 
		file.read(reinterpret_cast<char*>(&devent), sizeof(Event::dataEvent));
		//sprintf(buffer, "id: %llu, activationtime %llu",devent.id, devent.activationTime);
		//ui->printmsg(buffer);	
		dataEvents.push_back(devent);
	}
	ui->printmsg("data loaded");		
}

/**
 * Splits the data events into a set number of intensityMaps.
 * The number of intensity maps are determined by the number of timesteps that are 
 * achievable within total number of steps, minimum unit is 1 second.
 * @param Timestep, stepsize from 1[s] to -> int_max. 
 * @return number of intensityMaps.
 */
int MapHandler::binData(int timeStep, const char* L){
		std::list<Event::dataEvent>::iterator dataItr;
		int steps = int(dataInfo.tmuAmount / (dataInfo.timeResolution * double(timeStep)) + 0.5);
		//char buffer[50];
		printf("binning data into %d steps\n", steps);
		  	
		ui->printmsg("initiating intensity maps\n");
		for(int i = 0; i < steps; i++){
			IntensityMap* imap = new IntensityMap(dataInfo, mapTab->x()-100, mapTab->y()-50, 
					mapTab->w()-150, mapTab->h()-75, L);
			mapTab->add(imap);
			imap->hide();
			intensityMaps.push_back(imap);
		}		

		ui->printmsg("binning events into suitable eventmaps\n");
		for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){
			Event::dataEvent devent = *dataItr;	
			int tmp = int(devent.activationTime / (dataInfo.timeResolution * double(timeStep)) );
			//char buffer[100];
			//sprintf(buffer," inserting event at step : %d, a_tmu is : %llu \n", tmp,devent.activationTime);
			//ui->printmsg(buffer);		
			intensityMaps.at(tmp)->binEvent(devent);
		}
		ui->printmsg("binning of data done\n");
		return int (steps);
}
/**
 * 
 */
void MapHandler::setThreadData(){
	std::vector<IntensityMap*>::iterator mapItr;

	int i = 0;

	for(mapItr = intensityMaps.begin(); mapItr != intensityMaps.end(); mapItr++, i++){
		IntensityMap* imap = *mapItr;
		threadmap.at(i).push_back(imap);
		if(i == threadAmount)
			i = 0;
	}
}
/**
 * 
 */
void MapHandler::calcIntensityLevels(){

	std::thread* threads[threadAmount];

	for(int i = 0; i < threadAmount; i++){
		threads[i] = new std::thread(calculateIData, this, threadmap.at(i));
	}
	for(int i = 0; i< threadAmount; i++){
		threads[i]->join();
		delete threads[i];
	}
	
}
/**
 * 
 */
void MapHandler::calculateIData(MapHandler* m, std::vector<IntensityMap*> idata){
	std::vector<IntensityMap*>::iterator idtr;
	for(idtr = idata.begin(); idtr != idata.end(); idtr++){
		//*idtr.calculateMapValues();
	}	
}

void MapHandler::setProcessVariables(const char* fname, double thress, int stepS){
	stepSize = stepS;
	thresshold = thress;
	filename = fname;

	//ui->printmsg(filename);
}
