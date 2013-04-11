/*
 * =====================================================================================
 *
 *       Filename:  ui.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/11/2013 04:12:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Soeren V. Jorgensen (SVJ), sojoe02@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */



#include "ui.h"

UI::UI(Fl_Window* window)
	:window(window)
{

	tabs = new Fl_Tabs(2,0, 1019,675,"");
	mapTab = new Fl_Group(2,20,1010,670,"map");
	dataTab = new Fl_Group(50,20,1010,670,"data processing");
	tabs->add(dataTab);
	tabs->add(mapTab);

	//z-thresshold slider
	zCounter = new Fl_Hor_Slider(110, 50, 160,25, "Z-Thresshold");
	zCounter->bounds(0, 1);
	zCounter->step(0.01);
	double i = 4;
	zCounter->callback(zChanged_static_callback, (void*)this);
	zOutput = new Fl_Value_Output(5, 50, 100, 25, "");
	//output-area:
	output = new Fl_Text_Display(500,50,450,600,"Output");
	outputBuffer = new Fl_Text_Buffer();
	output->buffer(outputBuffer);

	dataTab->add(zCounter);
	dataTab->add(zOutput);
	dataTab->add(output);

	char buffer[40];
	for(int i = 0; i<1000; i++){
		sprintf(buffer, "%i\n",i);
		output->insert(buffer);
	}


	//	Fl_Hor_Value_Slider *timeSlider = new Fl_Hor_Value_Slider(2, 60, 140, 20, "Time-Resolution");

	mapCounter = new Fl_Counter(2, 100, 160, 20, "Active Map");
	mapCounter->step(1, 10);
	mapCounter->bounds(0, 5000);

	mapTab->add(mapCounter);

}

void UI::zChanged_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->zChanged_callback(w);
}

void UI::zChanged_callback(Fl_Widget *w){	
	zOutput->value(zCounter->value());
}



