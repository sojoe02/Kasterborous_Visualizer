#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Value_Output.H>

#include "event.h"

Fl_Window *window;
Fl_Hor_Slider *zSlider;
Fl_Value_Output *zOutput;
Fl_Value_Slider *mapSlider;

void zChanged_callback(Fl_Widget *w, void *value);


int main(int argc, char **argv) {
	window = new Fl_Window(1024,680,"Kasterborous Visualizer");

	//z-thresshold slider
	zSlider = new Fl_Hor_Slider(2, 10, 160,20, "Z-Thresshold");
	//zSlider->bounds(10,50);
	double i =  3;
	zSlider->callback(zChanged_callback, &i);

	zOutput = new Fl_Value_Output(2, 90, 140, 20, "");

	//Fl_Hor_Value_Slider *timeSlider = new Fl_Hor_Value_Slider(2, 60, 140, 20, "Time-Resolution");
	//mapSlider = new Fl_Hor_Value_Slider(2, 110, 140, 20, "Active Map");
	
	window->end();
	window->show(argc, argv);
	return Fl::run();
}

void zChanged_callback(Fl_Widget *w, void *value){
	double i = *(double *)value;
	zOutput->value(zSlider->value());
}
