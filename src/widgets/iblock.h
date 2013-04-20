/*
 * =====================================================================================
 *
 *       Filename:  iblock.h
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

#ifndef IBLOCK_H
#define IBLOCK_H

#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

#include <string>

class IBlock : public Fl_Button
{
	public:
		IBlock(double intensityLvl,std::string msg,
				int X, int Y, int W, int H, const char *L);
		
		void draw();

	private:
		double intensityLvl;
		std::string msg;

};	


#endif // IBLOCK_H


