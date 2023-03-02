/******************************************************************************
 fileVersions.h

	Version information for unstructured data files

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_fileVersions
#define _H_fileVersions

#include <jx-af/jcore/jTypes.h>

const JFileVersion kCurrentStateVersion = 12;

// version 12:
//	Upgraded to JXCSFDialogBase::GetState().
// version 11:
//	Discarded 3DPlotDirector state.
// version 10:
//	Stores 3DPlotDirector state.
// version 9:
//	Converted BaseConvDirector from 2 inputs + menus to 4 inputs.
// version 8:
//	JXHelpManager moved to JX shared prefs.
// version 7:
//	App stores EPS graph printer setup.
// version 6:
//	JX2DPlotWidget version number increased.
//	Plot2DDirector print name moved to JX2DPlotWidget.
// version 5:
//	Stores JXChooseSaveFile setup.
// version 4:
//	ExprDirector and Plot2DDirector store print name.
//	JXHelpManager setup version incremented.
// version 3:
//	App stores PS graph printer setup.
// version 2:
//	App stores expression window geometry and JXHelpManager setup.
// version 1:
//	App stores visibility of keypad.

#endif
