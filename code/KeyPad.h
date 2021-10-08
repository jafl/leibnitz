/******************************************************************************
 KeyPad.h

	Interface for the KeyPad class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_KeyPad
#define _H_KeyPad

#include <jx-af/jx/JXWidgetSet.h>

class JXTextButton;
class ExprEditor;

class KeyPad : public JXWidgetSet
{
public:

	KeyPad(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~KeyPad() override;

	void	SetExprEditor(ExprEditor* expr);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

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

	ExprEditor*	itsExpr;				// not owned
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
KeyPad::SetExprEditor
	(
	ExprEditor* expr
	)
{
	itsExpr = expr;
}

#endif
