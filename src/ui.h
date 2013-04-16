#ifndef UI_H
#define UI_H

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
#include <FL/Fl_Input.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <string>

#include "maphandler.h"
#include "colormap.h"

class MapHandler;
class UI
{
	public:
		UI(Fl_Window* window);
		Fl_Text_Display *output;

		void printmsg(const char *msg);
				
		//Callback functions:
		static void zChanged_static_callback(Fl_Widget *w, void *f);
		void zChanged_callback(Fl_Widget *w);

		static void pButton_static_callback(Fl_Widget *w, void *f);
		void pButton_callback(Fl_Widget *w, MapHandler *m);

		//const values:
		static const int UI_X = 1024;
		static const int UI_Y = 700;


	private:		
		void setupMapTab();
		void setupDataTab();

		MapHandler *maphandler;
		Fl_Window *window;
		//Tabs:
		Fl_Tabs *tabs;
		Fl_Group *mapTab;
		Fl_Group *dataTab;
		
		//Widgets:
		Fl_Hor_Slider *zCounter;
		Fl_Value_Output *zOutput;
		Fl_Counter *mapCounter;
		Fl_Text_Buffer *outputBuffer;
		Fl_Counter *stepSizeCounter;
		Fl_Button *processDataButton;

		Fl_Label *datalabel;
		Fl_Input *datafile;

		ColorMap *colormap;

		int xmax,ymax;
		int ImapAmount;
};

#endif // UI_H

