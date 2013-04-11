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

Fl_Window *window;


//All the callback functions:


int main(int argc, char **argv) {
	window = new Fl_Window(1024,680,"Kasterborous Visualizer");

	UI ui(window);	

	window->end();
	window->show(argc, argv);
	return Fl::run();
}




