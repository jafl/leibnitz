/******************************************************************************
 VarList.h

	Interface for VarList class.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_VarList
#define _H_VarList

#include <jx-af/jexpr/JVariableList.h>
#include <jx-af/jcore/JPtrArray.h>

class JString;
class JFunction;
class JConstantValue;

class VarList : public JVariableList
{
	friend std::ostream& operator<<(std::ostream&, const VarList&);

public:

	enum
	{
		kXIndex = 1,
		kYIndex = 2,
		kTIndex = 3,

		kUserFnOffset = 3
	};

public:

	VarList();
	VarList(std::istream& input, const JFileVersion vers);

	~VarList() override;

	JIndex	NewFunction();
	bool	SetVariableName(const JIndex index, const JString& name);

	const JFunction*	GetFunction(const JIndex index) const;
	bool				SetFunction(const JIndex index, const JString& expr);

	bool	OKToRemoveFunction(const JIndex index) const;
	void	RemoveFunction(const JIndex index);

// implementation of JVariableList

	const JString&	GetVariableName(const JIndex index) const override;
	void			GetVariableName(const JIndex index, JString* name,
									JString* subscript) const override;

	bool	IsArray(const JIndex index) const override;
	bool	ArrayIndexValid(const JIndex variableIndex,
								const JIndex itemIndex) const override;

	bool	GetNumericValue(const JIndex variableIndex,
							const JIndex itemIndex,
							JFloat* value) const override;
	bool	GetNumericValue(const JIndex variableIndex,
							const JIndex itemIndex,
							JComplex* value) const override;

	void	SetNumericValue(const JIndex variableIndex,
							const JIndex itemIndex,
							const JFloat value) override;
	void	SetNumericValue(const JIndex variableIndex,
							const JIndex itemIndex,
							const JComplex& value) override;

private:

	JPtrArray<JString>*		itsNames;
	JPtrArray<JFunction>*	itsFunctions;

	// store extra copies to avoid downcasting

	JConstantValue*	itsXValue;
	JConstantValue*	itsYValue;
	JConstantValue*	itsTValue;

private:

	void	VarListX();

	// not allowed

	VarList(const VarList&) = delete;
};


/******************************************************************************
 GetFunction

 ******************************************************************************/

inline const JFunction*
VarList::GetFunction
	(
	const JIndex index
	)
	const
{
	return itsFunctions->GetItem(index);
}

/******************************************************************************
 OKToRemoveFunction

 ******************************************************************************/

inline bool
VarList::OKToRemoveFunction
	(
	const JIndex index
	)
	const
{
	return index > kUserFnOffset &&
				 OKToRemoveVariable(index);
}

#endif
