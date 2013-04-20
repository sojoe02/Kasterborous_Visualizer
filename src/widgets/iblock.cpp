/*
 * =====================================================================================
 *
 *       Filename:  iblock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 01:13:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "iblock.h"
#include "utility.h"

IBlock::IBlock(double intensityLvl,std::string msg,int X, int Y, int W, int H, const char *L)
	:intensityLvl(intensityLvl),msg(msg),Fl_Button(X,Y,W,H,L){


	}

void IBlock::draw(){

	fl_color(Utility::thressToRainbow(intensityLvl));

	for(int i = 0; i<h();i++){
		fl_line(x(),y()+i,w(),h());
	}	
}
