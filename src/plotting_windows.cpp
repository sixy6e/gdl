/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "plotting.hpp"

namespace lib {

  using namespace std;

  void window( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current "
		"graphics device.");

    SizeT nParam=e->NParam();

    DLong wIx = 0;
    if( e->KeywordSet( 1)) // FREE
      {
	wIx = actDevice->WAdd();
	if( wIx == -1)
	  e->Throw( "No more window handles left.");
      }
    else
      {
	if( nParam == 1)
	  {
	    e->AssureLongScalarPar( 0, wIx);
	    if( wIx < 0 || wIx >= maxWin)
	      e->Throw( "Window number "+i2s(wIx)+
			" out of range.");
	  }
      }

    DString title;
    if( e->KeywordPresent( 4)) // TITLE
      {
	e->AssureStringScalarKWIfPresent( 4, title);
      }
    else
      {
	title = "GDL "+i2s( wIx);
      }

    DLong xPos = 0;
    if( e->KeywordPresent( 5)) // XPOS
	e->AssureLongScalarKW( 5, xPos);
    DLong yPos = 0;
    if( e->KeywordPresent( 6)) // YPOS
	e->AssureLongScalarKW( 6, yPos);

    DLong xSize = 640;
    e->AssureLongScalarKWIfPresent( "XSIZE", xSize);
    DLong ySize = 512;
    e->AssureLongScalarKWIfPresent( "YSIZE", ySize);

    if( xSize <= 0 || ySize <= 0 || xPos < 0 || yPos < 0)
      e->Throw(  "Unable to create window "
		 "(BadValue (integer parameter out of range for "
		 "operation)).");
    
    bool success = actDevice->WOpen( wIx, title, xSize, ySize, xPos, yPos);
    if( !success)
      e->Throw(  "Unable to create window.");
  }

  void wset( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if( nParam != 0)
      {
	e->AssureLongScalarPar( 0, wIx);
      }
    if( wIx == -1) wIx = actDevice->ActWin();
    if( wIx == -1) 
      e->Throw( "Window is closed and unavailable.");

    if( wIx == 0)
      {
	if( actDevice->ActWin() == -1)
	  {
	    bool success = actDevice->WOpen( 0, "GDL 0", 640, 512, 0, 0);
	    if( !success)
	      e->Throw( "Unable to create window.");
	    return;
	  }
      }

    bool success = actDevice->WSet( wIx);
    if( !success)
      e->Throw( "Window is closed and unavailable.");
  }

  void wshow( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if (nParam != 0) e->AssureLongScalarPar( 0, wIx);
    else wIx = actDevice->ActWin();
    bool show = true;
    if (nParam == 2) { 
      DIntGDL *showval = e->GetParAs<DIntGDL>(1);
      show = (*showval)[0] != 0;
    }
    // TODO: iconify
    if (!actDevice->WShow( wIx, show, false)) 
      e->Throw( "Window is closed and unavailable.");
  }

  void wdelete( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    if( nParam == 0)
      {
	DLong wIx = actDevice->ActWin();
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
			      " out of range or no more windows.");
	return;
      }

    for( SizeT i=0; i<nParam; i++)
      {
	DLong wIx;
	e->AssureLongScalarPar( i, wIx);
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
		    " out of range or no more windows.");
      }
  }

  // this code is issued from
  // http://users.actcom.co.il/~choo/lupg/tutorials/xlib-programming/xlib-programming.html#display_info
  // http://wwwcgi.rdg.ac.uk:8081/cgi-bin/cgiwrap/wsi14/poplog/pop11/ref/XHeightMMOfScreen 
  BaseGDL* get_screen_size( EnvT* e)
  {
#ifndef HAVE_X
    e->Throw("GDL was compiled without support for X-windows");
#else
    SizeT nParam=e->NParam(); 
    
    if ( nParam > 1) e->Throw( "Incorrect number of arguments.");
    char *TheDisplay=NULL;

    if ( nParam == 1) {
      DString GivenDisplay;
      //Message( "Warning, we don't know exactly how to manage DISPLAY now.");
      // this code was tested successfully on Ubuntu and OSX, through network
      // and also with "strange" like 'localhost:0.0', '0:0' and "/tmp/launch-OfEkVY/:0" (OSX)
      e->AssureStringScalarPar(0, GivenDisplay);
      TheDisplay = new char [GivenDisplay.size()+1];
      strcpy (TheDisplay, GivenDisplay.c_str());
    }

    /* open the connection to the display */
    Display* display;
    display = XOpenDisplay(TheDisplay);
    if (display == NULL)
      e->Throw("Cannot connect to X server");//, TheDisplay.c_str());
    
    int screen_num;
    
    /* these variables will store the size of the screen, in pixels.    */
    int screen_width;
    int screen_height;
   
    /* this variable will be used to store the ID of the root window of our */
    /* screen. Each screen always has a root window that covers the whole   */
    /* screen, and always exists.                                           */
    Window root_window;
    
    /* check the number of the default screen for our X server. */
    screen_num = DefaultScreen(display);
    
    /* find the width of the default screen of our X server, in pixels. */
    screen_width = DisplayWidth(display, screen_num);
   
    /* find the height of the default screen of our X server, in pixels. */
    screen_height = DisplayHeight(display, screen_num);

    int debug=0;
    if (debug ==1) {
      cout << "Nb pixels: " << screen_width << " x " << screen_height << endl;
    }

    static int resolutionIx = e->KeywordIx( "RESOLUTION");
    if( e->KeywordPresent( resolutionIx)) {
      int screen_width_mm;
      int screen_height_mm;
      screen_width_mm = DisplayWidthMM(display, screen_num);
      screen_height_mm = DisplayHeightMM(display, screen_num);
      DDoubleGDL* resolution;
      resolution = new DDoubleGDL(2, BaseGDL::NOZERO);
      (*resolution)[0]=(screen_width_mm/10.)/screen_width;
      (*resolution)[1]=(screen_height_mm/10.)/screen_height;
      e->SetKW(0, resolution);
      if (debug ==1) {
	cout << "screen (mm): " << screen_width_mm << " x " << screen_height_mm << endl;
	cout << "pixels (mm): " << (*resolution)[0] << " x " << (*resolution)[1] << endl;
      }
    }
    
    XCloseDisplay(display);
    DDoubleGDL* res;
    res = new DDoubleGDL(2, BaseGDL::NOZERO);
    (*res)[0]=screen_width;
    (*res)[1]=screen_height;
    return res->Convert2(FLOAT, BaseGDL::CONVERT);
#endif
  }

} // namespace