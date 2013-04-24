#ifndef UTILITY_H
#define UTILITY_H

#include <FL/Enumerations.H>
#include <stdio.h>
#include <mutex>

class Utility
{
	public:
		/**
		 *
		 */
		static Fl_Color thressToRainbow(double value, double min=0, double max=1){
			double thress = (value-min)/(max-min);

			//printf("%f\t%f\t%f\t%f\t%f\t%f\t%i\n",min,max,value,thress,Utility::max_cumulativeIlvl,Utility::max_frequency, c_state);
					
			uchar r = 0;
			uchar g = 0;
			uchar b = 0;
			uchar sat = 255;		
			
			double scale = max/5;

			if(thress < scale){
				b = sat;
				g = uchar (thress/scale * sat); 
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*2)){
				g = sat;
				b = uchar (sat - ((thress-scale*2)/scale * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*3)){
				g = sat;
				r = uchar ((thress-scale*3)/scale * sat-1);
				return fl_rgb_color(r,g,b);
			} else if(thress < (scale*4)){
				r = sat;
				g = uchar (sat - ((thress-scale*4)/scale * sat));
				return fl_rgb_color(r,g,b);
			} else if(thress < max){
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
		static int resolution;
		//IBlock statemachine boolean:
		static const int map_Cumulative = 1; /*<! show cumulative I levels */
		static const int map_Freq = 2; /* <! show frequency of calls */
		static const int map_Average = 3; /* <! show average I levels */

		/**
		 * Statemachine that control which state the map is in.
		 * Determines what the iblock should render
		 * @param state changes the state of the machine if a valid argument is given.
		 * @returns the current state the system is in.
		 * @see map_Cumulative
		 * @see map_Freq
		 * @see map_Average
		 */
		static int map_StateMachine(int state = 0){
			switch (state){
				case 1 :
					c_state = Utility::map_Cumulative;
					printf("changing state to cumulative\n");		
					return c_state;
					break;
				case 2 :
					c_state = Utility::map_Freq;
					printf("changing state to frequency\n");
					return c_state;
					break;
				case 3 :
					c_state = Utility::map_Average;
					printf("changing state to average\n");
					return c_state;
					break;
				case 0 :
					return c_state;
					break;
			}
		}

		/**
		 * 
		 */
		static Fl_Color getCumulativeColor(double value){
			return thressToRainbow(value,Utility::min_cumulativeIlvl,
					Utility::max_cumulativeIlvl);

		}
		static Fl_Color getFreqColor(double value){
			return thressToRainbow(value,Utility::min_frequency,
				       	Utility::max_frequency);

		}
		static Fl_Color getAvgColor(double value){
			return thressToRainbow(value, Utility::min_averageIlvl,
					Utility::max_averageIlvl);

		}


		/**
		 * Function to add values from each intensity map (via MapHandler)
		 * and set the maximum and minimum values.
		 * @param values[] array holding minmum and maximum values for each
		 * intensity map.
		 */
		static void addMaxMinValues(double values[]){
			std::lock_guard<std::mutex> lock(Utility::utilityMutex);
			if(max_cumulativeIlvl < values[0])
				max_cumulativeIlvl = values[0];
			if(max_frequency < values[1])
				max_frequency = values[1];
			if(max_averageIlvl < values[2])
				max_averageIlvl = values[2];
			if(min_cumulativeIlvl > values[3])
				min_cumulativeIlvl = values[3];
			if(min_frequency > values[4])
				max_frequency = values[4];
			if(min_averageIlvl > values[5])
				max_averageIlvl = values[5];
						
		}

	private: 
		static std::mutex utilityMutex;
		static double max_cumulativeIlvl;
		static double max_frequency;
		static double max_averageIlvl;	
		static double min_cumulativeIlvl;
		static double min_frequency;
		static double min_averageIlvl;
		static int c_state;

};



	





#endif // UTILITY_H

