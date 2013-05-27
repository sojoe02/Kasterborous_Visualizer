#ifndef UTILITY_H
#define UTILITY_H

#include <FL/Enumerations.H>
#include <stdio.h>
#include <mutex>
#include <atomic>

#include "ui.h"

class UI;
class Utility
{
	public:
		//Draw booleans:
		static bool show_Location;
		static bool show_SectorGrid;
		static int resolution;
		//IBlock statemachine boolean:
		static const int map_Cumulative = 1; /*<! show cumulative I levels */
		static const int map_Freq = 2; /* <! show frequency of calls */
		static const int map_Average = 3; /* <! show average I levels */
		static const int map_Highest = 4; /* <! show highest I levels */

		
		static double max_cumulativeIlvl;
		static double max_frequency;
		static double max_averageIlvl;	
		static double min_cumulativeIlvl;
		static double min_frequency;
		static double min_averageIlvl;

		static double max_intensity;

		static UI* ui;

		static void incrementProgress(double value){
			std::lock_guard<std::mutex> lock(Utility::utilityMutexUI);
			ui->incrementProgress(value);
		}

		static void printmsg(const char* msg){
			ui->printmsg(msg);
		}

		static void incrementDProgress(int value, const char* msg, int color){
			std::lock_guard<std::mutex> lock(Utility::utilityMutexUI);
			ui->incrementDProgress(value,msg,color);
		}

		static void setDProgressMinMax(int min, int max){
			std::lock_guard<std::mutex> lock(Utility::utilityMutexUI);
			ui->setDProgressMinMax(min,max);
		}


		//static void write
		/**
		 * Colormapper that upon receiving a value between a min and max returns a FLTK
		 * RGB color value. 
		 * Basicly it's a HSV to RGB mapper, where 255 blue represents the 
		 * lowest value and 'pink' the higest (<a href="http://en.wikipedia.org/wiki/HSL_and_HSV">wikipedia on HSV</a>.)
		 * @param min minimal possible value @see Utility::min_cumulativeIlvl @see Utility::min_frequency @see Utility::min_averageIlvl;
		 * @param max highest possible value @see Utility::max_cumulativeIlvl @see Utility::max_frequency @see Utility::max_averageIlvl;
		 * @param value a value between min and max
		 */
		static Fl_Color thressToRainbow(double value, double min=0, double max=1){
			double thress = (value-min)/(max-min);

			uchar r = 0;
			uchar g = 0;
			uchar b = 0;
			int sat = 255;		
		
			if(thress < 0.2){
				b = sat;
				g = uchar(thress/0.2* sat);	
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.399){
				g = sat;
				b = uchar (sat -(thress-0.4)/0.2 * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.599){
				g = sat;
				r = uchar((thress-0.6)/0.2 * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.799){
				r = sat;
				g = sat - uchar((thress-0.8)/0.2* sat);	
				return fl_rgb_color(r,g,b);
			} else if(thress < 0.999){
				r = sat;
				b = uchar((thress-1)/0.2 * sat);
				return fl_rgb_color(r,g,b);
			} else if(thress <= 1){
				r = sat;
				b = sat;
				return fl_rgb_color(r,g,b);
			} else return FL_WHITE;
		}
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
				case 4 :
					c_state = Utility::map_Highest;
					printf("changing state to highest\n");
					return c_state;
					break;
				case 0 :
					return c_state;
					break;
			}
		}

		/**
		 * Get color based on a cumulative intensity value.
		 * param value between minimum- and maximum cumulative value.
		 */
		static Fl_Color getCumulativeColor(double value){
			return thressToRainbow(value,Utility::min_cumulativeIlvl,
					Utility::max_cumulativeIlvl);
			/**
			 * Get color based on a frequency value.
			 * param value between minimum- and maximum frequency value.
			 */
		}
		static Fl_Color getFreqColor(double value){
			return thressToRainbow(value,Utility::min_frequency,
					Utility::max_frequency);
		}
		/**
		 * Get color based on a average intensity value.
		 * param value between minimum- and maximum intensity value.
		 */
		static Fl_Color getAvgColor(double value){
			return thressToRainbow(value, Utility::min_averageIlvl,
					Utility::max_averageIlvl);

		}
		/**
		 * Get color based on the highest intensity value.
		 * param value between minimum- and maximum intensity value.
		 */
		static Fl_Color getHighestColor(double value){
			return thressToRainbow(value, 0,
					Utility::max_intensity);

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
		static std::mutex utilityMutexUI;
		static int c_state;

};









#endif // UTILITY_H

