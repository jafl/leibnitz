/******************************************************************************
 ExprDirector.h

	Interface for the ExprDirector class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExprDirector
#define _H_ExprDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JString.h>

class JXTextButton;
class JXTextMenu;
class JXVertPartition;
class VarList;
class ExprEditor;
class TapeText;
class KeyPad;

class ExprDirector : public JXWindowDirector
{
public:

	ExprDirector(JXDirector* supervisor, const VarList* varList);
	ExprDirector(std::istream& input, const JFileVersion vers,
				   JXDirector* supervisor, const VarList* varList);

	virtual ~ExprDirector();

	virtual void	Activate() override;

	void	UpdateDisplay();
	void	WriteState(std::ostream& output) const;

	static void	ReadPrefs(std::istream& input, const JFileVersion vers);
	static void	WritePrefs(std::ostream& output);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ExprEditor*	itsExprWidget;
	TapeText*	itsTapeWidget;
	JXTextMenu*		itsActionsMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	static JString	theDefGeom;			// window size + partition geometry
	JString			itsTapeName;		// file in which user last saved tape

// begin JXLayout

	JXVertPartition* itsPartition;
	KeyPad*          itsKeyPad;

// end JXLayout

private:

	void	BuildWindow(const VarList* varList);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	SaveTape();

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UseDefaultGeometry();
	void	SetDefaultGeometry();
};

#endif
