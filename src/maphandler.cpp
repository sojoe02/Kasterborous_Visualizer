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

MapHandler::MapHandler(UI *ui)
	:ui(ui),filename("sdfawef"), thresshold(0),stepSize(0)
{

}

void MapHandler::parseData(std::string filename){

	std::ifstream file(filename.c_str(), std::ifstream::binary);
	
	file.read(reinterpret_cast<char*>(&dataInfo), sizeof(Event::simInfo));

	while(!file.eof()){
		Event::dataEvent devent; 
		file.read(reinterpret_cast<char*>(&devent), sizeof(Event::dataEvent));
		dataEvents.push_back(devent);
	}	
		
}

/**
 * Splits the data events into a set number of intensityMaps.
 * The number of intensity maps are determined by the number of timesteps that are 
 * achievable within total number of steps, minimum unit is 1 second.
 * @param Timestep, stepsize from 1[s] to -> int_max. 
 * @return number of intensityMaps.
 */
int MapHandler::binData(int timeStep){
		std::list<Event::dataEvent>::iterator dataItr;

		double steps = (dataInfo.timeResolution * dataInfo.tmuAmount) / double(timeStep);

		for(int i = 0; i < steps; i++){
			IntensityMap imap;
			intensityMaps.push_back(imap);
		}	

		for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){
			Event::dataEvent devent = *dataItr;			
			intensityMaps.at(int(devent.activationTime/steps)).binEvent(devent);
		}
		return int (steps);
	
}

void MapHandler::setProcessVariables(std::string fname, double thress, int stepS){
	stepSize = stepS;
	thresshold = thress;
	filename = fname;

	ui->printmsg(filename.c_str());
}
