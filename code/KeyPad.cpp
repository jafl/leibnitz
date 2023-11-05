/******************************************************************************
 KeyPad.cpp

	BASE CLASS = JXWidgetSet

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "KeyPad.h"
#include "ExprEditor.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

// remember to keep the enum up to date

static const JUtf8Byte kOpKey[] =
{
	'.', 'e', '+', '-', '*', '/', '=', kJEscapeKey
};

/******************************************************************************
 Constructor

 ******************************************************************************/

KeyPad::KeyPad
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsExpr = nullptr;
	CreateButtons();

	SetNeedsInternalFTC();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

KeyPad::~KeyPad()
{
}

/******************************************************************************
 CreateButtons (private)

 ******************************************************************************/

void
KeyPad::CreateButtons()
{
// begin kpLayout

	const JRect kpLayout_Aperture = this->GetAperture();
	this->AdjustSize(60 - kpLayout_Aperture.width(), 160 - kpLayout_Aperture.height());

	itsDigitButton[7] =
		jnew JXTextButton(JGetString("itsDigitButton[7]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 20,20);
	assert( itsDigitButton[7] != nullptr );

	itsDigitButton[8] =
		jnew JXTextButton(JGetString("itsDigitButton[8]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,0, 20,20);
	assert( itsDigitButton[8] != nullptr );

	itsDigitButton[9] =
		jnew JXTextButton(JGetString("itsDigitButton[9]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,0, 20,20);
	assert( itsDigitButton[9] != nullptr );

	itsDigitButton[4] =
		jnew JXTextButton(JGetString("itsDigitButton[4]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,20, 20,20);
	assert( itsDigitButton[4] != nullptr );

	itsDigitButton[5] =
		jnew JXTextButton(JGetString("itsDigitButton[5]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 20,20);
	assert( itsDigitButton[5] != nullptr );

	itsDigitButton[6] =
		jnew JXTextButton(JGetString("itsDigitButton[6]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 20,20);
	assert( itsDigitButton[6] != nullptr );

	itsDigitButton[1] =
		jnew JXTextButton(JGetString("itsDigitButton[1]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,40, 20,20);
	assert( itsDigitButton[1] != nullptr );

	itsDigitButton[2] =
		jnew JXTextButton(JGetString("itsDigitButton[2]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 20,20);
	assert( itsDigitButton[2] != nullptr );

	itsDigitButton[3] =
		jnew JXTextButton(JGetString("itsDigitButton[3]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 20,20);
	assert( itsDigitButton[3] != nullptr );

	itsDigitButton[0] =
		jnew JXTextButton(JGetString("itsDigitButton[0]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,60, 20,20);
	assert( itsDigitButton[0] != nullptr );

	itsOpButton[kPtOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kPtOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 20,20);
	assert( itsOpButton[kPtOpIndex] != nullptr );

	itsOpButton[kExpOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kExpOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,60, 20,20);
	assert( itsOpButton[kExpOpIndex] != nullptr );

	itsClearAllButton =
		jnew JXTextButton(JGetString("itsClearAllButton::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,140, 60,20);
	assert( itsClearAllButton != nullptr );

	itsOpButton[kClearOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kClearOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,120, 60,20);
	assert( itsOpButton[kClearOpIndex] != nullptr );

	itsOpButton[kEvalOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kEvalOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,80, 20,40);
	assert( itsOpButton[kEvalOpIndex] != nullptr );

	itsOpButton[kMultiplyOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kMultiplyOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,100, 20,20);
	assert( itsOpButton[kMultiplyOpIndex] != nullptr );

	itsOpButton[kDivideOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kDivideOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 20,20);
	assert( itsOpButton[kDivideOpIndex] != nullptr );

	itsOpButton[kAddOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kAddOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,80, 20,20);
	assert( itsOpButton[kAddOpIndex] != nullptr );

	itsOpButton[kSubtractOpIndex] =
		jnew JXTextButton(JGetString("itsOpButton[kSubtractOpIndex]::KeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 20,20);
	assert( itsOpButton[kSubtractOpIndex] != nullptr );

	this->SetSize(kpLayout_Aperture.width(), kpLayout_Aperture.height());

// end kpLayout

	itsOpButton[kMultiplyOpIndex]->SetLabel(JGetString("MultiplicationSymbol::KeyPad"));
	itsOpButton[kDivideOpIndex]->SetLabel(JGetString("DivisionSymbol::KeyPad"));

	for (auto* button : itsDigitButton)
	{
		ListenTo(button);
	}

	for (auto* button : itsOpButton)
	{
		ListenTo(button);
	}

	ListenTo(itsClearAllButton);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
KeyPad::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JXButton::kPushed) && HandleButton(sender))
	{
		// work is done by function
	}
	else
	{
		JXWidgetSet::Receive(sender, message);
	}
}

/******************************************************************************
 HandleButton (private)

 ******************************************************************************/

bool
KeyPad::HandleButton
	(
	JBroadcaster* sender
	)
	const
{
JXKeyModifiers modifiers(GetDisplay());

	if (itsExpr == nullptr || !itsExpr->Focus())
	{
		return false;
	}

	for (JUnsignedOffset i=0; i<10; i++)
	{
		if (sender == itsDigitButton[i])
		{
			if (!itsExpr->HasSelection() && !itsExpr->UIFIsActive())
			{
				if (itsExpr->ContainsUIF())
				{
					itsExpr->ActivateNextUIF();
				}
				else
				{
					itsExpr->SelectAll();
				}
			}

			itsExpr->HandleKeyPress(JUtf8Character('0' + i), 0, modifiers);
			return true;
		}
	}

	for (JUnsignedOffset i=0; i<kOpCount; i++)
	{
		if (sender == itsOpButton[i])
		{
			if (!itsExpr->HasSelection() && !itsExpr->UIFIsActive())
			{
				itsExpr->SelectAll();
			}

			itsExpr->HandleKeyPress(JUtf8Character(kOpKey[i]), 0, modifiers);
			return true;
		}
	}

	if (sender == itsClearAllButton)
	{
		itsExpr->CancelEditing();
		itsExpr->SelectAll();
		itsExpr->DeleteSelection();
		return true;
	}

	return false;
}
