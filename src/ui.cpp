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
#include "maphandler.h"
#include "utility.h"

UI::UI(Fl_Window* window)
	:window(window)
{
	maphandler = new MapHandler(this);

	xmax = UI::UI_X -15;
	ymax = UI::UI_Y -15;

	tabs = new Fl_Tabs(2,0, xmax,ymax,"");

	mapTab = new Fl_Group(2,20,xmax-10,ymax-10,"map");
	dataTab = new Fl_Group(2,20,xmax-10,ymax-10,"data processing");
	tabs->add(dataTab);
	tabs->add(mapTab);
	setupDataTab();

	/* Then setup the map tab */

	//char buffer[40];
	//for(int i = 0; i<1000; i++){
	//	sprintf(buffer, "%i\n",i);
	//	output->insert(buffer);
	//}
	//	Fl_Hor_Value_Slider *timeSlider = new Fl_Hor_Value_Slider(2, 60, 140, 20, "Time-Resolution");
	mapCounter = new Fl_Counter(5, 50, 160, 20, "Active Map:");
	mapCounter->step(1, 10);
	mapCounter->bounds(0, 5000);
	mapCounter->align(FL_ALIGN_TOP_LEFT);

	colormap = new ColorMap(5,120,160,window->h() - 170,"Colour Map:"); 
	colormap->align(FL_ALIGN_TOP_LEFT);

	mapTab->add(colormap);
	mapTab->add(mapCounter);
}


void UI::printmsg(const char* msg){
	output->show_insert_position();	
	output->insert(msg);
	output->insert("\n");
}



//CALLBACK FUNCTIONS:
void UI::zChanged_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->zChanged_callback(w);
}

void UI::zChanged_callback(Fl_Widget *w){	
	zOutput->value(zCounter->value());
}

void UI::pButton_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->pButton_callback(w, ((UI *)f)->maphandler);
}

void UI::pButton_callback(Fl_Widget *w, MapHandler *m){
	output->show_insert_position();	
	int s = int (stepSizeCounter->value());
	double t = zCounter->value();
	const char* fname = datafile->value();
	maphandler->setProcessVariables(fname, t, s);
	maphandler->parseData(fname);
}

//SETUP FUNCTIONS:

void UI::setupDataTab(){
	zCounter = new Fl_Hor_Slider(5, 50, 200,25, "Z-Thresshold:");
	zCounter->bounds(0, 1);
	zCounter->step(0.01);
	zCounter->callback(zChanged_static_callback, (void*)this);
	zOutput = new Fl_Value_Output(205, 50, 100, 25, "");
	zCounter->align(FL_ALIGN_TOP_LEFT);

	datafile = new Fl_Input(5,150, 350,25,"Filename:");
	datafile->insert("savefile.kas");
	datafile->align(FL_ALIGN_TOP_LEFT);

	stepSizeCounter = new Fl_Counter(5,100,200,25,"Step Size:");
	stepSizeCounter->step(1,10);
	stepSizeCounter->bounds(1, 5000);
	stepSizeCounter->align(FL_ALIGN_TOP_LEFT);

	processDataButton = new Fl_Button(5,ymax-100,200,25,"Process Data");
	processDataButton->callback(pButton_static_callback, (void*)this);

	//output-area:
	output = new Fl_Text_Display(500,50,450,600,"Output");
	outputBuffer = new Fl_Text_Buffer();
	output->buffer(outputBuffer);
}
