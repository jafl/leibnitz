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

static const JUtf8Byte* kWMClassInstance = "Leibnitz";
static const JUtf8Byte* kExprWindowClass = "Leibnitz_Expression";
static const JUtf8Byte* kPlotWindowClass = "Leibnitz_Plot";
static const JUtf8Byte* kConvWindowClass = "Leibnitz_Base_Conversion";
static const JUtf8Byte* kVarWindowClass  = "Leibnitz_Constants";

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
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew MDIServer;
	assert( theMDIServer != nullptr );

	theTapePrinter = jnew JXPTPrinter;
	assert( theTapePrinter != nullptr );

	JXDisplay* display = app->GetDisplay(1);

	thePSGraphPrinter = jnew JXPSPrinter(display);
	assert( thePSGraphPrinter != nullptr );

	theEPSGraphPrinter = jnew JX2DPlotEPSPrinter(display);
	assert( theEPSGraphPrinter != nullptr );
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
 GetPrefsManager

 ******************************************************************************/

PrefsManager*
GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
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
 GetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
GetWMClassInstance()
{
	return kWMClassInstance;
}

/******************************************************************************
 GetExprWindowClass

 ******************************************************************************/

const JUtf8Byte*
GetExprWindowClass()
{
	return kExprWindowClass;
}

/******************************************************************************
 GetPlotWindowClass

 ******************************************************************************/

const JUtf8Byte*
GetPlotWindowClass()
{
	return kPlotWindowClass;
}

/******************************************************************************
 GetBaseConvWindowClass

 ******************************************************************************/

const JUtf8Byte*
GetBaseConvWindowClass()
{
	return kConvWindowClass;
}

/******************************************************************************
 GetVarWindowClass

 ******************************************************************************/

const JUtf8Byte*
GetVarWindowClass()
{
	return kVarWindowClass;
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
