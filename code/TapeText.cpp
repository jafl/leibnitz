/******************************************************************************
 TapeText.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "TapeText.h"
#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TapeText::TapeText
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, jnew JXStyledText(true, false, enclosure->GetFontManager()), true,
			 false, scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TapeText::~TapeText()
{
}
