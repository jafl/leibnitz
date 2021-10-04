/******************************************************************************
 App.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "App.h"
#include "VarList.h"
#include "VarDirector.h"
#include "ExprDirector.h"
#include "Plot2DDirector.h"
#include "Plot2DFunctionDialog.h"
#include "BaseConvDirector.h"
#include "AboutDialog.h"
#include "globals.h"
#include "stringData.h"
#include "fileVersions.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jx/JXSharedPrefsManager.h>
#include <jx-af/jx/JXSplashWindow.h>
#include <jx-af/jx/JXTipOfTheDayDialog.h>
#include <jx-af/jx/jXActionDefs.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <fstream>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

// application signature (MDI)

static const JUtf8Byte* kAppSignature = "leibnitz";

// state information

static const JString kStateFileName(".leibnitz.session");
static const JString kOldStateFileName(".THX-1138.session");

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Tip of the Day"
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpWindowCmd,
	kTipCmd,
	kHelpChangeLogCmd, kHelpCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

App::App
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
	itsStartupFlag = true;

	itsExprList = jnew JPtrArray<ExprDirector>(JPtrArrayT::kForgetAll);
	assert( itsExprList != nullptr );

	itsKeyPadVisibleFlag = true;

	its2DPlotList = jnew JPtrArray<Plot2DDirector>(JPtrArrayT::kForgetAll);
	assert( its2DPlotList != nullptr );
	its2DPlotList->SetCompareFunction(Compare2DPlotTitles);
	its2DPlotList->SetSortOrder(JListT::kSortAscending);

	its2DPlotFnDialog = nullptr;

	CreateGlobals(this);
	RestoreProgramState();

	itsStartupFlag = false;
}

/******************************************************************************
 Destructor

	Delete global objects in reverse order of creation.

 ******************************************************************************/

App::~App()
{
	jdelete itsVarList;
	jdelete itsExprList;		// objects deleted by JXDirector
	jdelete its2DPlotList;	// objects deleted by JXDirector

	DeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

bool
App::Close()
{
	if (!itsVarDirector->OKToDeactivate())
	{
		return false;
	}

	SaveProgramState();
	return JXApplication::Close();
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
App::DisplayAbout
	(
	const JString& prevVersStr
	)
{
	auto* dlog = jnew AboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 NewExpression

 ******************************************************************************/

ExprDirector*
App::NewExpression
	(
	const bool centerOnScreen
	)
{
	auto* expr = jnew ExprDirector(this, itsVarList);
	assert( expr != nullptr );
	if (centerOnScreen)
	{
		(expr->GetWindow())->PlaceAsDialogWindow();
	}
	expr->Activate();
	itsExprList->Append(expr);
	return expr;
}

/******************************************************************************
 SetKeyPadVisible

 ******************************************************************************/

void
App::SetKeyPadVisible
	(
	const bool visible
	)
{
	if (visible != itsKeyPadVisibleFlag)
	{
		itsKeyPadVisibleFlag = visible;

		const JSize count = itsExprList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			(itsExprList->GetElement(i))->UpdateDisplay();
		}
	}
}

/******************************************************************************
 New2DPlot

	prevPlot should be the Plot2DDirector that originated the request,
	nullptr otherwise.

 ******************************************************************************/

void
App::New2DPlot
	(
	const Plot2DDirector* prevPlot
	)
{
	assert( its2DPlotFnDialog == nullptr );

	its2DPlotFnDialog = jnew Plot2DFunctionDialog(this, itsVarList, prevPlot);
	assert( its2DPlotFnDialog != nullptr );
	its2DPlotFnDialog->BeginDialog();
	ListenTo(its2DPlotFnDialog);
}

/******************************************************************************
 Create2DPlot (private)

 ******************************************************************************/

void
App::Create2DPlot()
{
	assert( its2DPlotFnDialog != nullptr );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	its2DPlotFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	if (plotIndex > its2DPlotList->GetElementCount())
	{
		auto* plot = jnew Plot2DDirector(this);
		assert( plot != nullptr );
		its2DPlotList->Append(plot);
	}

	Plot2DDirector* plot = its2DPlotList->GetElement(plotIndex);
	plot->AddFunction(itsVarList, *f, curveName, xMin, xMax);
	plot->Activate();
}

/******************************************************************************
 ShowConstants

 ******************************************************************************/

void
App::ShowConstants()
	const
{
	itsVarDirector->Activate();
}

/******************************************************************************
 ShowBaseConversion

 ******************************************************************************/

void
App::ShowBaseConversion()
	const
{
	itsBCDirector->Activate();
}

/******************************************************************************
 RestoreProgramState (private)

 ******************************************************************************/

void
App::RestoreProgramState()
{
JIndex i;

	if (!JGetPrefsDirectory(&itsStatePath))
	{
		JGetUserNotification()->ReportError(JGetString("NoPrefsDir::App"));
		JThisProcess::Exit(1);
	}

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	if (!JFileExists(fullName))
	{
		const JString oldName = JCombinePathAndName(itsStatePath, kOldStateFileName);
		if (!JFileExists(oldName) || !(JRenameFile(oldName, fullName)).OK())
		{
			InitProgramState();
			DisplayAbout();
			return;
		}
	}

	std::ifstream input(fullName.GetBytes());

	JFileVersion vers;
	input >> vers;
	if (input.eof() || input.fail())
	{
		InitProgramState();
		return;
	}
	else if (vers > kCurrentStateVersion)
	{
		const JUtf8Byte* map[] =
	{
			"name", fullName.GetBytes()
	};
		const JString msg = JGetString("CannotReadNewerVersion::App", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		JThisProcess::Exit(1);
	}

	JString prevProgramVers;
	input >> prevProgramVers;

	bool displayAbout = false;
	if (prevProgramVers != GetVersionNumberStr())
	{
		if (!JGetUserNotification()->AcceptLicense())
		{
			JThisProcess::Exit(0);
		}

		displayAbout = true;
	}

	itsVarList = jnew VarList(input, vers);
	assert( itsVarList != nullptr );

	itsVarDirector = jnew VarDirector(input, vers, this, itsVarList);
	assert( itsVarDirector != nullptr );

	itsBCDirector = jnew BaseConvDirector(input, vers, this);
	assert( itsBCDirector != nullptr );

	if (vers >= 1)
	{
		input >> JBoolFromString(itsKeyPadVisibleFlag);
	}

	if (vers >= 2)
	{
		ExprDirector::ReadPrefs(input, vers);

		// ignoring obsolete JXGetHelpManager data, since it was removed in version 8
	}

	if (vers >= 5)
	{
		(JXGetChooseSaveFile())->ReadSetup(input);
	}

	if (vers >= 3)
	{
		(GetPSGraphPrinter())->ReadXPSSetup(input);
	}
	if (vers >= 7)
	{
		(GetEPSGraphPrinter())->ReadX2DEPSSetup(input);
	}

	JSize exprCount;
	input >> exprCount;

	if (exprCount == 0)
	{
		NewExpression(true);
	}
	else
	{
		for (i=1; i<=exprCount; i++)
		{
			auto* expr = jnew ExprDirector(input, vers, this, itsVarList);
			assert( expr != nullptr );
			expr->Activate();
			itsExprList->Append(expr);

			if (exprCount == 1)
			{
				(expr->GetWindow())->Deiconify();
			}
		}
	}

	JSize plotCount;
	input >> plotCount;

	for (i=1; i<=plotCount; i++)
	{
		auto* plot = jnew Plot2DDirector(input, vers, this, itsVarList);
		assert( plot != nullptr );
		plot->Activate();
		its2DPlotList->Append(plot);
	}

	if (displayAbout)
	{
		DisplayAbout(prevProgramVers);
	}
}

/******************************************************************************
 InitProgramState (private)

 ******************************************************************************/

void
App::InitProgramState()
{
	if (!JGetUserNotification()->AcceptLicense())
	{
		JThisProcess::Exit(0);
	}

	itsVarList = jnew VarList;
	assert( itsVarList != nullptr );

	itsVarDirector = jnew VarDirector(this, itsVarList);
	assert( itsVarDirector != nullptr );

	itsBCDirector = jnew BaseConvDirector(this);
	assert( itsBCDirector != nullptr );

	NewExpression(true);
}

/******************************************************************************
 SaveProgramState (private)

 ******************************************************************************/

void
App::SaveProgramState()
{
JIndex i;

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	std::ofstream output(fullName.GetBytes());

	output << kCurrentStateVersion;
	output << ' ' << JString(GetVersionNumberStr());
	output << ' ' << *itsVarList;

	output << ' ';
	itsVarDirector->WriteState(output);

	output << ' ';
	itsBCDirector->WriteState(output);

	output << ' ' << JBoolToString(itsKeyPadVisibleFlag);

	output << ' ';
	ExprDirector::WritePrefs(output);

	output << ' ';
	(JXGetChooseSaveFile())->WriteSetup(output);

	output << ' ';
	(GetPSGraphPrinter())->WriteXPSSetup(output);

	output << ' ';
	(GetEPSGraphPrinter())->WriteX2DEPSSetup(output);

	const JSize exprCount = itsExprList->GetElementCount();
	output << ' ' << exprCount;

	for (i=1; i<=exprCount; i++)
	{
		output << ' ';
		(itsExprList->GetElement(i))->WriteState(output);
	}

	JSize plotCount = its2DPlotList->GetElementCount();
	output << ' ' << plotCount;

	for (i=1; i<=plotCount; i++)
	{
		output << ' ';
		(its2DPlotList->GetElement(i))->WriteState(output);
	}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
App::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == its2DPlotFnDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			Create2DPlot();
		}
		its2DPlotFnDialog = nullptr;
	}

	else
	{
		JXApplication::Receive(sender, message);
	}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for windows that are closed.

	You cannot use dynamic_cast because the object is already at least
	partially deleted.

 ******************************************************************************/

void
App::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex dirIndex;
	auto* exprDir = (ExprDirector*) theDirector;
	if (exprDir != nullptr && itsExprList->Find(exprDir, &dirIndex))
	{
		itsExprList->RemoveElement(dirIndex);
	}
	auto* plot2DDir = (Plot2DDirector*) theDirector;
	if (plot2DDir != nullptr && its2DPlotList->Find(plot2DDir, &dirIndex))
	{
		its2DPlotList->RemoveElement(dirIndex);
	}

	JXApplication::DirectorClosed(theDirector);

	if (!itsStartupFlag && itsExprList->IsEmpty())
	{
		Quit();
	}
}

/*****************************************************************************
 BuildPlotMenu

	If origPlot != nullptr, initialChoice is set to the index of the director.

	We are deactivated until a plot is chosen, so sorting its2DPlotList
	is safe.

 ******************************************************************************/

void
App::BuildPlotMenu
	(
	JXTextMenu*					menu,
	const Plot2DDirector*	origPlot,
	JIndex*						initialChoice
	)
	const
{
	its2DPlotList->Sort();

	*initialChoice = 0;

	const JSize count = its2DPlotList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Plot2DDirector* plot = its2DPlotList->GetElement(i);
		menu->AppendItem((plot->GetWindow())->GetTitle(), JXMenu::kRadioType);

		if (plot == origPlot)
		{
			*initialChoice = i;
		}
	}

	if (*initialChoice == 0)
	{
		*initialChoice = count+1;
	}

	if (count > 0)
	{
		menu->ShowSeparatorAfter(count);
	}
	menu->AppendItem(JGetString("NewWindowItem::App"), JXMenu::kRadioType);
	menu->SetUpdateAction(JXMenu::kDisableNone);
}

/******************************************************************************
 Compare2DPlotTitles (static private)

 ******************************************************************************/

JListT::CompareResult
App::Compare2DPlotTitles
	(
	Plot2DDirector* const & p1,
	Plot2DDirector* const & p2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&((p1->GetWindow())->GetTitle())),
			const_cast<JString*>(&((p2->GetWindow())->GetTitle())));
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

JXTextMenu*
App::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	idNamespace
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	menu->SetMenuItems(kHelpMenuStr, idNamespace);
	menu->SetUpdateAction(JXMenu::kDisableNone);

	menu->SetItemImage(kHelpTOCCmd,    jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd, jx_help_specific);

	return menu;
}

/******************************************************************************
 UpdateHelpMenu

 ******************************************************************************/

void
App::UpdateHelpMenu
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
App::HandleHelpMenu
	(
	JXTextMenu*			menu,
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
	{
		DisplayAbout();
	}

	else if (index == kHelpTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kHelpOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kHelpWindowCmd)
	{
		JXGetHelpManager()->ShowSection(windowSectionName);
	}

	else if (index == kTipCmd)
	{
		auto* dlog = jnew JXTipOfTheDayDialog;
		assert( dlog != nullptr );
		dlog->BeginDialog();
	}

	else if (index == kHelpChangeLogCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kHelpCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
App::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
	{
		SaveProgramState();
	}
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
App::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
App::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}
