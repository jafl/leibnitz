/******************************************************************************
 THXTapeText.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXTapeText
#define _H_THXTapeText

#include <JXTEBase.h>

class THXTapeText : public JXTEBase
{
public:

	THXTapeText(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~THXTapeText();
};

#endif
