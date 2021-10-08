/******************************************************************************
 Plot2DDirector.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_Plot2DDirector
#define _H_Plot2DDirector

#include <jx-af/jx/JXWindowDirector.h>

class JFunction;
class JXTextButton;
class JXTextMenu;
class JX2DPlotWidget;
class J2DPlotFunction;
class VarList;
class Plot2DFunctionDialog;

class Plot2DDirector : public JXWindowDirector
{
public:

	Plot2DDirector(JXDirector* supervisor);
	Plot2DDirector(std::istream& input, const JFileVersion vers,
					  JXDirector* supervisor, VarList* varList);

	~Plot2DDirector() override;

	void	AddFunction(VarList* varList, const JFunction& f,
						const JString& name,
						const JFloat xMin, const JFloat xMax);

	void	WriteState(std::ostream& output) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsEditFnMenu;
	JXTextMenu*	itsHelpMenu;
	JIndex		itsEditFunctionItemIndex;	// index of item on Curve Options pop up menu

	JPtrArray<J2DPlotFunction>*	itsFnList;			// contents not owned
	Plot2DFunctionDialog*		itsEditFnDialog;	// nullptr unless editing
	JIndex						itsEditFnIndex;		// index of curve being edited

// begin JXLayout

	JX2DPlotWidget* itsPlotWidget;

// end JXLayout

private:

	void	Plot2DDirectorX();
	void	BuildWindow();
	void	AddFunction(VarList* varList, JFunction* f,
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
