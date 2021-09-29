/******************************************************************************
 Plot2DDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "Plot2DDirector.h"
#include "Plot2DFunctionDialog.h"
#include "VarList.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/jXActionDefs.h>
#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotFunction.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
	"    New expression      %k Meta-N"
	"  | Edit constants      %k Meta-E"
	"  | Plot 2D function... %k Meta-Shift-P"
	"  | Base conversion...  %k Meta-B"
	"%l| PostScript page setup..."
	"  | Print PostScript... %k Meta-P"
	"%l| Print plot as EPS..."
	"  | Print marks as EPS..."
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewExprCmd = 1,
	kEditConstCmd,
	kNew2DPlotCmd,
	kConvBaseCmd,
	kPSPageSetupCmd, kPrintPSCmd,
	kPrintPlotEPSCmd, kPrintMarksEPSCmd,
	kCloseWindowCmd, kQuitCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

Plot2DDirector::Plot2DDirector
	(
	JXDirector*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	Plot2DDirectorX();
	BuildWindow();
}

Plot2DDirector::Plot2DDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	VarList*			varList
	)
	:
	JXWindowDirector(supervisor)
{
	Plot2DDirectorX();

	BuildWindow();
	GetWindow()->ReadGeometry(input);

	JSize fnCount;
	input >> fnCount;

	JExprParser p(varList);
	JString expr;

	for (JIndex i=1; i<=fnCount; i++)
	{
		JFloat xMin, xMax;
		input >> xMin >> xMax;

		input >> expr;

		JFunction* f;
		const bool ok = p.Parse(expr, &f);
		assert( ok );

		AddFunction(varList, f, JString::empty, xMin, xMax);
	}

	itsPlotWidget->PWXReadSetup(input);
	itsPlotWidget->PWReadCurveSetup(input);

	if (4 <= vers && vers <= 5)
	{
		JString s;
		input >> s;
		itsPlotWidget->SetPSPrintFileName(s);
	}
}

// private

void
Plot2DDirector::Plot2DDirectorX()
{
	itsFnList = jnew JPtrArray<J2DPlotFunction>(JPtrArrayT::kForgetAll);
	assert( itsFnList != nullptr );

	itsEditFnDialog = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Plot2DDirector::~Plot2DDirector()
{
	jdelete itsFnList;	// contents not owned
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
Plot2DDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	const JSize fnCount = itsPlotWidget->GetCurveCount();
	output << ' ' << fnCount;

	for (JIndex i=1; i<=fnCount; i++)
	{
		const J2DPlotDataBase& data      = itsPlotWidget->GetCurve(i);
		const auto* fnData = dynamic_cast<const J2DPlotFunction*>(&data);
		assert( fnData != nullptr );

		JFloat xMin, xMax;
		fnData->GetXRange(&xMin, &xMax);
		output << ' ' << xMin << ' ' << xMax;

		output << ' ' << fnData->GetFunction().Print();
	}

	output << ' ';
	itsPlotWidget->PWXWriteSetup(output);

	output << ' ';
	itsPlotWidget->PWWriteCurveSetup(output);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_2D_plot_window.xpm"

void
Plot2DDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 600,400, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != nullptr );

	itsPlotWidget =
		jnew JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);
	assert( itsPlotWidget != nullptr );

// end JXLayout

	window->SetMinSize(300, 200);
	window->SetWMClass(GetWMClassInstance(), GetPlotWindowClass());

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, thx_2D_plot_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsPlotWidget->SetPSPrinter(GetPSGraphPrinter());
	itsPlotWidget->SetEPSPrinter(GetEPSGraphPrinter());

	ListenTo(itsPlotWidget);

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::globals"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	JXTextMenu* optionsMenu               = itsPlotWidget->GetOptionsMenu();
	const JIndex editFunctionSubmenuIndex = optionsMenu->GetItemCount()+1;
	optionsMenu->AppendItem(JGetString("EditFunctionItem::Plot2DDirector"));

	itsEditFnMenu = jnew JXTextMenu(optionsMenu, editFunctionSubmenuIndex, menuBar);
	assert( itsEditFnMenu != nullptr );
	itsEditFnMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditFnMenu);

	itsHelpMenu = (GetApplication())->CreateHelpMenu(menuBar, "Plot2DDirector");
	ListenTo(itsHelpMenu);

	JXTextMenu* curveOptionsMenu = itsPlotWidget->GetCurveOptionsMenu();
	itsEditFunctionItemIndex     = curveOptionsMenu->GetItemCount()+1;
	curveOptionsMenu->ShowSeparatorAfter(itsEditFunctionItemIndex-1);
	curveOptionsMenu->AppendItem(JGetString("EditFunctionItem::Plot2DDirector"));
	ListenTo(curveOptionsMenu);

	// do this after everything is constructed so Receive() doesn't crash

	itsPlotWidget->SetTitle(JGetString("PlotTitle::Plot2DDirector"));
}

/******************************************************************************
 AddFunction

 ******************************************************************************/

void
Plot2DDirector::AddFunction
	(
	VarList*			varList,
	const JFunction&	f,
	const JString&		name,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	AddFunction(varList, f.Copy(), name, xMin, xMax);
}

/******************************************************************************
 AddFunction (private)

	We take ownership of the function.

 ******************************************************************************/

void
Plot2DDirector::AddFunction
	(
	VarList*		varList,
	JFunction*		f,
	const JString&	name,
	const JFloat	xMin,
	const JFloat	xMax
	)
{
	auto* data =
		jnew J2DPlotFunction(itsPlotWidget, varList, f, true,
							 VarList::kXIndex, xMin, xMax);
	assert( data != nullptr );

	itsPlotWidget->AddCurve(data, true, name);

	itsFnList->Append(data);
	ListenTo(data);
	UpdateEditFnMenu();
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
Plot2DDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	const JSize count = itsFnList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (sender == itsFnList->GetElement(i))
		{
			itsFnList->RemoveElement(i);
			break;
		}
	}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Plot2DDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlotWidget && message.Is(J2DPlotWidget::kTitleChanged))
	{
		GetWindow()->SetTitle(itsPlotWidget->GetTitle());
	}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionsMenu();
	}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
	}

	else if (sender == itsEditFnMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateEditFnMenu();
	}
	else if (sender == itsEditFnMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEditFnMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		(GetApplication())->UpdateHelpMenu(itsHelpMenu);
	}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		(GetApplication())->HandleHelpMenu(itsHelpMenu, "2DPlotHelp",
											  selection->GetIndex());
	}

	else if (sender == itsPlotWidget->GetCurveOptionsMenu() &&
			 message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateCurveOptionsMenu();
	}
	else if (sender == itsPlotWidget->GetCurveOptionsMenu() &&
			 message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCurveOptionsMenu(selection->GetIndex());
	}

	else if (sender == itsEditFnDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateFunction();
		}
		itsEditFnDialog = nullptr;
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnable(kPrintMarksEPSCmd, itsPlotWidget->HasMarks());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kNewExprCmd)
	{
		(GetApplication())->NewExpression();
	}
	else if (index == kEditConstCmd)
	{
		(GetApplication())->ShowConstants();
	}
	else if (index == kNew2DPlotCmd)
	{
		(GetApplication())->New2DPlot(this);
	}
	else if (index == kConvBaseCmd)
	{
		(GetApplication())->ShowBaseConversion();
	}

	else if (index == kPSPageSetupCmd)
	{
		itsPlotWidget->HandlePSPageSetup();
	}
	else if (index == kPrintPSCmd)
	{
		itsPlotWidget->PrintPS();
	}

	else if (index == kPrintPlotEPSCmd)
	{
		itsPlotWidget->PrintPlotEPS();
	}
	else if (index == kPrintMarksEPSCmd)
	{
		itsPlotWidget->PrintMarksEPS();
	}

	else if (index == kCloseWindowCmd)
	{
		GetWindow()->Close();
	}
	else if (index == kQuitCmd)
	{
		(GetApplication())->Quit();
	}
}

/*******************************************************************************
 UpdateEditFnMenu

 ******************************************************************************/

void
Plot2DDirector::UpdateEditFnMenu()
{
	itsEditFnMenu->RemoveAllItems();

	const JSize count = itsFnList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsEditFnMenu->AppendItem(itsPlotWidget->GetCurveName(i));
	}
}

/*******************************************************************************
 HandleEditFnMenu

 ******************************************************************************/

void
Plot2DDirector::HandleEditFnMenu
	(
	const JIndex index
	)
{
	EditFunction(index);
}

/*******************************************************************************
 EditFunction (private)

 ******************************************************************************/

void
Plot2DDirector::EditFunction
	(
	const JIndex index
	)
{
	assert( itsEditFnDialog == nullptr );

	const J2DPlotFunction* curve = itsFnList->GetElement(index);

	JFloat min, max;
	curve->GetXRange(&min, &max);

	itsEditFnDialog =
		jnew Plot2DFunctionDialog(this, (GetApplication())->GetVariableList(),
									curve->GetFunction(),
									itsPlotWidget->GetCurveName(index), min, max);
	assert( itsEditFnDialog != nullptr );
	itsEditFnDialog->BeginDialog();
	ListenTo(itsEditFnDialog);

	itsEditFnIndex = index;
}

/******************************************************************************
 UpdateFunction (private)

 ******************************************************************************/

void
Plot2DDirector::UpdateFunction()
{
	assert( itsEditFnDialog != nullptr );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	itsEditFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	J2DPlotFunction* curve = itsFnList->GetElement(itsEditFnIndex);
	curve->SetFunction((GetApplication())->GetVariableList(), f->Copy(), true,
					   VarList::kXIndex, xMin, xMax);

	itsPlotWidget->SetCurveName(itsEditFnIndex, curveName);
}

/******************************************************************************
 UpdateCurveOptionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::UpdateCurveOptionsMenu()
{
	(itsPlotWidget->GetCurveOptionsMenu())->EnableItem(itsEditFunctionItemIndex);
}

/******************************************************************************
 HandleCurveOptionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::HandleCurveOptionsMenu
	(
	const JIndex index
	)
{
	if (index == itsEditFunctionItemIndex)
	{
		EditFunction(itsPlotWidget->GetCurveOptionsMenuCurveIndex());
	}
}
