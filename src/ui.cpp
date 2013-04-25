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
#include <iostream>
#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "maphandler.h"
#include "utility.h"

UI::UI(Fl_Window* window)
	:window(window)
{
	xmax = UI::UI_X -15;
	ymax = UI::UI_Y -15;

	tabs = new Fl_Tabs(2,0, xmax,ymax,"");

	mapTab = new Fl_Group(2,20,xmax-10,ymax-10,"map");
	dataTab = new Fl_Group(2,20,xmax-10,ymax-10,"data processing");

	maphandler = new MapHandler(this, mapTab);	

	tabs->add(dataTab);
	tabs->add(mapTab);
	setupDataTab();
	setupMapTab();

}


void UI::printmsg(const char* msg, ...){

	std::lock_guard<std::mutex> lock(printMutex);

	const char *nextStr = NULL;
	va_list argp;

	//output->show_insert_position();	
	outputBuffer->append(msg);

	va_start(argp, msg);
	while( (nextStr = va_arg(argp, const char*)) != NULL){
			outputBuffer->append(nextStr);
	}
	va_end(argp);


	//Fl::flush();
}
//CALLBACK FUNCTIONS:
//

void UI::showFreq_callback(Fl_Widget *w){
	showCumulative->value(0);
	showAverage->value(0);
	Utility::map_StateMachine(Utility::map_Freq);
	redrawMap();
}

void UI::showCumulative_callback(Fl_Widget *w){
	showFreq->value(0);
	showAverage->value(0);
	Utility::map_StateMachine(Utility::map_Cumulative);
	redrawMap();
}

void UI::showAverage_callback(Fl_Widget *w){
	showCumulative->value(0);
	showFreq->value(0);
	Utility::map_StateMachine(Utility::map_Average);
	redrawMap();
}


void UI::showLocation_callback(Fl_Widget *w){
	if(Utility::show_Location){
		Utility::show_Location = false;
	}else Utility::show_Location = true; 

	maphandler->redrawMap();		
}


void UI::showGrid_callback(Fl_Widget *w){
	if(Utility::show_SectorGrid){
		Utility::show_SectorGrid = false;
	} else Utility::show_SectorGrid = true;

	maphandler->redrawMap();
}

void UI::mapCounter_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->mapCounter_callback(w);
}

void UI::mapCounter_callback(Fl_Widget *w){
	maphandler->showIntensityMap(mapCounter->value());
}


void UI::zChanged_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->zChanged_callback(w);
}

void UI::zChanged_callback(Fl_Widget *w){	
	zOutput->value(zCounter->value());
}

void UI::resSlide_callback(Fl_Widget *w){
	Utility::resolution = resSlide->value()+1;
	resOutput->value(resSlide->value()+1);
}

void UI::pButton_static_callback(Fl_Widget *w, void *f){
	((UI *)f)->pButton_callback(w, ((UI *)f)->maphandler);
}

void UI::pButton_callback(Fl_Widget *w, MapHandler *m){
	processDataButton->label("processing");
	output->show_insert_position();	
	int s = int (stepSizeCounter->value());
	double t = zCounter->value();
	const char* fname = datafile->value();
	maphandler->setProcessVariables(fname, t, s);
	if(maphandler->parseData(fname)){
		ImapAmount = maphandler->binData(stepSizeCounter->value(), "IntensityMap");
		mapCounter->value(0);
		mapCounter->bounds(0, ImapAmount-1);
		double tmp = maphandler->calcMaxIntensityLevels();
		char buffer[100];
		sprintf(buffer, "max level has been calculated as %f\n, adjusting thresshold slider...", tmp);
		printmsg(buffer, NULL);
		zCounter->bounds(0, tmp);
		zCounter->value(tmp/33);
		zOutput->value(tmp/33);
		zCounter->step(tmp/100);
	} else printmsg(fname, " not found", NULL);
	calculateIButton->show();
}

void UI::calculateIButton_callback(Fl_Widget *w){
	maphandler->calcIntensityLevels(zCounter->value());
	mapCounter->show();

}

//SETUP FUNCTIONS:
void UI::setupDataTab(){
	zCounter = new Fl_Hor_Slider(5, 50, 200,25, "Z-Thresshold:");
	zCounter->bounds(0, 1);
	zCounter->step(0.01);
	zCounter->callback(zChanged_static_callback, (void*)this);
	zOutput = new Fl_Value_Output(205, 50, 100, 25, "");
	zCounter->align(FL_ALIGN_TOP_LEFT);

	resSlide = new Fl_Hor_Slider(5,100,200,25, "Resolution:");
	resSlide->bounds(0,101);

	resSlide->step(2);
	resSlide->value(3);
	resSlide->callback(resSlide_static_callback, (void*)this);

	resOutput = new Fl_Value_Output(205,100,100,25,"");
	resSlide->align(FL_ALIGN_TOP_LEFT);
	resSlide->value(Utility::resolution);
	resOutput->value(Utility::resolution);

	datafile = new Fl_Input(5,150, 350,25,"Filename:");
	datafile->insert("savefile.kas");
	datafile->align(FL_ALIGN_TOP_LEFT);

	stepSizeCounter = new Fl_Counter(5,200,200,25,"Step Size:");
	stepSizeCounter->step(1,10);
	stepSizeCounter->bounds(1, 5000);
	stepSizeCounter->value(50);
	stepSizeCounter->align(FL_ALIGN_TOP_LEFT);

	processDataButton = new Fl_Button(5,ymax-100,150,25,"Process Data");
	processDataButton->callback(pButton_static_callback, (void*)this);

	calculateIButton = new Fl_Button(175,ymax-100,150,25,"Calculate Intensities");
	calculateIButton->callback(calculateIButton_static_callback, (void*)this);
	calculateIButton->hide();

	//output-area:
	output = new Fl_Text_Display(500,50,450,600,"Output");
	outputBuffer = new Fl_Text_Buffer();
	output->buffer(outputBuffer);
}

void UI::setupMapTab(){
	/* Then setup the map tab */
	mapCounter = new Fl_Counter(5, 50, 160, 20, "Active Map:");
	mapCounter->step(1, 10);
	mapCounter->bounds(0, 5000);
	mapCounter->align(FL_ALIGN_TOP_LEFT);
	mapCounter->callback(mapCounter_static_callback, (void*)this);
	mapCounter->hide();

	showLocation = new Fl_Check_Button(5,100,100,20, "Origins:");
	showLocation->align(FL_ALIGN_TOP_LEFT);
	showLocation->callback(showLocation_static_callback, (void*)this);
	showLocation->set();

	showGrid = new Fl_Check_Button(5,140,100,20, "Block Grid:");
	showGrid->align(FL_ALIGN_TOP_LEFT);
	showGrid->callback(showGrid_static_callback, (void*)this);

	showCumulative = new Fl_Check_Button(5,180,100,20, "Cumulative");
	showCumulative->align(FL_ALIGN_TOP_LEFT);
	showCumulative->callback(showCumulative_static_callback, (void*)this);

	showAverage = new Fl_Check_Button(5,220,100,20, "Average");
	showAverage->align(FL_ALIGN_TOP_LEFT);
	showAverage->callback(showAverage_static_callback, (void*)this);
	showAverage->set();
	Utility::map_StateMachine(Utility::map_Average);

	showFreq = new Fl_Check_Button(5,260,100,20, "Frequency:");
	showFreq->align(FL_ALIGN_TOP_LEFT);
	showFreq->callback(showFreq_static_callback, (void*)this);
	
	colormap = new ColorMap(5,300,120,300,"Colour Values:"); 
	colormap->align(FL_ALIGN_TOP_LEFT);

	mapTab->add(colormap);
	mapTab->add(mapCounter);
	mapTab->add(showLocation);
	mapTab->add(showGrid);
	mapTab->add(showFreq);
	mapTab->add(showAverage);
	mapTab->add(showCumulative);
	//mapTab->hide();

}

void UI::redrawMap(){
	maphandler->redrawMap();
	colormap->hide();
	colormap->show();

}
