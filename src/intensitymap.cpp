/*
 * =====================================================================================
 *
 *       Filename:  intensitymap.cpp
 *
 *    Description:  
 *
 *        Created:  04/10/2013 12:52:52 PM
 *
 *         Author:  Soeren V. Jorgensen (SVJ), sojoe02@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "intensitymap.h"

IntensityMap::IntensityMap(){

}

/**
 * 
 */
void IntensityMap::binEvent(Event::dataEvent devent){
	dataEvents.push_back(devent);	
}

/**
 * 
 */
void IntensityMap::normalizeIntensityLevels(double maxValue, double minValue){
	for(itIlevels = intensityLevels.begin(); itIlevels != intensityLevels.end(); ++itIlevels){
		double cLevel = *itIlevels;
		double nLevel = (maxValue - minValue) / (cLevel - minValue);
		normalizedIntensityLevels.push_back(nLevel);
	}
}
