/******************************************************************************
 Plot2DFunctionDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "Plot2DFunctionDialog.h"
#include "VarList.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jexpr/JXExprEditorSet.h>
#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	prevPlot can be nullptr.

 ******************************************************************************/

Plot2DFunctionDialog::Plot2DFunctionDialog
	(
	const VarList*			varList,
	const Plot2DDirector*	prevPlot
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(varList, prevPlot);
}

Plot2DFunctionDialog::Plot2DFunctionDialog
	(
	const VarList*		varList,
	const JFunction&	f,
	const JString&		curveName,
	const JFloat		min,
	const JFloat		max
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(varList, nullptr);

	itsExprWidget->SetFunction(varList, f.Copy());
	itsPlotMenu->Hide();
	itsCurveName->GetText()->SetText(curveName);
	itsMinInput->SetValue(min);
	itsMaxInput->SetValue(max);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Plot2DFunctionDialog::~Plot2DFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
Plot2DFunctionDialog::BuildWindow
	(
	const VarList*			varList,
	const Plot2DDirector*	prevPlot
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,320, JString::empty);
	assert( window != nullptr );

	auto* exprEditorSet =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 330,150);
	assert( exprEditorSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,290, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,290, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::Plot2DFunctionDialog::shortcuts::JXLayout"));

	auto* curveLabel =
		jnew JXStaticText(JGetString("curveLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,220, 80,20);
	assert( curveLabel != nullptr );
	curveLabel->SetToLabel();

	itsCurveName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 110,220, 180,20);
	assert( itsCurveName != nullptr );

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,250, 80,20);
	assert( rangeLabel != nullptr );
	rangeLabel->SetToLabel();

	itsMinInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,250, 80,20);
	assert( itsMinInput != nullptr );

	auto* toLabel =
		jnew JXStaticText(JGetString("toLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,250, 20,20);
	assert( toLabel != nullptr );
	toLabel->SetToLabel();

	itsMaxInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 210,250, 80,20);
	assert( itsMaxInput != nullptr );

	itsPlotMenu =
		jnew JXTextMenu(JGetString("itsPlotMenu::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 30,170, 260,30);
	assert( itsPlotMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::Plot2DFunctionDialog"));
	SetButtons(okButton, cancelButton);

	(GetApplication())->BuildPlotMenu(itsPlotMenu, prevPlot, &itsPlotIndex);
	itsPlotMenu->SetToPopupChoice(true, itsPlotIndex);
	ListenTo(itsPlotMenu);

	itsCurveName->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsMinInput->SetValue(0.0);
	itsMinInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsMaxInput->SetValue(1.0);
	itsMaxInput->ShareEditMenu(itsExprWidget->GetEditMenu());
}

/******************************************************************************
 GetSettings

 ******************************************************************************/

void
Plot2DFunctionDialog::GetSettings
	(
	JIndex*				plotIndex,
	const JFunction**	f,
	JString*			curveName,
	JFloat*				fMin,
	JFloat*				fMax
	)
	const
{
	*plotIndex = itsPlotIndex;
	*f         = itsExprWidget->GetFunction();
	*curveName = itsCurveName->GetText()->GetText();

	if (curveName->IsEmpty())
	{
		*curveName = (**f).Print();
	}

	JFloat min, max;

	bool ok = itsMinInput->GetValue(&min);
	assert( ok );

	ok = itsMaxInput->GetValue(&max);
	assert( ok );

	*fMin = JMin(min, max);
	*fMax = JMax(min, max);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
Plot2DFunctionDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlotMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		itsPlotMenu->CheckItem(itsPlotIndex);
	}
	else if (sender == itsPlotMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsPlotIndex = selection->GetIndex();
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
Plot2DFunctionDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	else if (itsExprWidget->ContainsUIF())
	{
		JGetUserNotification()->ReportError(JGetString("FinishFunction::Plot2DFunctionDialog"));
		itsExprWidget->Focus();
		return false;
	}
	else
	{
		return true;
	}
}
