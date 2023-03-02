/******************************************************************************
 Plot2DFunctionDialog.h

	Interface for the Plot2DFunctionDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_Plot2DFunctionDialog
#define _H_Plot2DFunctionDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JFunction;
class JXTextMenu;
class JXInputField;
class JXFloatInput;
class JXExprEditor;
class Plot2DDirector;
class VarList;

class Plot2DFunctionDialog : public JXModalDialogDirector
{
public:

	Plot2DFunctionDialog(const VarList* varList,
							const Plot2DDirector* prevPlot = nullptr);
	Plot2DFunctionDialog(const VarList* varList,
							const JFunction& f, const JString& curveName,
							const JFloat min, const JFloat max);

	~Plot2DFunctionDialog() override;

	void	GetSettings(JIndex* plotIndex,
						const JFunction** f, JString* curveName,
						JFloat* fMin, JFloat* fMax) const;

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex			itsPlotIndex;
	JXExprEditor*	itsExprWidget;

// begin JXLayout

	JXInputField* itsCurveName;
	JXFloatInput* itsMinInput;
	JXFloatInput* itsMaxInput;
	JXTextMenu*   itsPlotMenu;

// end JXLayout

private:

	void	BuildWindow(const VarList* varList, const Plot2DDirector* prevPlot);
};

#endif
