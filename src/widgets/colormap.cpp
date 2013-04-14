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
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "../utility.h"
#include "colormap.h"
#include <stdio.h>
#include <string.h>

ColorMap::ColorMap(int X, int Y, int W, int H, const char *L)
	:Fl_Widget(X,Y,W,H,L)
{

}

void ColorMap::draw(){
	for(int i = 1; i <= h(); i++){
		double thress = double(i) / double(h());
		fl_color(Utility::thressToRainbow(thress));
		fl_line(x(), y()+i, x()+w() - 50,y()+i);
		if(i % 20 == 0){
			char buffer[20];
			sprintf(buffer, "t=%f", thress);
			fl_draw(buffer,6, x() + w() - 45, y()+ i);
		}

	}
}

