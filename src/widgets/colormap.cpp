/*
 * =====================================================================================
 *
 *       Filename:  colormap.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/14/2013 01:14:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Soeren V. Jorgensen (SVJ), sojoe02@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
//#include <FL/Fl.H>
//#include <FL/fl_draw.H>
#include "utility.h"
#include "colormap.h"
#include <stdio.h>
#include <string.h>

ColorMap::ColorMap(int X, int Y, int W, int H, const char *L)
	:Fl_Widget(X,Y,W,H,L)
{

}

void ColorMap::draw(){
	for(int i = 1; i <= h(); i++){

		double step = 0;
		double thress = double(i) / double(h());

		int state = Utility::map_StateMachine();

		if(state == Utility::map_Cumulative){
			step = Utility::max_cumulativeIlvl * thress;
			fl_color(Utility::getCumulativeColor(step));
		}else if(state == Utility::map_Freq){
			step = Utility::max_frequency * thress;
			fl_color(Utility::getFreqColor(step));
		}else if(state == Utility::map_Average){
			step = Utility::max_averageIlvl * thress;
			fl_color(Utility::getAvgColor(step));
		}

		fl_line(x(), y()+i, x()+w() - 50,y()+i);
		if(i % 20 == 0){
			char buffer[20];
			//fl_color(FL_BLACK);
			sprintf(buffer, "%f", step);
			fl_draw(buffer,6, x() + w() - 45, y()+ i);
		}

	}
}

