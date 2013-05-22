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
#include"utility.h"

MapHandler::MapHandler(UI *ui, Fl_Group *mapTab)
	:ui(ui),threadAmount(4),mapTab(mapTab), thresshold(0),stepSize(0)
{

}

bool MapHandler::parseData(const char* filename){
	std::ifstream file(filename, std::ifstream::binary);
	
	if(file.is_open()){	
		file.read(reinterpret_cast<char*>(&dataInfo), sizeof(Event::simInfo));
		ui->printmsg("Loading datafile\nLUA filename is:");
		ui->printmsg(dataInfo.luaFileName);
		ui->printmsg("\n");
		//ui->printmsg(dataInfo.luaFileName);
		while(!file.eof()){
			//char buffer[100];
			Event::dataEvent devent; 
			file.read(reinterpret_cast<char*>(&devent), sizeof(Event::dataEvent));
			//sprintf(buffer, "id: %llu, activationtime %llu",devent.id, devent.activationTime);			
			dataEvents.push_back(devent);
		}
		ui->setProgressMinMax(1,dataEvents.size());
		ui->printmsg("Data loaded\n");
		return true;
	} else return false;
}

/**
 * Splits the data events into a set number of intensityMaps.
 * The number of intensity maps are determined by the number of timesteps that are 
 * achievable within total number of steps, minimum unit is 1 second.
 * @param Timestep, stepsize from 1[s] to -> int_max. 
 * @return number of intensityMaps.
 */
int MapHandler::binData(int timeStep, const char* L){

	activeIMap = NULL;
	
	std::vector<IntensityMap *>::iterator imapItr;
	for(imapItr = intensityMaps.begin(); imapItr!=intensityMaps.end(); imapItr++){
		Fl::delete_widget(*imapItr);
	}

	intensityMaps.clear();	
	threadmap.clear();

	std::list<Event::dataEvent>::iterator dataItr;
	
	int steps = int(dataInfo.tmuAmount / (dataInfo.timeResolution * double(timeStep)) + 0.5);
	unsigned long long intensityPeriod = dataInfo.tmuAmount / steps;
	

	char buffer[50];
	sprintf(buffer,"binning data into %d steps %llu \n", steps,dataInfo.tmuAmount);
	ui->printmsg(buffer);
	ui->printmsg("\nInitiating intensity maps\n");
	for(int i = 0; i < steps; i++){
		char buffer2[50];
		sprintf(buffer2, "Intensity Map %i[s] to %i[s]", i*timeStep, i*timeStep+timeStep);
		std::string msg = buffer2;
		//printf("%s : %s\n",buffer2 , msg.c_str());
		IntensityMap* imap = new IntensityMap(msg,dataInfo,i,intensityPeriod, 185, 50,630, 630, "Intensity Map:");
		mapTab->add(imap);
		imap->labelcolor(FL_LIGHT3);
		imap->hide();
		intensityMaps.push_back(imap);
		imap->align(FL_ALIGN_TOP_LEFT);
	}		

	//ui->printmsg("Binning events into suitable eventmaps\n");
	for(dataItr = dataEvents.begin(); dataItr != dataEvents.end(); dataItr++){
		Event::dataEvent devent = *dataItr;	
		int tmp = int(devent.activationTime / (dataInfo.timeResolution * double(timeStep)) - 0.5);
		//char buffer[100];
		//sprintf(buffer," inserting event at step : %d, a_tmu is : %llu \n", tmp,devent.activationTime);
		//ui->printmsg(buffer, NULL);		
		intensityMaps.at(tmp)->binEvent(devent);
	}
	ui->printmsg("Binning of data done\n");	

	return intensityMaps.size();
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
	ui->resetProgress();
	ui->printmsg("calculating intensity levels\n");
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
	double ilvl = 0;
	double tmp = 0;

	for(; itr!= intensityMaps.end(); itr++){
		tmp = (*itr)->getMaxIntensity();
		if(tmp > ilvl)	
			ilvl = tmp;	
	}
	
	std::vector<IntensityMap *>::iterator iItr;
	for(iItr = intensityMaps.begin(); iItr != intensityMaps.end(); iItr++){
		(*iItr)->setMaxIntensity(ilvl);
	}
	Utility::max_intensity = ilvl;
	return ilvl;
}

/**
 * Choose which map that should be shown
 */
void MapHandler::showIntensityMap(int index){
	if(activeIMap != NULL)
		activeIMap->hide();

	if(!intensityMaps.empty() && index < intensityMaps.size()){
		activeIMap = intensityMaps.at(index);
		activeIMap->show();
	}
}

void MapHandler::redrawMap(){
	if(activeIMap!=NULL){
		activeIMap->hide();
		activeIMap->show();
	}
}

/**
 * Calculates the maximum amount of intensity level for each Intensity map.
 * Designed to be run in a seperate thread.
 * @param m pointer to this maphandler
 * @param idata vector with a chuck of imaps (all imaps / threadnumber)
 */
void MapHandler::calculateMaxIData(MapHandler *m, std::vector<IntensityMap*> idata){
	Utility::printmsg("starting thread\n");
	std::vector<IntensityMap*>::iterator idtr;
	for(idtr = idata.begin(); idtr != idata.end(); idtr++){
		(*idtr)->calculateMaxIntensity();
	}
}

/**
 * Calculate local intensity levels for each of the intensitymaps.
 * This function starts a thread @see threadAmount for each intensity map vector
 * @see MapHandler::calculateIData
 * @param thresshold, minimum impact value for the call calculations, how far will
 * the recursive algorithm go.
 * 
 */
void MapHandler::calcIntensityLevels(double thresshold){
	ui->printmsg("Calculating Intensity levels\n");
	ui->resetProgress();

	std::thread* threads[threadAmount];

	for(int i = 0; i < threadAmount; i++){
		threads[i] = new std::thread(calculateIData, this, threadmap.at(i),thresshold);
	}
	for(int i = 0; i< threadAmount; i++){
		threads[i]->join();
		delete threads[i];
		printf("exiting");
	}
}
/**
 * Static function meant to be run in a seperate thread. 
 * takes a predefined chunck of Intensity maps (all intensity maps / threadAmount) 
 * and calculates their intensity levels.
 * @param m pointer to this maphandler
 * @param idata vector containing a chunk of the active Intensitymap widgets
 */
void MapHandler::calculateIData(MapHandler* m, std::vector<IntensityMap*> idata, double thresshold){
	std::vector<IntensityMap*>::iterator idtr;
	for(idtr = idata.begin(); idtr != idata.end(); idtr++){
		(*idtr)->calculateIlevel(thresshold);
	}	
}


void MapHandler::setProcessVariables(const char* fname, double thress, int stepS){
	stepSize = stepS;
	thresshold = thress;
	filename = fname;
	//ui->printmsg(filename);
}

void MapHandler::generateDynamicMap(double thresshold){
	if(activeIMap != NULL){
		activeIMap->calculateDlevel(thresshold);
	}
}
