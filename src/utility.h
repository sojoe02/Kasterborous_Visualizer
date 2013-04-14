#ifndef UTILITY_H
#define UTILITY_H

#include <FL/Enumerations.H>

class Utility
{
	public:
		static Fl_Color thressToRainbow(double thress){			
			uchar r = 0;
			uchar g = 0;
			uchar b = 0;
			uchar sat = 255;		
			if(thress < 0.2){
				b = sat;
				g =uchar (thress/0.2 * sat); 
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.4){
				g = sat;
				b = uchar (sat - ((thress-0.4)/0.2 * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.6){
				g = sat;
				r = uchar ((thress-0.6)/0.2 * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.8){
				r = sat;
				g = uchar (sat - ((thress-0.8)/0.2 * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < 1.0){
				r = sat;
				b = uchar ((thress-1.0)/0.2 * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress <= 1){
				r = sat;
				b = sat;
				return fl_rgb_color(r,g,b);
			} else return FL_WHITE;
		}

};


#endif // UTILITY_H

