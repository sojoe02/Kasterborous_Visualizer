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

class UI
{
	public:
		UI(Fl_Window * window);
		Fl_Text_Display *output;
		
		//Callback functions:
		static void zChanged_static_callback(Fl_Widget *w, void *f);
		void zChanged_callback(Fl_Widget *w);

	private:
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


};

#endif // UI_H

