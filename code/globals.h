/******************************************************************************
 globals.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_globals
#define _H_globals

// we include these for convenience

#include <jx-af/jx/jXGlobals.h>
#include "App.h"

class MDIServer;
class PrefsManager;
class JXPTPrinter;
class JXPSPrinter;
class JX2DPlotEPSPrinter;

App*				GetApplication();
MDIServer*			GetMDIServer();
bool				HasPrefsManager();
PrefsManager*		GetPrefsManager();
void				ForgetPrefsManager();
JXPTPrinter*		GetTapePrinter();
JXPSPrinter*		GetPSGraphPrinter();
JX2DPlotEPSPrinter*	GetEPSGraphPrinter();

const JString&		GetVersionNumberStr();
JString				GetVersionStr();

	// called by App

void CreateGlobals(App* app);
void DeleteGlobals();

#endif
