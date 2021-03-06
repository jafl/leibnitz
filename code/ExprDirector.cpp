/******************************************************************************
 ExprDirector.cpp

	Window to display expression being edited by user.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "ExprDirector.h"
#include "ExprEditor.h"
#include "TapeText.h"
#include "KeyPad.h"
#include "VarList.h"
#include "globals.h"
#include "fileVersions.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/jXActionDefs.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <fstream>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

JString	ExprDirector::theDefGeom;
const JFileVersion kCurrentDefGeomVersion = 0;

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
	"    New expression      %k Meta-N"
	"  | Edit constants      %k Meta-E"
	"  | Plot 2D function... %k Meta-Shift-P"
	"  | Base conversion...  %k Meta-B"
	"%l| Page setup..."
	"  | Print tape...       %k Meta-P"
	"%l| Save tape...        %k Meta-S"
	"  | Clear tape"
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewExprCmd = 1,
	kEditConstCmd,
	kNew2DPlotCmd,
	kConvBaseCmd,
	kPageSetupCmd, kPrintTapeCmd,
	kSaveTapeCmd, kClearTapeCmd,
	kCloseWindowCmd, kQuitCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Show keypad %b"
	"  | Mac/Win/X emulation..."
	"%l| Save window size as default";

enum
{
	kToggleKeyPadVisibleCmd = 1,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

ExprDirector::ExprDirector
	(
	JXDirector*			supervisor,
	const VarList*	varList
	)
	:
	JXWindowDirector(supervisor),
	itsTapeName("Untitled")
{
	BuildWindow(varList);
	UseDefaultGeometry();
}

ExprDirector::ExprDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	const VarList*	varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);

	JString expr;
	input >> expr;

	JExprParser p(itsExprWidget);

	JFunction* f;
	const bool ok = p.Parse(expr, &f);
	assert( ok );

	itsExprWidget->SetFunction(varList, f);

	JString tapeText;
	input >> tapeText >> itsTapeName;
	itsTapeWidget->GetText()->SetText(tapeText);

	if (vers >= 4)
	{
		JString printName;
		input >> printName;
		itsTapeWidget->SetPTPrintFileName(printName);
	}

	GetWindow()->ReadGeometry(input);
	itsPartition->ReadGeometry(input);
	itsExprWidget->ReadScrollSetup(input);
	itsTapeWidget->ReadScrollSetup(input);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExprDirector::~ExprDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
ExprDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	output << ' ' << itsExprWidget->GetFunction()->Print();
	output << ' ' << itsTapeWidget->GetText()->GetText();
	output << ' ' << itsTapeName;
	output << ' ' << itsTapeWidget->GetPTPrintFileName();

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsPartition->WriteGeometry(output);

	output << ' ';
	itsExprWidget->WriteScrollSetup(output);

	output << ' ';
	itsTapeWidget->WriteScrollSetup(output);
}

/******************************************************************************
 ReadPrefs (static)

 ******************************************************************************/

void
ExprDirector::ReadPrefs
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	if (vers >= 2)
	{
		input >> theDefGeom;
	}
}

/******************************************************************************
 WritePrefs (static)

 ******************************************************************************/

void
ExprDirector::WritePrefs
	(
	std::ostream& output
	)
{
	output << theDefGeom;
}

/******************************************************************************
 UseDefaultGeometry (private)

 ******************************************************************************/

void
ExprDirector::UseDefaultGeometry()
{
	if (!theDefGeom.IsEmpty())
	{
		const std::string s(theDefGeom.GetRawBytes(), theDefGeom.GetByteCount());
		std::istringstream input(s);

		JFileVersion vers;
		input >> vers;
		if (vers <= kCurrentDefGeomVersion)
		{
			JCoordinate w,h;
			input >> w >> h;
			GetWindow()->SetSize(w,h);

			itsPartition->ReadGeometry(input);
		}
	}
}

/******************************************************************************
 SetDefaultGeometry (private)

 ******************************************************************************/

void
ExprDirector::SetDefaultGeometry()
{
	std::ostringstream data;
	data << kCurrentDefGeomVersion;
	data << ' ' << GetWindow()->GetFrameWidth();
	data << ' ' << GetWindow()->GetFrameHeight();
	data << ' ';
	itsPartition->WriteGeometry(data);

	theDefGeom = data.str();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
ExprDirector::Activate()
{
	JXWindowDirector::Activate();
	itsExprWidget->Focus();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_expr_window.xpm"

void
ExprDirector::BuildWindow
	(
	const VarList* varList
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(100);
	heights.AppendElement(0);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(50);
	minHeights.AppendElement(50);

// begin JXLayout

	auto* window = jnew JXWindow(this, 360,240, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 360,30);
	assert( menuBar != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 300,210);
	assert( itsPartition != nullptr );

	itsKeyPad =
		jnew KeyPad(window,
					JXWidget::kFixedRight, JXWidget::kVElastic, 300,30, 60,210);
	assert( itsKeyPad != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ExprDirector"));
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetExprWindowClass());

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, thx_expr_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	// create tape

	auto* scrollbarSet2 =
		jnew JXScrollbarSet(itsPartition->GetCompartment(2),
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet2 != nullptr );
	scrollbarSet2->FitToEnclosure();

	itsTapeWidget =
		jnew TapeText(scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 10,10);
	assert( itsTapeWidget != nullptr );
	itsTapeWidget->FitToEnclosure();
	itsTapeWidget->SetPTPrinter(GetTapePrinter());

	// create expr editor -- requires tape

	auto* scrollbarSet1 =
		jnew JXScrollbarSet(itsPartition->GetCompartment(1),
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet1 != nullptr );
	scrollbarSet1->FitToEnclosure();

	itsExprWidget =
		jnew ExprEditor(varList, menuBar, itsTapeWidget, scrollbarSet1,
						  scrollbarSet1->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 10,10);
	assert( itsExprWidget != nullptr );
	itsExprWidget->FitToEnclosure();

	itsKeyPad->SetExprEditor(itsExprWidget);
	itsTapeWidget->ShareEditMenu(itsExprWidget->GetEditMenu());

	// create menus

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::globals"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = (GetApplication())->CreateHelpMenu(menuBar, "ExprDirector");
	ListenTo(itsHelpMenu);

	// keypad visibility

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
ExprDirector::UpdateDisplay()
{
	if (itsKeyPad->WouldBeVisible() && !(GetApplication())->KeyPadIsVisible())
	{
		itsKeyPad->Hide();
		itsPartition->AdjustSize(itsKeyPad->GetFrameWidth(), 0);
	}
	else if (!itsKeyPad->WouldBeVisible() && (GetApplication())->KeyPadIsVisible())
	{
		itsKeyPad->Show();
		itsPartition->AdjustSize(-itsKeyPad->GetFrameWidth(), 0);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ExprDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionsMenu();
	}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
	}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdatePrefsMenu();
	}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		GetApplication()->UpdateHelpMenu(itsHelpMenu);
	}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		GetApplication()->HandleHelpMenu("ExprHelp", selection->GetIndex());
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
ExprDirector::UpdateActionsMenu()
{
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
ExprDirector::HandleActionsMenu
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
		(GetApplication())->New2DPlot();
	}
	else if (index == kConvBaseCmd)
	{
		(GetApplication())->ShowBaseConversion();
	}

	else if (index == kPageSetupCmd)
	{
		itsTapeWidget->HandlePTPageSetup();
	}
	else if (index == kPrintTapeCmd)
	{
		itsTapeWidget->PrintPT();
	}

	else if (index == kSaveTapeCmd)
	{
		SaveTape();
	}
	else if (index == kClearTapeCmd)
	{
		// this way, it is undoable

		itsTapeWidget->SelectAll();
		itsTapeWidget->DeleteSelection();
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

/******************************************************************************
 SaveTape (private)

 ******************************************************************************/

void
ExprDirector::SaveTape()
{
	JString newName;
	if ((JXGetChooseSaveFile())->SaveFile(
			JGetString("SaveTapePrompt::ExprDirector"),
			JString::empty, itsTapeName, &newName))
	{
		itsTapeName = newName;

		std::ofstream output(itsTapeName.GetBytes());
		itsTapeWidget->GetText()->GetText().Print(output);
	}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
ExprDirector::UpdatePrefsMenu()
{
	if ((GetApplication())->KeyPadIsVisible())
	{
		itsPrefsMenu->CheckItem(kToggleKeyPadVisibleCmd);
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
ExprDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kToggleKeyPadVisibleCmd)
	{
		(GetApplication())->ToggleKeyPadVisible();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		SetDefaultGeometry();
	}
}
