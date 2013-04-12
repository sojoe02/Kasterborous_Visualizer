#ifndef EVENT_H
#define EVENT_H

class Event
{
	public :

	//define the data event, precisely the same as eventqueues:
	struct dataEvent{
		unsigned long long id;
		int duration;
		unsigned long long activationTime;
		double originX;
		double originY;
		double originID;
		char desc[50];
		char table[120];
	};

	struct simInfo{
		char luaFileName[150]; /*!< Filename of the lua auton thats active in this simulation*/
		unsigned long long eventAmount; /*!< Number of external events active in the simulation*/
		int numberOfAutons; /*!< Total number of autons*/
		double timeResolution; /*!< Resolution of the simulation number of microsteps possible pr second*/
		int macroFactor; /*!< Macro Factor */
		unsigned long long tmuAmount; /*!< number of total timeunist of  Timeresolution performed  */
		double areaY; /*!< Areas Y size[m] (std computer coordinate system with reversed Y axis)*/
		double areaX; /*!< Areas X size[m] */
	};


};
#endif // EVENT_H

