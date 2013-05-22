#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <mutex>

#include "../build/kast_vis.h"
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
double Utility::max_intensity = 0;
int Utility::c_state = 0;
UI* Utility::ui = NULL;

std::mutex Utility::utilityMutex;
std::mutex Utility::utilityMutexUI;

int Utility::resolution = 5;


Fl_Window *window;
UI *ui;

 void main_callback(Fl_Widget *w){
	exit(0);
}

int main(int argc, char **argv) {

	char buffer[50];
	sprintf(buffer, "Kasterborous Visualizer V. %d.%d\n",
			kast_vis_VERSION_MAJOR,
			kast_vis_VERSION_MINOR);

	Fl_Color mainColor = fl_rgb_color(102,102,102);


	window = new Fl_Window(UI::UI_X,UI::UI_Y,buffer);
	window->color(mainColor);
	ui = new UI(window);
	Utility::ui = ui;
	window->callback(main_callback);
	window->end();
	window->show(argc, argv);
	return Fl::run();
}




