/******************************************************************************
 MDIServer.cpp

	BASE CLASS = JXMDIServer

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "MDIServer.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDIServer::MDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDIServer::~MDIServer()
{
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 ******************************************************************************/

void
MDIServer::HandleMDIRequest
	(
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	((GetApplication())->GetCurrentDisplay())->RaiseAllWindows();
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
MDIServer::PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
	{
		"vers", GetVersionNumberStr().GetBytes()
	};
	JString s = JGetString("CommandLineHelp::MDIServer");
	JGetStringManager()->Replace(&s, map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
