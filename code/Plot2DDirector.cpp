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
#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotFunction.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

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
		const J2DPlotDataBase* data = itsPlotWidget->GetCurve(i);
		auto& fnData = dynamic_cast<const J2DPlotFunction&>(*data);

		JFloat xMin, xMax;
		fnData.GetXRange(&xMin, &xMax);
		output << ' ' << xMin << ' ' << xMax;

		output << ' ' << fnData.GetFunction().Print();
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
#include "Plot2DDirector-Actions.h"

void
Plot2DDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 600,400, JString::empty);
	window->SetMinSize(300, 200);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Leibnitz_Plot");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != nullptr );

	itsPlotWidget =
		jnew JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);

// end JXLayout

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, thx_2D_plot_window);
	window->SetIcon(icon);

	itsPlotWidget->SetPSPrinter(GetPSGraphPrinter());
	itsPlotWidget->SetEPSPrinter(GetEPSGraphPrinter());

	ListenTo(itsPlotWidget, std::function([this](const J2DPlotWidget::TitleChanged&)
	{
		GetWindow()->SetTitle(itsPlotWidget->GetTitle());
	}));

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Plot2DDirector_Actions"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsActionsMenu->AttachHandlers(this,
		&Plot2DDirector::UpdateActionsMenu,
		&Plot2DDirector::HandleActionsMenu);
	ConfigureActionsMenu(itsActionsMenu);

	JXTextMenu* optionsMenu               = itsPlotWidget->GetOptionsMenu();
	const JIndex editFunctionSubmenuIndex = optionsMenu->GetItemCount()+1;
	optionsMenu->AppendItem(JGetString("EditFunctionItem::Plot2DDirector"));

	itsEditFnMenu = jnew JXTextMenu(optionsMenu, editFunctionSubmenuIndex, menuBar);
	itsEditFnMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsEditFnMenu->AttachHandlers(this,
		&Plot2DDirector::UpdateEditFnMenu,
		&Plot2DDirector::HandleEditFnMenu);

	GetApplication()->CreateHelpMenu(menuBar, "2DPlotHelp");

	JXTextMenu* curveOptionsMenu = itsPlotWidget->GetCurveOptionsMenu();
	itsEditFunctionItemIndex     = curveOptionsMenu->GetItemCount()+1;
	curveOptionsMenu->ShowSeparatorAfter(itsEditFunctionItemIndex-1);
	curveOptionsMenu->AppendItem(JGetString("EditFunctionItem::Plot2DDirector"));
	curveOptionsMenu->AttachHandlers(this,
		&Plot2DDirector::UpdateCurveOptionsMenu,
		&Plot2DDirector::HandleCurveOptionsMenu);

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
	const JSize count = itsFnList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (sender == itsFnList->GetItem(i))
		{
			itsFnList->RemoveItem(i);
			break;
		}
	}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnabled(kPrintMarksEPSCmd, itsPlotWidget->HasMarks());
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

	const JSize count = itsFnList->GetItemCount();
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
	const J2DPlotFunction* curve = itsFnList->GetItem(index);

	JFloat min, max;
	curve->GetXRange(&min, &max);

	auto* dlog =
		jnew Plot2DFunctionDialog(GetApplication()->GetVariableList(),
								  curve->GetFunction(),
								  itsPlotWidget->GetCurveName(index), min, max);
	if (dlog->DoDialog())
	{
		JIndex plotIndex;
		const JFunction* f;
		JString curveName;
		JFloat xMin, xMax;
		dlog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

		J2DPlotFunction* curve = itsFnList->GetItem(index);
		curve->SetFunction(GetApplication()->GetVariableList(), f->Copy(), true,
						   VarList::kXIndex, xMin, xMax);

		itsPlotWidget->SetCurveName(index, curveName);
	}
}

/******************************************************************************
 UpdateCurveOptionsMenu (private)

 ******************************************************************************/

void
Plot2DDirector::UpdateCurveOptionsMenu()
{
	itsPlotWidget->GetCurveOptionsMenu()->EnableItem(itsEditFunctionItemIndex);
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
