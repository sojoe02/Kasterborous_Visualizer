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
		IBlock(int X, int Y, int W, int H, const char *L);
		
		void draw();

		/**
		 * Add an intensity level 
		 */
		void addIntensityLevel(double ilvl);
		void getBlockValues(double &c, double &f,
				double &a);

	private:
		double cumulativeIlvl;
		double frequency;
		double averageIlvl;
		

};	


#endif // IBLOCK_H


