/******************************************************************************
 VarDirector.h

	Interface for the VarDirector class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarDirector
#define _H_VarDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXTextButton;
class VarList;
class VarTable;

class VarDirector : public JXWindowDirector
{
public:

	VarDirector(JXDirector* supervisor, VarList* varList);
	VarDirector(std::istream& input, const JFileVersion vers,
				JXDirector* supervisor, VarList* varList);

	~VarDirector() override;

	void	WriteState(std::ostream& output) const;

	// public so App can call it

	bool	OKToDeactivate() override;

private:

	JXTextMenu*	itsActionsMenu;

// begin JXLayout

	VarTable* itsVarTable;

// end JXLayout

private:

	void	BuildWindow(VarList* varList);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);
};

#endif
