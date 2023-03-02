/******************************************************************************
 PrefsManager.cpp

	Exists to keep JCheckForNewerVersion() happy.

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "PrefsManager.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

PrefsManager::PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true, JPrefID())
{
	*isNew = JPrefsManager::UpgradeData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PrefsManager::~PrefsManager()
{
	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
PrefsManager::UpgradeData
	(
	const bool			isNew,
	const JFileVersion	currentVersion
	)
{
}
