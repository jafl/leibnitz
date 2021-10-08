/******************************************************************************
 ExprEditor.h

	Interface for the ExprEditor class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExprEditor
#define _H_ExprEditor

#include <jx-af/jexpr/JXExprEditor.h>

class JXTEBase;
class VarList;

class ExprEditor : public JXExprEditor
{
public:

	ExprEditor(const VarList* varList, JXMenuBar* menuBar,
				  JXTEBase* tapeWidget,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~ExprEditor() override;

	virtual void	EvaluateSelection() const;

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	JXTEBase*	itsTapeWidget;	// not owned
};

#endif
