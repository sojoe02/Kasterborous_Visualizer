#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <mutex>

#include "event.h"
#include "ui.h"
#include "maphandler.h"
#include "utility.h"


bool Utility::show_Location = true;
bool Utility::show_SectorGrid = false;

double Utility::max_cumulativeIlvl = 0;
double Utility::max_frequency = 0;
double Utility::max_averageIlvl = 0;
double Utility::min_cumulativeIlvl = 0;
double Utility::min_frequency = 0;
double Utility::min_averageIlvl = 0;

std::mutex Utility::utilityMutex;

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




