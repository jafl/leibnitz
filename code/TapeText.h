/******************************************************************************
 TapeText.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_TapeText
#define _H_TapeText

#include <jx-af/jx/JXTEBase.h>

class TapeText : public JXTEBase
{
public:

	TapeText(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TapeText() override;
};

#endif
