#ifndef EVENT_H
#define EVENT_H

class Event
{
	public :

		//define the data event, precisely the same as events:
		struct dataEvent{
			unsigned long long id;
			double duration;
			unsigned long long activationTime;
			double propagationSpeed;
			double originX;
			double originY;
			int originID;
			char desc[150];
			char table[500];
		};


		struct simInfo{
			char luaFileName[250]; /*!< Filename of the lua auton thats active in this simulation*/
			unsigned long long eventAmount; /*!< Number of external events active in the simulation*/
			int numberOfAutons; /*!< Total number of autons*/
			double timeResolution; /*!< Resolution of the simulation number of microsteps possible pr second*/
			int macroFactor; /*!< Macro Factor */
			unsigned long long tmuAmount; /*!< number of total timeunist of  Timeresolution performed  */
			double areaY; /*!< Areas Y size[m] (std computer coordinate system with reversed Y axis)*/
			double areaX; /*!< Areas X size[m] */
		};

		struct autonInfo{
			int ID;
			int eventAmount;
			char info[1000];
		};


};
#endif // EVENT_H

