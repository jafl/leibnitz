/******************************************************************************
 BaseConvDirector.h

	Interface for the BaseConvDirector class

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_BaseConvDirector
#define _H_BaseConvDirector

#include <jx-af/jx/JXWindowDirector.h>

class JString;
class JXStaticText;
class JXTextButton;
class JXInputField;
class BaseConvMenu;

const JSize kBaseCount = 4;

class BaseConvDirector : public JXWindowDirector
{
public:

	BaseConvDirector(JXDirector* supervisor);
	BaseConvDirector(std::istream& input, const JFileVersion vers,
						JXDirector* supervisor);

	virtual ~BaseConvDirector();

	void	WriteState(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool			itsIgnoreTextFlag;
	JXInputField*	itsInput[ kBaseCount ];

// begin JXLayout

	JXTextButton* itsCloseButton;
	JXInputField* its10Input;
	JXTextButton* itsHelpButton;
	JXInputField* its2Input;
	JXInputField* its8Input;
	JXInputField* its16Input;

// end JXLayout

private:

	void	BuildWindow();
	void	Convert(JBroadcaster* sender);
};

#endif
