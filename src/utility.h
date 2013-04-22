#ifndef UTILITY_H
#define UTILITY_H

#include <FL/Enumerations.H>
#include <stdio.h>

class Utility
{
	public:
		static Fl_Color thressToRainbow(double value, double min=0, double max=1){
			double thress = (value-min)/(max-min);
					
			uchar r = 0;
			uchar g = 0;
			uchar b = 0;
			uchar sat = 255;		
			
			double scale = max/5;

			if(thress < scale){
				b = sat;
				g =uchar (thress/scale * sat); 
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*2)){
				g = sat;
				b = uchar (sat - ((thress-scale*2)/scale * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*3)){
				g = sat;
				int tmp = int ((thress-scale*3)/scale * sat);
				r =tmp-1;
				//printf("%i, %f, %f \n", tmp, thress,(thress-scale*3)/scale*sat);
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*4)){
				r = sat;
				g = uchar (sat - ((thress-scale*4)/scale * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < max){
				//g= 0;
				r = sat;
				b = uchar ((thress-max)/scale * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress <= max){
				r = sat;
				b = sat;
				return fl_rgb_color(r,g,b);
			} else return FL_WHITE;
		}

		//Draw booleans:
		static bool show_Location;
		static bool show_SectorGrid;
		//IBlock control booleans:
		static bool show_CumulativeILevels;
		static bool show_Frequency;
		static bool show_AverageIlevels;		

		static int resolution;

};




#endif // UTILITY_H

