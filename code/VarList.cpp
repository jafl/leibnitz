/******************************************************************************
 VarList.cpp

	BASE CLASS = JVariableList

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "VarList.h"
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JUserInputFunction.h>
#include <jx-af/jexpr/JConstantValue.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

static const JString kXName("x");
static const JString kYName("y");
static const JString kTName("t");

/******************************************************************************
 Constructor

 ******************************************************************************/

VarList::VarList()
	:
	JVariableList()
{
	VarListX();
}

VarList::VarList
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	:
	JVariableList()
{
	VarListX();

	JSize varCount;
	input >> varCount;

	JArray<JIndex> misfitIndexList;
	JPtrArray<JString> misfitFnList(JPtrArrayT::kDeleteAll);

	JGetUserNotification()->SetSilent(true);		// complain the second time

	JExprParser p(this);

	for (JIndex i=1; i<=varCount; i++)
	{
		auto* name = jnew JString;
		input >> *name;
		name->Set(JUserInputFunction::ConvertToGreek(*name));
		itsNames->Append(name);

		auto* fStr = jnew JString;
		input >> *fStr;
		JFunction* f;
		if (p.Parse(*fStr, &f))
		{
			jdelete fStr;
			itsFunctions->Append(f);
		}
		else
		{
			itsFunctions->Append(nullptr);
			misfitIndexList.AppendElement(i + kUserFnOffset);
			misfitFnList.Append(fStr);
		}
	}

	JGetUserNotification()->SetSilent(false);

	const JSize misfitCount = misfitIndexList.GetElementCount();
	for (JIndex i=1; i<=misfitCount; i++)
	{
		const JString* fStr = misfitFnList.GetElement(i);
		JFunction* f;
		if (p.Parse(*fStr, &f))
		{
			const JIndex j = misfitIndexList.GetElement(i);
			itsFunctions->SetElement(j, f, JPtrArrayT::kDelete);
		}
	}
}

// private

void
VarList::VarListX()
{
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNames != nullptr );

	itsFunctions = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != nullptr );

	InstallCollection(itsNames);

	// variables for plotting

	itsNames->Append(kXName);

	itsXValue = jnew JConstantValue(0.0);
	itsFunctions->Append(itsXValue);

	itsNames->Append(kYName);

	itsYValue = jnew JConstantValue(0.0);
	itsFunctions->Append(itsYValue);

	itsNames->Append(kTName);

	itsTValue = jnew JConstantValue(0.0);
	itsFunctions->Append(itsTValue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarList::~VarList()
{
	jdelete itsNames;
	jdelete itsFunctions;
}

/******************************************************************************
 NewFunction

 ******************************************************************************/

JIndex
VarList::NewFunction()
{
	auto* name = jnew JString;

	JUInt64 i = 1;
	JIndex j;
	do
	{
		*name = JGetString("NewVarName::VarList");
		if (i > 1)
		{
			*name += JString(i);
		}
		i++;
	}
		while (ParseVariableName(*name, &j));

	itsNames->Append(name);

	JFunction* f = jnew JConstantValue(0.0);
	itsFunctions->Append(f);

	const JIndex varIndex = GetElementCount();
	Broadcast(VarInserted(varIndex));
	return varIndex;
}

/******************************************************************************
 RemoveFunction

 ******************************************************************************/

void
VarList::RemoveFunction
	(
	const JIndex index
	)
{
	assert( index > kUserFnOffset );

	itsNames->DeleteElement(index);
	itsFunctions->DeleteElement(index);
	Broadcast(VarRemoved(index));
}

/******************************************************************************
 SetVariableName

 ******************************************************************************/

bool
VarList::SetVariableName
	(
	const JIndex	varIndex,
	const JString&	name
	)
{
	assert( varIndex > kUserFnOffset );

	JIndex index;
	if (!JVariableList::NameValid(name))
	{
		return false;
	}
	else if (ParseVariableName(name, &index) && index != varIndex)
	{
		JGetUserNotification()->ReportError(JGetString("NameUsed::VarList"));
		return false;
	}
	else
	{
		JString* varName = itsNames->GetElement(varIndex);
		*varName = name;
		Broadcast(VarNameChanged(varIndex));
		return true;
	}
}

/******************************************************************************
 SetFunction

 ******************************************************************************/

bool
VarList::SetFunction
	(
	const JIndex	index,
	const JString&	expr
	)
{
	assert( index > kUserFnOffset );

	JExprParser p(this);

	JFunction* f;
	if (p.Parse(expr, &f))
	{
		itsFunctions->SetElement(index, f, JPtrArrayT::kDelete);
		Broadcast(VarValueChanged(index,1));
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Virtual functions for VarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
VarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNames->GetElement(index));
}

void
VarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	const JString* fullName = itsNames->GetElement(index);
	if (fullName->GetCharacterCount() > 1)
	{
		*name      = fullName->GetFirstCharacter();
		*subscript = *fullName;

		JStringIterator iter(subscript);
		iter.RemoveNext(1);
	}
	else
	{
		*name = *fullName;
		subscript->Clear();
	}
}

/******************************************************************************
 IsArray

 ******************************************************************************/

bool
VarList::IsArray
	(
	const JIndex index
	)
	const
{
	return false;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

bool
VarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return elementIndex == 1;
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

bool
VarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	const JFunction* f = itsFunctions->GetElement(variableIndex);
	if (elementIndex == 1)
	{
		if (IsOnEvalStack(variableIndex))
		{
			*value = 0.0;
			return false;
		}
		PushOnEvalStack(variableIndex);
		const bool ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
	}
	else
	{
		*value = 0.0;
		return false;
	}
}

bool
VarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JComplex*		value
	)
	const
{
	const JFunction* f = itsFunctions->GetElement(variableIndex);
	if (elementIndex == 1)
	{
		if (IsOnEvalStack(variableIndex))
		{
			*value = 0.0;
			return false;
		}
		PushOnEvalStack(variableIndex);
		const bool ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
	}
	else
	{
		*value = 0.0;
		return false;
	}
}

/******************************************************************************
 SetNumericValue

 ******************************************************************************/

void
VarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JFloat	value
	)
{
	assert( elementIndex == 1 );

	if (variableIndex == kXIndex)
	{
		itsXValue->SetValue(value);
	}
	else if (variableIndex == kYIndex)
	{
		itsYValue->SetValue(value);
	}
	else if (variableIndex == kTIndex)
	{
		itsTValue->SetValue(value);
	}
	else
	{
		assert_msg( 0, "VarList::SetNumericValue() called for a function" );
	}
}

void
VarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JComplex&	value
	)
{
	assert_msg( 0, "VarList does not store complex values" );
}

/******************************************************************************
 Global functions for VarList class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::ostream&
operator<<
	(
	std::ostream&			output,
	const VarList&	varList
	)
{
	const JSize varCount = (varList.itsNames)->GetElementCount();
	output << varCount - VarList::kUserFnOffset;

	for (JIndex i = 1+VarList::kUserFnOffset; i<=varCount; i++)
	{
		const JString* name = (varList.itsNames)->GetElement(i);
		output << ' ' << *name;

		const JFunction* f = (varList.itsFunctions)->GetElement(i);
		output << ' ' << f->Print();
	}

	// allow chaining

	return output;
}
