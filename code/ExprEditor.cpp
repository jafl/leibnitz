/******************************************************************************
 ExprEditor.cpp

	BASE CLASS = JXExprEditor

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "ExprEditor.h"
#include "VarList.h"
#include <jx-af/jx/JXTEBase.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExprEditor::ExprEditor
	(
	const VarList*		varList,
	JXMenuBar*			menuBar,
	JXTEBase*			tapeWidget,
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
	JXExprEditor(varList, menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	itsTapeWidget = tapeWidget;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExprEditor::~ExprEditor()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
ExprEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == '=')
	{
		JIndex selection;
		const bool hadSelection = GetSelection(&selection);
		if (hadSelection)
		{
			ClearSelection();
		}
		else if (!EndEditing())
		{
			return;
		}
		EvaluateSelection();
		if (hadSelection)
		{
			SetSelection(selection);
		}
	}
	else
	{
		JXExprEditor::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 EvaluateSelection (virtual)

 ******************************************************************************/

void
ExprEditor::EvaluateSelection()
	const
{
	const JFunction* f;
	if (!GetConstSelectedFunction(&f))
	{
		f = GetFunction();
	}

	JComplex value;
	JString valueStr;
	if (f->Evaluate(&value))
	{
		valueStr = JPrintComplexNumber(value);
	}
	else
	{
		valueStr = "Error";
	}

	const JSize textLength = itsTapeWidget->GetText()->GetText().GetCharacterCount();

	JString newText = f->Print();
	if (textLength > 0)
	{
		newText.Prepend("\n\n");
	}
	newText.Append("\n");
	newText.Append(valueStr);

	itsTapeWidget->SetCaretLocation(textLength+1);
	itsTapeWidget->Paste(newText);
}
