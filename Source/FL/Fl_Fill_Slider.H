//
// "$Id: Fl_Fill_Slider.H 9127 2011-10-04 16:42:53Z greg.ercolano $"
//
// Filled slider header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

/* \file
   Fl_Fill_Slider widget . */

#ifndef Fl_Fill_Slider_H
#define Fl_Fill_Slider_H

#include "Fl_Slider.H"
/** Widget that draws a filled horizontal  slider,  useful as a progress or value meter*/
class FL_EXPORT Fl_Fill_Slider : public Fl_Slider {
public:
    /** Creates the slider from its position,size and optional title. */
    Fl_Fill_Slider(int X,int Y,int W,int H,const char *L=0)
	: Fl_Slider(X,Y,W,H,L) {type(FL_VERT_FILL_SLIDER);}
};

#endif

//
// End of "$Id: Fl_Fill_Slider.H 9127 2011-10-04 16:42:53Z greg.ercolano $".
//
