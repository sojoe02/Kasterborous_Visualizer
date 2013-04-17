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
	

	ui->printmsg("Loading datafile\n LUA filename is:", dataInfo.luaFileName, "\n",NULL);
	//ui->printmsg(dataInfo.luaFileName);
	

	while(!file.eof()){
	 	//char buffer[100];
		Event::dataEvent devent; 
		file.read(reinterpret_cast<char*>(&devent), sizeof(Event::dataEvent));
		//sprintf(buffer, "id: %llu, activationtime %llu",devent.id, devent.activationTime);
		//ui->printmsg(buffer);	
		dataEvents.push_back(devent);
	}
	ui->printmsg("data loaded\n",NULL);		
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
		char buffer[50];
		sprintf(buffer,"binning data into %d steps\n", steps);
		ui->printmsg(buffer,NULL);
		  	
		ui->printmsg("Initiating intensity maps\n",NULL);
		for(int i = 0; i < steps; i++){
			IntensityMap* imap = new IntensityMap(dataInfo, mapTab->x()-100, mapTab->y()-50, 
					mapTab->w()-150, mapTab->h()-75, L);
			mapTab->add(imap);
			imap->hide();
			intensityMaps.push_back(imap);
		}		

		//ui->printmsg("Binning events into suitable eventmaps\n");
		for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){
			Event::dataEvent devent = *dataItr;	
			int tmp = int(devent.activationTime / (dataInfo.timeResolution * double(timeStep)) );
			//char buffer[100];
			//sprintf(buffer," inserting event at step : %d, a_tmu is : %llu \n", tmp,devent.activationTime);
			//ui->printmsg(buffer, NULL);		
			intensityMaps.at(tmp)->binEvent(devent);
		}
		ui->printmsg("Binning of data done\n", NULL);
		return int (steps);
}
/**
 * 
 */
void MapHandler::setThreadData(){
	std::vector<IntensityMap*>::iterator mapItr;

	int i = 0;

	for(int i = 0; i<threadAmount; i++){
		thread_map tmp;		
		threadmap.push_back(tmp);
	}

	for(mapItr = intensityMaps.begin(); mapItr != intensityMaps.end(); mapItr++, i++){
		if(i == threadAmount)
			i = 0;
		IntensityMap* imap = *mapItr;
		threadmap.at(i).push_back(imap);
		
	}
}

/**
 * Calculate the maximum possible Intensity level
 */
double MapHandler::calcMaxIntensityLevels(){

	ui->printmsg("calculating intensity levels\n", NULL);
	std::thread* threads[threadAmount];
	setThreadData();

	for(int i = 0; i < threadAmount; i++){
		threads[i] = new std::thread(calculateMaxIData, this, threadmap.at(i));
	}
	for(int i = 0; i< threadAmount; i++){
		threads[i]->join();
		delete threads[i];
	}
	std::vector<IntensityMap *>::iterator itr = intensityMaps.begin();
	double i = 0;
	double tmp = 0;

	for(; itr!= intensityMaps.end(); itr++){
		//char buffer[50];
		//sprintf(buffer,"tmp is now %f \n", tmp);
		//ui->printmsg(buffer,NULL);
		
		tmp = (*itr)->getMaxIntensity();
		if(tmp > i)	
			i = tmp;	
	}
	return i;
}

/**
 * Calculates the maximum amount of intensity level for each Intensity map.
 */
void MapHandler::calculateMaxIData(MapHandler *m, std::vector<IntensityMap*> idata){
	m->ui->printmsg("starting thread\n",NULL);
	std::vector<IntensityMap*>::iterator idtr;
	for(idtr = idata.begin(); idtr != idata.end(); idtr++){
		(*idtr)->calculateMaxIntensity();
	}
}

/**
 * Calculate local intensity levels for each of the intensitymaps.
 * This function starts a threads for each intensity map vector
 * @see MapHandler::calculateIData
 * 
 */
void MapHandler::calcIntensityLevels(){

	ui->printmsg("calculating intensity levels\n", NULL);

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
 * Static function meant to be run in a seperate thread. 
 * takes a predefined chunck of Intensity maps and calculates their
 * intensity levels.
 * @param m pointer to this maphandler
 * @param idata vector containing a chunk of the active Intensitymap widgets
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
