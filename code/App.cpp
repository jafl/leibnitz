/******************************************************************************
 App.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "App.h"
#include "PrefsManager.h"
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
#include <jx-af/jx/JXSharedPrefsManager.h>
#include <jx-af/jx/JXSplashWindow.h>
#include <jx-af/jx/JXTipOfTheDayDialog.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <fstream>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

// application signature (MDI)

static const JUtf8Byte* kAppSignature = "leibnitz";

// state information

static const JString kStateFileName(".leibnitz.session");
static const JString kOldStateFileName(".THX-1138.session");

/******************************************************************************
 Constructor

 ******************************************************************************/

App::App
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, kAppSignature, "Leibnitz", kDefaultStringData)
{
	itsStartupFlag = true;

	itsExprList = jnew JPtrArray<ExprDirector>(JPtrArrayT::kForgetAll);
	assert( itsExprList != nullptr );

	itsKeyPadVisibleFlag = true;

	its2DPlotList = jnew JPtrArray<Plot2DDirector>(JPtrArrayT::kForgetAll);
	assert( its2DPlotList != nullptr );
	its2DPlotList->SetCompareFunction(Compare2DPlotTitles);
	its2DPlotList->SetSortOrder(JListT::kSortAscending);

	CreateGlobals(this);

	StartFiber([this]()
	{
		RestoreProgramState();
	});
}

/******************************************************************************
 Destructor

	Delete global objects in reverse order of creation.

 ******************************************************************************/

App::~App()
{
	jdelete itsVarList;
	jdelete itsExprList;	// objects deleted by JXDirector
	jdelete its2DPlotList;	// objects deleted by JXDirector

	DeleteGlobals();
}

/******************************************************************************
 Quit (virtual)

 ******************************************************************************/

void
App::Quit()
{
	if (itsVarDirector == nullptr || itsVarDirector->OKToDeactivate())
	{
		if (!IsQuitting() && HasPrefsManager())
		{
			SaveProgramState();
		}

		JXApplication::Quit();
	}
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
App::DisplayAbout
	(
	const bool		showLicense,
	const JString&	prevVersStr
	)
{
	StartFiber([showLicense, prevVersStr]()
	{
		if (!showLicense || JGetUserNotification()->AcceptLicense())
		{
			auto* dlog = jnew AboutDialog(prevVersStr);
			dlog->DoDialog();

			JCheckForNewerVersion(GetPrefsManager(), kVersionCheckID);
		}
		else
		{
			ForgetPrefsManager();
			JXGetApplication()->Quit();
		}
	});
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
	if (centerOnScreen)
	{
		expr->GetWindow()->PlaceAsDialogWindow();
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

		const JSize count = itsExprList->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			(itsExprList->GetItem(i))->UpdateDisplay();
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
	auto* dlog = jnew Plot2DFunctionDialog(itsVarList, prevPlot);
	if (dlog->DoDialog())
	{
		JIndex plotIndex;
		const JFunction* f;
		JString curveName;
		JFloat xMin, xMax;
		dlog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

		if (plotIndex > its2DPlotList->GetItemCount())
		{
			auto* plot = jnew Plot2DDirector(this);
			its2DPlotList->Append(plot);
		}

		Plot2DDirector* plot = its2DPlotList->GetItem(plotIndex);
		plot->AddFunction(itsVarList, *f, curveName, xMin, xMax);
		plot->Activate();
	}
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
	if (!JGetPrefsDirectory(&itsStatePath))
	{
		JGetUserNotification()->ReportError(JGetString("NoPrefsDir::App"));
		Quit();
		return;
	}

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	if (!JFileExists(fullName))
	{
		const JString oldName = JCombinePathAndName(itsStatePath, kOldStateFileName);
		if (!JFileExists(oldName) || !(JRenameFile(oldName, fullName)).OK())
		{
			InitProgramState();
			DisplayAbout(true);
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
		Quit();
		return;
	}

	JString prevProgramVers;
	input >> prevProgramVers;

	const bool displayAbout = prevProgramVers != GetVersionNumberStr();

	itsVarList = jnew VarList(input, vers);

	itsVarDirector = jnew VarDirector(input, vers, this, itsVarList);

	itsBCDirector = jnew BaseConvDirector(input, vers, this);

	if (vers >= 1)
	{
		input >> JBoolFromString(itsKeyPadVisibleFlag);
	}

	if (vers >= 2)
	{
		ExprDirector::ReadPrefs(input, vers);

		// ignoring obsolete JXGetHelpManager data, since it was removed in version 8
	}

	if (5 <= vers && vers < 12)
	{
		JXCSFDialogBase::ReadOldState(input);
	}
	else if (vers >= 12)
	{
		JString s;
		input >> s;
		JXCSFDialogBase::SetState(s);
	}

	if (vers >= 3)
	{
		GetPSGraphPrinter()->ReadXPSSetup(input);
	}
	if (vers >= 7)
	{
		GetEPSGraphPrinter()->ReadX2DEPSSetup(input);
	}

	JSize exprCount;
	input >> exprCount;

	if (exprCount == 0)
	{
		NewExpression(true);
	}
	else
	{
		for (JIndex i=1; i<=exprCount; i++)
		{
			auto* expr = jnew ExprDirector(input, vers, this, itsVarList);
			expr->Activate();
			itsExprList->Append(expr);

			if (exprCount == 1)
			{
				expr->GetWindow()->Deiconify();
			}
		}
	}

	JSize plotCount;
	input >> plotCount;

	for (JIndex i=1; i<=plotCount; i++)
	{
		auto* plot = jnew Plot2DDirector(input, vers, this, itsVarList);
		plot->Activate();
		its2DPlotList->Append(plot);
	}

	if (displayAbout)
	{
		DisplayAbout(true, prevProgramVers);
	}

	itsStartupFlag = false;
}

/******************************************************************************
 InitProgramState (private)

 ******************************************************************************/

void
App::InitProgramState()
{
	itsVarList = jnew VarList;

	itsVarDirector = jnew VarDirector(this, itsVarList);

	itsBCDirector = jnew BaseConvDirector(this);

	NewExpression(true);
}

/******************************************************************************
 SaveProgramState (private)

 ******************************************************************************/

void
App::SaveProgramState()
{
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

	output << ' ' << JXCSFDialogBase::GetState();

	output << ' ';
	GetPSGraphPrinter()->WriteXPSSetup(output);

	output << ' ';
	GetEPSGraphPrinter()->WriteX2DEPSSetup(output);

	output << ' ' << itsExprList->GetItemCount();

	for (auto* e : *itsExprList)
	{
		output << ' ';
		e->WriteState(output);
	}

	output << ' ' << its2DPlotList->GetItemCount();

	for (auto* p : *its2DPlotList)
	{
		output << ' ';
		p->WriteState(output);
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
	auto* exprDir = dynamic_cast<ExprDirector*>(theDirector);
	if (exprDir != nullptr && itsExprList->Find(exprDir, &dirIndex))
	{
		itsExprList->RemoveItem(dirIndex);
	}

	auto* plot2DDir = dynamic_cast<Plot2DDirector*>(theDirector);
	if (plot2DDir != nullptr && its2DPlotList->Find(plot2DDir, &dirIndex))
	{
		its2DPlotList->RemoveItem(dirIndex);
	}

	if (theDirector == itsVarDirector)
	{
		itsVarDirector = nullptr;
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
	JXTextMenu*				menu,
	const Plot2DDirector*	origPlot,
	JIndex*					initialChoice
	)
	const
{
	its2DPlotList->Sort();

	*initialChoice = 0;

	const JSize count = its2DPlotList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Plot2DDirector* plot = its2DPlotList->GetItem(i);
		menu->AppendItem(plot->GetWindow()->GetTitle(), JXMenu::kRadioType);

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

std::weak_ordering
App::Compare2DPlotTitles
	(
	Plot2DDirector* const & p1,
	Plot2DDirector* const & p2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&p1->GetWindow()->GetTitle()),
			const_cast<JString*>(&p2->GetWindow()->GetTitle()));
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include "App-Help.h"

JXTextMenu*
App::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	sectionName
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("MenuTitle::App_Help"));
	menu->SetMenuItems(kHelpMenuStr);
	menu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureHelpMenu(menu);

	ListenTo(menu, std::function([this, sectionName](const JXMenu::ItemSelected& msg)
	{
		HandleHelpMenu(sectionName, msg.GetIndex());
	}));

	return menu;
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
App::HandleHelpMenu
	(
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
		dlog->DoDialog();
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
