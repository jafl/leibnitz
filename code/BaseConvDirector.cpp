/******************************************************************************
 BaseConvDirector.cpp

	Window to convert non-negative integers from one base to another.
	Since we only support four bases, we list all of them.

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "BaseConvDirector.h"
#include "globals.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

static const JString::Base kBase[] =
{
	JString::kBase2, JString::kBase8, JString::kBase10, JString::kBase16
};

/******************************************************************************
 Constructor

 ******************************************************************************/

BaseConvDirector::BaseConvDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	itsIgnoreTextFlag(false)
{
	BuildWindow();
}

BaseConvDirector::BaseConvDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXDirector*			supervisor
	)
	:
	JXWindowDirector(supervisor),
	itsIgnoreTextFlag(false)
{
	BuildWindow();

	if (vers <= 8)
	{
		JSize fromBase, toBase;
		input >> fromBase >> toBase;

		JString fromValue;
		input >> fromValue;

		switch (fromBase)
		{
			case  2:  its2Input->GetText()->SetText(fromValue); break;
			case  8:  its8Input->GetText()->SetText(fromValue); break;
			case 10: its10Input->GetText()->SetText(fromValue); break;
			case 16: its16Input->GetText()->SetText(fromValue); break;
		}
	}
	else
	{
		JString s;
		input >> s;
		its10Input->GetText()->SetText(s);
	}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);

	bool active;
	input >> JBoolFromString(active);
	if (active)
	{
		Activate();
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BaseConvDirector::~BaseConvDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
BaseConvDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	output << its10Input->GetText()->GetText();

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << JBoolToString(IsActive());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_base_conv_window.xpm"

void
BaseConvDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 180,160, JGetString("WindowTitle::BaseConvDirector::JXLayout"));
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Leibnitz_Base_Conversion");

	auto* base10Label =
		jnew JXStaticText(JGetString("base10Label::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 60,20);
	base10Label->SetToLabel(false);

	auto* base2Label =
		jnew JXStaticText(JGetString("base2Label::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 60,20);
	base2Label->SetToLabel(false);

	auto* base8Label =
		jnew JXStaticText(JGetString("base8Label::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 60,20);
	base8Label->SetToLabel(false);

	auto* base16Label =
		jnew JXStaticText(JGetString("base16Label::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 60,20);
	base16Label->SetToLabel(false);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::BaseConvDirector::JXLayout"));

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::BaseConvDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 100,130, 60,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::BaseConvDirector::JXLayout"));

	its10Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,20, 80,20);

	its2Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,50, 80,20);

	its8Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,70, 80,20);

	its16Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,90, 80,20);

// end JXLayout

	window->LockCurrentMinSize();
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	window->PlaceAsDialogWindow();

	JXDisplay* display = GetDisplay();
	auto* icon         = jnew JXImage(display, thx_base_conv_window);
	window->SetIcon(icon);

	ListenTo(its2Input);
	ListenTo(its8Input);
	ListenTo(its10Input);
	ListenTo(its16Input);

	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);

	// remember to update kBase[]

	itsInput[0] = its2Input;
	itsInput[1] = its8Input;
	itsInput[2] = its10Input;
	itsInput[3] = its16Input;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
BaseConvDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		GetWindow()->Close();
	}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		(JXGetHelpManager())->ShowSection("BaseConvHelp");
	}

	else
	{
		if (!itsIgnoreTextFlag &&
			(message.Is(JStyledText::kTextSet) ||
			 message.Is(JStyledText::kTextChanged)))
		{
			itsIgnoreTextFlag = true;
			Convert(sender);
			itsIgnoreTextFlag = false;
		}

		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 Convert (private)

 ******************************************************************************/

void
BaseConvDirector::Convert
	(
	JBroadcaster* sender
	)
{
	JXInputField* input = nullptr;
	JSize base          = 0;

	for (JUnsignedOffset i=0; i<kBaseCount; i++)
	{
		if (sender == itsInput[i])
		{
			input = itsInput[i];
			base  = kBase[i];
			break;
		}
	}

	if (input == nullptr)
	{
		return;
	}

	JUInt value;
	if (!input->GetText()->GetText().ConvertToUInt(&value, base))
	{
		for (JUnsignedOffset i=0; i<kBaseCount; i++)
		{
			if (itsInput[i] != input)
			{
				itsInput[i]->GetText()->SetText(JString::empty);
			}
		}
		return;
	}

	for (JUnsignedOffset i=0; i<kBaseCount; i++)
	{
		if (itsInput[i] != input)
		{
			const JString s(value, kBase[i], true);
			itsInput[i]->GetText()->SetText(s);
		}
	}
}
