#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "event.h"
#include "ui.h"
#include "maphandler.h"
#include "utility.h"


bool Utility::show_Location = true;
bool Utility::show_SectorGrid = false;
bool Utility::show_CumulativeILevels = false;
bool Utility::show_Frequency = false;
bool Utility::show_AverageIlevels = false;

int Utility::resolution = 5;

Fl_Window *window;
UI *ui;
//MapHandler *maphandler;
//All the callback functions:


int main(int argc, char **argv) {
	window = new Fl_Window(UI::UI_X,UI::UI_Y,"Kasterborous Visualizer");

	UI ui(window);

	//maphandler->setProcessVariables("323",5,5);	

	window->end();
	window->show(argc, argv);
	return Fl::run();
}




