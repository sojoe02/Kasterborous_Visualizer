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
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Progress.H>

#include <string>
#include <mutex>

#include "maphandler.h"
#include "colormap.h"

class Utility;
class MapHandler;
class UI
{
	public:
		UI(Fl_Window* window);
		Fl_Text_Display *output;

		void printmsg(const char *msg,...);
		void setProgressMinMax(int min, int max);
		void incrementProgress(double value);
		void resetProgress(){progress->value(0);}
				
		//Callback functions:
		static void zChanged_static_callback(Fl_Widget *w, void *f);
		void zChanged_callback(Fl_Widget *w);

		static void pButton_static_callback(Fl_Widget *w, void *f){
			((UI *)f)->pButton_callback(w, ((UI *)f)->maphandler);}
		void pButton_callback(Fl_Widget *w, MapHandler *m);

		static void mapCounter_static_callback(Fl_Widget *w, void *f);
		void mapCounter_callback(Fl_Widget *w);

		static void showLocation_static_callback(Fl_Widget *w, void *f){
			((UI*)f)->showLocation_callback(w);}
		void showLocation_callback(Fl_Widget *w);

		static void showGrid_static_callback(Fl_Widget *w, void *f){
			((UI*)f)->showGrid_callback(w);}
		void showGrid_callback(Fl_Widget *w);

		static void resSlide_static_callback(Fl_Widget *w, void *f){
			((UI*)f)->resSlide_callback(w);}
		void resSlide_callback(Fl_Widget *w);

		static void showFreq_static_callback(Fl_Widget *w, void *f){
			((UI *)f)->showFreq_callback(w);}
		void showFreq_callback(Fl_Widget *w);

		static void showCumulative_static_callback(Fl_Widget *w, void *f){
			((UI *)f)->showCumulative_callback(w);}
		void showCumulative_callback(Fl_Widget *w);

		static void showAverage_static_callback(Fl_Widget *w, void *f){
			((UI *)f)->showAverage_callback(w);}
		void showAverage_callback(Fl_Widget *w);

		static void calculateIButton_static_callback(Fl_Widget *w, void *f){
			((UI *)f)->calculateIButton_callback(w);}
		void calculateIButton_callback(Fl_Widget *w);


		//const values:
		static const int UI_X = 1024;
		static const int UI_Y = 700;


	private:		
		void setupMapTab();
		void setupDataTab();
		void redrawMap();

		MapHandler *maphandler;
		Fl_Window *window;
		//Tabs:
		Fl_Tabs *tabs;
		Fl_Group *mapTab;
		Fl_Group *dataTab;

		//Widgets:
		//Data process widgets:
		Fl_Hor_Slider *zCounter;
		Fl_Value_Output *zOutput;
		Fl_Text_Buffer *outputBuffer;
		Fl_Counter *stepSizeCounter;
		Fl_Button *processDataButton;
		Fl_Button *calculateIButton;
		Fl_Hor_Slider *resSlide;
		Fl_Value_Output *resOutput;
		Fl_Progress *progress;

		//Map Widgets:
		Fl_Check_Button *showCumulative;
		Fl_Check_Button *showAverage;
		Fl_Check_Button *showFreq;
		Fl_Check_Button *showLocation; /*<! check button to enable or disable event origin drawing  */
		Fl_Check_Button *showGrid; /*<! check button to enable sector grid in map drawing*/
		Fl_Counter *mapCounter; /*<! counter for controlling currently active map*/
		//Fl_Label *datalabel; /*<! */
		Fl_Input *datafile;
		ColorMap *colormap;

		int xmax,ymax;
		int ImapAmount;

		std::mutex printMutex;
};

#endif // UI_H

