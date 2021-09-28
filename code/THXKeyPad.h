/******************************************************************************
 THXKeyPad.h

	Interface for the THXKeyPad class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXKeyPad
#define _H_THXKeyPad

#include <jx-af/jx/JXWidgetSet.h>

class JXTextButton;
class THXExprEditor;

class THXKeyPad : public JXWidgetSet
{
public:

	THXKeyPad(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	virtual ~THXKeyPad();

	void	SetExprEditor(THXExprEditor* expr);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// remember to keep the op key array up to date

	enum
	{
		kPtOpIndex,
		kExpOpIndex,
		kAddOpIndex,
		kSubtractOpIndex,
		kMultiplyOpIndex,
		kDivideOpIndex,
		kEvalOpIndex,
		kClearOpIndex,

		kOpCount
	};

private:

	THXExprEditor*	itsExpr;				// not owned
	JXTextButton*	itsDigitButton[10];
	JXTextButton*	itsOpButton [ kOpCount ];

// begin kpLayout

	JXTextButton* itsClearAllButton;

// end kpLayout

private:

	void	CreateButtons();
	bool	HandleButton(JBroadcaster* sender) const;
};


/******************************************************************************
 SetExprEditor

 ******************************************************************************/

inline void
THXKeyPad::SetExprEditor
	(
	THXExprEditor* expr
	)
{
	itsExpr = expr;
}

#endif
