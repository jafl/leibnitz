/******************************************************************************
 VarDirector.cpp

	Window to display list of constants defined by user.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "VarDirector.h"
#include "VarList.h"
#include "VarTable.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jexpr/JXExprInput.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/jXActionDefs.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

VarDirector::VarDirector
	(
	JXDirector*	supervisor,
	VarList*	varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);
}

VarDirector::VarDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	VarList*			varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsVarTable->ReadScrollSetup(input);

	JSize colWidth;
	input >> colWidth;
	itsVarTable->SetColWidth(VarTable::kNameColumn, colWidth);

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

VarDirector::~VarDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
VarDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsVarTable->WriteScrollSetup(output);

	output << ' ' << itsVarTable->GetColWidth(VarTable::kNameColumn);
	output << ' ' << JBoolToString(IsActive() && !GetWindow()->IsIconified());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "VarDirector-Actions.h"

void
VarDirector::BuildWindow
	(
	VarList* varList
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 260,250, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 260,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,50, 260,200);
	assert( scrollbarSet != nullptr );

	auto* colHdrContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,30, 260,20);
	assert( colHdrContainer != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::VarDirector"));
	window->SetWMClass(GetWMClassInstance(), GetVarWindowClass());
	window->SetMinSize(150,150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::VarDirector_Actions"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsActionsMenu->AttachHandlers(this,
		&VarDirector::UpdateActionsMenu,
		&VarDirector::HandleActionsMenu);
	ConfigureActionsMenu(itsActionsMenu);

	JXTextMenu* fontMenu = JXExprInput::CreateFontMenu(menuBar);
	menuBar->AppendMenu(fontMenu);

	GetApplication()->CreateHelpMenu(menuBar, "ConstantsHelp");

	itsVarTable =
		jnew VarTable(varList, fontMenu, scrollbarSet,
						scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 10,10);
	assert( itsVarTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsVarTable, scrollbarSet, colHdrContainer,
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("NameColumnTitle::VarDirector"));
	colHeader->SetColTitle(2, JGetString("ValueColumnTitle::VarDirector"));
	colHeader->TurnOnColResizing(20);
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
VarDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnabled(kRemoveConstCmd,
								  itsVarTable->OKToRemoveSelectedConstant());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
VarDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kNewConstCmd)
	{
		itsVarTable->NewConstant();
	}
	else if (index == kRemoveConstCmd)
	{
		itsVarTable->RemoveSelectedConstant();
	}
	else if (index == kNewExprCmd)
	{
		GetApplication()->NewExpression();
	}
	else if (index == kNew2DPlotCmd)
	{
		GetApplication()->New2DPlot();
	}
	else if (index == kConvBaseCmd)
	{
		GetApplication()->ShowBaseConversion();
	}

	else if (index == kCloseWindowCmd)
	{
		GetWindow()->Close();
	}
	else if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 OKToDeactivate (virtual)

	Check that the table contents are valid.

	This is public so App can call it.

 ******************************************************************************/

bool
VarDirector::OKToDeactivate()
{
	return JXWindowDirector::OKToDeactivate();
}
