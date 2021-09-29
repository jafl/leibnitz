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

	virtual ~VarDirector();

	void	WriteState(std::ostream& output) const;

	// public so App can call it

	virtual bool	OKToDeactivate() override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	VarTable*	itsVarTable;
	JXTextMenu*		itsActionsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(VarList* varList);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);
};

#endif
