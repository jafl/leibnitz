/******************************************************************************
 App.h

	Interface for the App class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_App
#define _H_App

#include <jx-af/jx/JXApplication.h>

class JString;
class JXMenuBar;
class JXTextMenu;
class VarList;
class VarDirector;
class ExprDirector;
class Plot2DDirector;
class Plot2DFunctionDialog;
class BaseConvDirector;

class App : public JXApplication
{
public:

	App(int* argc, char* argv[]);

	virtual ~App();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	VarList*		GetVariableList() const;
	ExprDirector*	NewExpression(const bool centerOnScreen = false);
	void			New2DPlot(const Plot2DDirector* prevPlot = nullptr);
	void			ShowConstants() const;
	void			ShowBaseConversion() const;

	void	BuildPlotMenu(JXTextMenu* menu, const Plot2DDirector* origPlot,
						  JIndex* initialChoice) const;

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JUtf8Byte* idNamespace);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JUtf8Byte* windowSectionName,
							   const JIndex index);

	bool	KeyPadIsVisible() const;
	void	SetKeyPadVisible(const bool visible);
	void	ToggleKeyPadVisible();

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual bool	Close() override;
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;
	virtual void	DirectorClosed(JXDirector* theDirector) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsStartupFlag;
	JString	itsStatePath;

	VarList*					itsVarList;
	VarDirector*				itsVarDirector;
	JPtrArray<ExprDirector>*	itsExprList;
	bool						itsKeyPadVisibleFlag;
	JPtrArray<Plot2DDirector>*	its2DPlotList;
	Plot2DFunctionDialog*		its2DPlotFnDialog;	// nullptr unless asking user
	BaseConvDirector*			itsBCDirector;

private:

	void	RestoreProgramState();
	void	InitProgramState();
	void	SaveProgramState();

	void	Create2DPlot();

	static JListT::CompareResult
		Compare2DPlotTitles(Plot2DDirector* const & p1, Plot2DDirector* const & p2);
};


/******************************************************************************
 GetVariableList

 ******************************************************************************/

inline VarList*
App::GetVariableList()
	const
{
	return itsVarList;
}

/******************************************************************************
 KeyPad visible

 ******************************************************************************/

inline bool
App::KeyPadIsVisible()
	const
{
	return itsKeyPadVisibleFlag;
}

inline void
App::ToggleKeyPadVisible()
{
	SetKeyPadVisible(!itsKeyPadVisibleFlag);
}

#endif
