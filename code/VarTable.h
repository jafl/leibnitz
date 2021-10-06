/******************************************************************************
 VarTable.h

	Interface for the VarTable class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarTable
#define _H_VarTable

#include <jx-af/jx/JXEditTable.h>

class JXTextMenu;
class JXExprInput;
class VarList;

class VarTable : public JXEditTable
{
public:

	enum
	{
		kNameColumn = 1,
		kFnColumn   = 2
	};

public:

	VarTable(VarList*	varList, JXTextMenu* fontMenu,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~VarTable();

	void	NewConstant();

	bool	OKToRemoveSelectedConstant() const;
	void	RemoveSelectedConstant();

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	VarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// nullptr unless editing
	JXTextMenu*		itsFontMenu;
	JString			itsOrigText;	// used while itsTextInput != nullptr

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();
};

#endif
