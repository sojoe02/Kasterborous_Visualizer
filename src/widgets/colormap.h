#ifndef COLORMAP_H
#define COLORMAP_H

#include <FL/Fl.H>
#include <FL/fl_draw.H>

class ColorMap : public Fl_Widget
{
	public:
		ColorMap(int X, int Y, int W, int H, const char *L=0);
		void draw();

	private:
		
		
};
#endif // COLORMAP_H
