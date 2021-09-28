/******************************************************************************
 THX2DPlotDirector.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THX2DPlotDirector
#define _H_THX2DPlotDirector

#include <jx-af/jx/JXWindowDirector.h>

class JFunction;
class JXTextButton;
class JXTextMenu;
class JX2DPlotWidget;
class J2DPlotFunction;
class THXVarList;
class THX2DPlotFunctionDialog;

class THX2DPlotDirector : public JXWindowDirector
{
public:

	THX2DPlotDirector(JXDirector* supervisor);
	THX2DPlotDirector(std::istream& input, const JFileVersion vers,
					  JXDirector* supervisor, THXVarList* varList);

	virtual ~THX2DPlotDirector();

	void	AddFunction(THXVarList* varList, const JFunction& f,
						const JString& name,
						const JFloat xMin, const JFloat xMax);

	void	WriteState(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JXTextMenu*			itsActionsMenu;
	JXTextMenu*			itsEditFnMenu;
	JXTextMenu*			itsHelpMenu;
	JIndex				itsEditFunctionItemIndex;	// index of item on Curve Options pop up menu

	JPtrArray<J2DPlotFunction>*	itsFnList;			// contents not owned
	THX2DPlotFunctionDialog*		itsEditFnDialog;	// nullptr unless editing
	JIndex							itsEditFnIndex;		// index of curve being edited

// begin JXLayout

	JX2DPlotWidget* itsPlotWidget;

// end JXLayout

private:

	void	THX2DPlotDirectorX();
	void	BuildWindow();
	void	AddFunction(THXVarList* varList, JFunction* f,
						const JString& name,
						const JFloat xMin, const JFloat xMax);
	void	EditFunction(const JIndex index);
	void	UpdateFunction();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	UpdateEditFnMenu();
	void	HandleEditFnMenu(const JIndex index);

	void	UpdateCurveOptionsMenu();
	void	HandleCurveOptionsMenu(const JIndex index);
};

#endif
