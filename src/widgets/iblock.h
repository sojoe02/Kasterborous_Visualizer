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

#include <FL/Enumerations.H>

#include <string>

class IBlock 
{
	public:
		IBlock(int posx,int posy);
		
		/**
		 * Add an intensity level 
		 */
		void addIntensityLevel(double ilvl);
		void getBlockValues(double &c, double &f,
				double &a);
		
		Fl_Color getColor();

		int getPosX(){return x;}
		int getPosY(){return y;}

	private:
		double cumulativeIlvl;
		double frequency;
		double averageIlvl;
		int x, y;
		

};	


#endif // IBLOCK_H


