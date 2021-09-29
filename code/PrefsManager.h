/******************************************************************************
 PrefsManager.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>

// Preferences -- do not change ID's once they are assigned

enum
{
	kVersionCheckID = 1
};

class PrefsManager : public JXPrefsManager
{
public:

	PrefsManager(bool* isNew);

	virtual	~PrefsManager();

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion);
};

#endif
