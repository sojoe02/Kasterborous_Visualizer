#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "event.h"
#include "ui.h"
#include "maphandler.h"
#include "utility.h"

bool Utility::location = true;
bool Utility::transMarkers = true;

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




