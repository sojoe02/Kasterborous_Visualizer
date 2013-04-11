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

IntensityMap::IntensityMap(){

}

/**
 * 
 */
IntensityMap::binEvent(Event::dataEvent devent){
	intensityLevels.push_back(devent);	
}

/**
 * 
 */
IntensityLevels::normalizeIntensityLevels(double maxValue, double minValue){
	for(itIlevels = intensityLevels.begin(); itIlevels != intensityLevels.end(); ++itIlevels){
		double cLevel = *itIlevels;
		nLevel = (maxValue - minValue) / (cLevel - minValue);
		normalizedIntensityLevels.push_back(nLevel);
	}
}
