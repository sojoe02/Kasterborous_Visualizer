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
#include <limits.h>

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_XPM_Image.H>
#include <Fl/Fl_Pixmap.H>
#include <FL/Fl_Image.H>

#include "ui.h"
#include "maphandler.h"
#include "utility.h"

UI::UI(Fl_Window* window)
	:window(window)
{
	xmax = UI::UI_X -5;
	ymax = UI::UI_Y -5;

	fl_register_images();

	tabs = new Fl_Tabs(2,0, xmax,ymax,"");

	mapTab = new Fl_Group(2,30,xmax,ymax,"Intensity Map Processing");
	dataTab = new Fl_Group(2,30,xmax,ymax,"Event Data Processing");
	
	Fl_Color tabColor = fl_rgb_color(200,200,200);


	//mapTab->image(mapIcon);
	//mapTab->color(tabColor);
	//dataTab->color(tabColor);

	maphandler = new MapHandler(this, mapTab);	

	tabs->add(dataTab);
	tabs->add(mapTab);
	setupDataTab();
	setupMapTab();

}


void UI::printmsg(const char* msg){
	std::lock_guard<std::mutex> lock(printMutex);
	outputBuffer->append(msg);	
	Fl::check();
}

void UI::setProgressMinMax(int min, int max){
	progress->maximum(max);
	progress->minimum(min);	
}

void UI::incrementProgress(double value = 1){
	progress->value(progress->value() + value);
	char percent[20];
	sprintf(percent,"%d%%", int (progress->value()/progress->maximum()*100.0));
	progress->label(percent);
	Fl::check();
}

void UI::setDProgressMinMax(int min, int max){
	d_progress->value(0);
	d_progress->maximum(max);
	d_progress->minimum(min);	
}


void UI::incrementDProgress(double value, const char* msg, int color){
	if(color == 1){
		d_progress->selection_color(0xFFFF9900);     // progress bar color
	}else d_progress->selection_color(0x4444ff00);     // progress bar color

	d_progress->value(d_progress->value() + value);
	char percent[100];
	sprintf(percent,"%s - %d%%",msg, int (d_progress->value()/d_progress->maximum()*100.0));
	d_progress->label(percent);
	Fl::check();
}

//CALLBACK FUNCTIONS:
//

void UI::showFreq_callback(Fl_Widget *w){
	showCumulative->value(0);
	showAverage->value(0);
	showHighest->value(0);
	Utility::map_StateMachine(Utility::map_Freq);
	redrawMap();
}

void UI::showCumulative_callback(Fl_Widget *w){
	showFreq->value(0);
	showAverage->value(0);
	showHighest->value(0);
	Utility::map_StateMachine(Utility::map_Cumulative);
	redrawMap();
}

void UI::showAverage_callback(Fl_Widget *w){
	showCumulative->value(0);
	showFreq->value(0);
	showHighest->value(0);
	Utility::map_StateMachine(Utility::map_Average);
	redrawMap();
}


void UI::showHighest_callback(Fl_Widget *w){
	showCumulative->value(0);
	showFreq->value(0);
	showAverage->value(0);
	Utility::map_StateMachine(Utility::map_Highest);
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

void UI::stepSizeCounter_callback(Fl_Widget *w){	
	imapOutput->value(stepSizeCounter->value());
}

void UI::resSlide_callback(Fl_Widget *w){
	if(resSlide->value() == 8){
		resSlide->value(9);
	}
	if(resSlide->value() == 4){
		resSlide->value(5);
	}

	Utility::resolution = resSlide->value();
	resOutput->value(resSlide->value());
}

void UI::pButton_callback(Fl_Widget *w, MapHandler *m){
	processDataButton->deactivate();
	processDataButton->label("Processing...");
	Fl::check();
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
		sprintf(buffer, "Max level has been calculated as %f\n Adjusting thresshold slider\n", tmp);
		printmsg(buffer);
		zCounter->bounds(0, tmp);
		zCounter->value(tmp/33);
		zOutput->value(tmp/33);
		zCounter->step(tmp/100);
		calculateIButton->activate();
		zCounter->activate();
		mapCounter->show();
	} else {
		printmsg(fname);
		printmsg(" not found\n");
	}
	processDataButton->label("Bin Events Into Intensity Maps");
	processDataButton->activate();
}

void UI::calculateIButton_callback(Fl_Widget *w){
	processDataButton->deactivate();
	calculateIButton->deactivate();
	calculateIButton->label("Generating Intensity Maps...");
	maphandler->calcIntensityLevels(zCounter->value());
	tabs->value(mapTab);
	maphandler->showIntensityMap(mapCounter->value());
	calculateIButton->label("Process Intensity Maps");
	calculateIButton->deactivate();
	zCounter->deactivate();
	processDataButton->activate();

}

void UI::processDMapButton_callback(Fl_Widget *w){
	processDataButton->deactivate();
	processDMapButton->deactivate();

	processDMapButton->label("Processing...");
	maphandler->generateDynamicMap(zCounter->value());
	processDMapButton->label("Write\n Dynamic Map");

	processDMapButton->activate();
	processDataButton->activate();
}

void UI::browseButton_callback(Fl_Widget *w){
	// Create the file chooser, and show it
	Fl_File_Chooser chooser(".",                // directory
			"*.kas",                    // filter
			Fl_File_Chooser::MULTI,     // chooser type
			"Finding Kasterborous Data File");        // title
	chooser.show();
	// Block until user picks something.
	while(chooser.shown()){
		Fl::wait();
	}
	// User hit cancel?
	if ( chooser.value() == NULL ){
		fprintf(stderr, "(User hit 'Cancel')\n"); return;
	}
	char buffer[200];
	sprintf(buffer,"%s",chooser.value());
	datafile->value(buffer);
}

//SETUP FUNCTIONS:
void UI::setupDataTab(){
	zCounter = new Fl_Hor_Slider(5, 550, 300,25, "Z-Thresshold:");
	zCounter->bounds(0, 1);
	zCounter->step(0.01);
	zCounter->callback(zChanged_static_callback, (void*)this);
	zOutput = new Fl_Value_Output(305, 550, 100, 25, "");
	zCounter->align(FL_ALIGN_TOP_LEFT);
	zCounter->deactivate();

	resSlide = new Fl_Hor_Slider(5,100,300,25, "Resolution:");
	resSlide->bounds(1,9);

	resSlide->step(1);
	resSlide->value(4);
	resSlide->callback(resSlide_static_callback, (void*)this);

	resOutput = new Fl_Value_Output(305,100,100,25,"");
	resSlide->align(FL_ALIGN_TOP_LEFT);
	resSlide->value(Utility::resolution);
	resOutput->value(Utility::resolution);

	datafile = new Fl_Input(5,50, 300,25,"Filename:");
	datafile->insert("savefile.kas");
	datafile->align(FL_ALIGN_TOP_LEFT);

	browseButton = new Fl_Button(305,50,100,25,"Browse");
	browseButton->callback(browseButton_static_callback, (void*)this);

	stepSizeCounter = new Fl_Hor_Slider(5,150,300,25,"Intensity Map Time Coverage[s]:");
	stepSizeCounter->value(360);
	stepSizeCounter->bounds(1,3600);
	stepSizeCounter->step(10);
	imapOutput = new Fl_Value_Output(305,150,100,25,"");
	imapOutput->value(360);
	stepSizeCounter->callback(stepSizeCounter_static_callback, (void*)this);
	stepSizeCounter->align(FL_ALIGN_TOP_LEFT);

	processDataButton = new Fl_Button(5,200,405,50,"Bin Events Into Intensity Maps");
	processDataButton->callback(pButton_static_callback, (void*)this);

	calculateIButton = new Fl_Button(5,600,405,50,"Process Intensity Maps");
	calculateIButton->callback(calculateIButton_static_callback, (void*)this);
	calculateIButton->deactivate();

	//output-area:
	output = new Fl_Text_Display(500,50,500,660,"Output:");
	output->align(FL_ALIGN_TOP_LEFT);
	outputBuffer = new Fl_Text_Buffer();
	output->buffer(outputBuffer);

	imageBox = new Fl_Box(5,250,405,300);
	Fl_XPM_Image* mapIcon = new Fl_XPM_Image("../icon/frog.xpm");
	//Fl_Pixmap icon("../icon/frog.xpm");
	//icon->draw(5,250);
	imageBox->image(mapIcon);



	//progress-bar:
	progress = new Fl_Progress(5,680,405,30);
	progress->color(0x88888800);               // background color
	progress->selection_color(0x4444ff00);     // progress bar color
	progress->labelcolor(FL_WHITE);            // percent text color
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
	//showLocation->set();

	showGrid = new Fl_Check_Button(5,140,100,20, "Block Grid:");
	showGrid->align(FL_ALIGN_TOP_LEFT);
	showGrid->callback(showGrid_static_callback, (void*)this);

	showCumulative = new Fl_Check_Button(5,180,100,20, "Cumulative");
	showCumulative->align(FL_ALIGN_TOP_LEFT);
	showCumulative->callback(showCumulative_static_callback, (void*)this);

	showHighest = new Fl_Check_Button(5,220,100,20, "Highest");
	showHighest->align(FL_ALIGN_TOP_LEFT);
	showHighest->callback(showHighest_static_callback, (void*)this);
	showHighest->set();
	Utility::map_StateMachine(Utility::map_Highest);


	showAverage = new Fl_Check_Button(5,260,100,20, "Average");
	showAverage->align(FL_ALIGN_TOP_LEFT);
	showAverage->callback(showAverage_static_callback, (void*)this);

	showFreq = new Fl_Check_Button(5,300,100,20, "Frequency:");
	showFreq->align(FL_ALIGN_TOP_LEFT);
	showFreq->callback(showFreq_static_callback, (void*)this);

	colormap = new ColorMap(xmax-190,50,120,630,"Colour Values:"); 
	colormap->align(FL_ALIGN_TOP_LEFT);

	processDMapButton = new Fl_Button(5,630,160,50,"Write\n Dynamic Map");
	processDMapButton->callback(processDMapButton_static_callback, (void*)this);


	//progress-bar:
	d_progress = new Fl_Progress(5,690,900,30);
	d_progress->color(0x88888800);               // background color
	d_progress->selection_color(0xff000000);     // progress bar color
	d_progress->labelcolor(FL_WHITE);


	mapTab->add(colormap);
	mapTab->add(mapCounter);
	mapTab->add(showLocation);
	mapTab->add(showGrid);
	mapTab->add(showFreq);
	mapTab->add(showAverage);
	mapTab->add(showHighest);

	mapTab->add(showCumulative);
	mapTab->add(d_progress);
	mapTab->add(processDMapButton);
	//mapTab->hide();

}

void UI::redrawMap(){
	maphandler->redrawMap();
	colormap->hide();
	colormap->show();

}
