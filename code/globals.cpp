/******************************************************************************
 globals.cpp

	Access to global objects and factories.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "globals.h"
#include "MDIServer.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jcore/jAssert.h>

static App*					theApplication     = nullptr;	// owns itself
static MDIServer*			theMDIServer       = nullptr;	// owned by JX
static PrefsManager*		thePrefsManager    = nullptr;
static JXPTPrinter*			theTapePrinter     = nullptr;
static JXPSPrinter*			thePSGraphPrinter  = nullptr;
static JX2DPlotEPSPrinter*	theEPSGraphPrinter = nullptr;

/******************************************************************************
 CreateGlobals

 ******************************************************************************/

void
CreateGlobals
	(
	App* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew PrefsManager(&isNew);

	JXInitHelp();

	theMDIServer = jnew MDIServer;

	theTapePrinter = jnew JXPTPrinter;

	JXDisplay* display = app->GetDisplay(1);

	thePSGraphPrinter = jnew JXPSPrinter(display);

	theEPSGraphPrinter = jnew JX2DPlotEPSPrinter(display);
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	jdelete theTapePrinter;
	theTapePrinter = nullptr;

	jdelete thePSGraphPrinter;
	thePSGraphPrinter = nullptr;

	jdelete theEPSGraphPrinter;
	theEPSGraphPrinter = nullptr;

	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 GetApplication

 ******************************************************************************/

App*
GetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 HasPrefsManager

 ******************************************************************************/

bool
HasPrefsManager()
{
	return thePrefsManager != nullptr;
}

/******************************************************************************
 GetPrefsManager

 ******************************************************************************/

PrefsManager*
GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 ForgetPrefsManager

	Called when license is not accepted, to avoid writing prefs file.

 ******************************************************************************/

void
ForgetPrefsManager()
{
	thePrefsManager = nullptr;
}

/******************************************************************************
 GetMDIServer

 ******************************************************************************/

MDIServer*
GetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 GetPSGraphPrinter

 ******************************************************************************/

JXPSPrinter*
GetPSGraphPrinter()
{
	assert( thePSGraphPrinter != nullptr );
	return thePSGraphPrinter;
}

/******************************************************************************
 GetEPSGraphPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
GetEPSGraphPrinter()
{
	assert( theEPSGraphPrinter != nullptr );
	return theEPSGraphPrinter;
}

/******************************************************************************
 GetTapePrinter

 ******************************************************************************/

JXPTPrinter*
GetTapePrinter()
{
	assert( theTapePrinter != nullptr );
	return theTapePrinter;
}

/******************************************************************************
 GetVersionNumberStr

 ******************************************************************************/

const JString&
GetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GetVersionStr

 ******************************************************************************/

JString
GetVersionStr()
{
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	return JGetString("Description::globals", map, sizeof(map));
}
