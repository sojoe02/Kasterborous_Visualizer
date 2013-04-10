#ifndef EVENT_H
#define EVENT_H

class Event{

	//define the data event, precisely the same as eventqueues:
	struct dataEvent{
		unsigned long long id;
		int duration;
		double originX;
		double originY;
		double originID;
		char desc[50];
		char table[120];
	};

	struct info{

	};

};

#endif // EVENT_H

