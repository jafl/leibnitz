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

	auto* window = jnew JXWindow(this, 330,320, JGetString("WindowTitle::Plot2DFunctionDialog::JXLayout"));

	auto* exprEditorSet =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 330,150);
	assert( exprEditorSet != nullptr );

	itsPlotMenu =
		jnew JXTextMenu(JGetString("itsPlotMenu::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 30,170, 260,30);

	auto* curveLabel =
		jnew JXStaticText(JGetString("curveLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,220, 80,20);
	curveLabel->SetToLabel(false);

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,250, 80,20);
	rangeLabel->SetToLabel(false);

	auto* toLabel =
		jnew JXStaticText(JGetString("toLabel::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,250, 20,20);
	toLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,290, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::Plot2DFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 199,289, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::Plot2DFunctionDialog::JXLayout"));

	itsCurveName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 110,220, 180,20);

	itsMinInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,250, 80,20);
	itsMinInput->SetIsRequired(false);

	itsMaxInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 210,250, 80,20);
	itsMaxInput->SetIsRequired(false);

// end JXLayout

	SetButtons(okButton, cancelButton);

	GetApplication()->BuildPlotMenu(itsPlotMenu, prevPlot, &itsPlotIndex);
	itsPlotMenu->SetToPopupChoice(true, itsPlotIndex);

	ListenTo(itsPlotMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsPlotMenu->CheckItem(itsPlotIndex);
	}));

	ListenTo(itsPlotMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsPlotIndex = msg.GetIndex();
	}));

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
