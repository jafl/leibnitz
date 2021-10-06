/******************************************************************************
 MDIServer.h

	Interface for the MDIServer class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>

class MDIServer : public JXMDIServer
{
public:

	MDIServer();

	virtual ~MDIServer();

	static void	PrintCommandLineHelp();

protected:

	void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;
};

#endif
