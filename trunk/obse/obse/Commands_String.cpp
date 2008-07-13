#include "GameAPI.h"
#include "StringVar.h"
#include "ParamInfos.h"
#include "obse/Commands_String.h"
#include <string>

#if OBLIVION

static bool Cmd_sv_Construct_Execute(COMMAND_ARGS)
{
	*result = 0;
	char buffer[kMaxMessageLength] = {0};
	double strID = 0;
	UInt8 modIndex = 0;

	if (!ExtractSetStatementVar(scriptObj, eventList, arg1, &strID, &modIndex))
		return true;
	else if(!ExtractFormatStringArgs(0, buffer, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, kCommandInfo_sv_Construct.numParams))
		return true;

	StringVar* strVar = g_StringMap.Get(strID);
	if (strVar)
		strVar->Set(buffer);
	else
		strID = g_StringMap.Add(modIndex, buffer);

	*result = strID;
	return true;
}

static bool Cmd_sv_SubString_Execute(COMMAND_ARGS)
{
	*result = 0;
	double lhStrID = 0;
	UInt8 modIndex = 0;
	float rhStrID = 0;
	float startPos = 0;
	float endPos = -1;

	if (!ExtractSetStatementVar(scriptObj, eventList, arg1, &lhStrID, &modIndex))
		return true;
	else if (!ExtractArgs(PASS_EXTRACT_ARGS, &rhStrID, &startPos, &endPos))
		return true;
	
	StringVar* rhVar = g_StringMap.Get(rhStrID);
	if (!rhVar)
		return true;

	if (endPos == -1)
		endPos = rhVar->GetLength();

	std::string subStr = rhVar->SubString(startPos, endPos);
	StringVar* lhVar = g_StringMap.Get(lhStrID);
	if (lhVar)
		lhVar->Set(subStr.c_str());
	else
		lhStrID = g_StringMap.Add(modIndex, subStr.c_str());

	*result = lhStrID;
	return true;
}

static bool Cmd_sv_Compare_Execute(COMMAND_ARGS)
{
	*result = 0xFFFF;	//sentinel value if comparison fails
	UInt32 stringID = 0;
	char buffer[kMaxMessageLength] = { 0 };
	float bCaseSensitive = 0;

	if (!ExtractFormatStringArgs(1, buffer, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, kCommandInfo_sv_Compare.numParams, &stringID, &bCaseSensitive))
		return true;

	StringVar* lhs = g_StringMap.Get(stringID);
	if (!lhs)
		return true;

	*result = lhs->Compare(buffer, bCaseSensitive ? true : false);

	return true;
}

static bool Cmd_sv_Length_Execute(COMMAND_ARGS)
{
	*result = -1;
	UInt32 strID = 0;
	if (ExtractArgs(EXTRACT_ARGS, &strID))
	{
		StringVar* str = g_StringMap.Get(strID);
		if (str)
			*result = str->GetLength();
	}

	return true;
}

static bool Cmd_sv_Erase_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 strID = 0;
	float startPos = 0;
	float endPos = -1;

	if (!ExtractArgs(PASS_EXTRACT_ARGS, &strID, &startPos, &endPos))
		return true;

	StringVar* strVar = g_StringMap.Get(strID);
	if (strVar)
	{
		if (endPos = -1)
			endPos = strVar->GetLength();
		strVar->Erase(startPos, endPos);
	}

	return true;
}

enum {
	eMode_svFind,
	eMode_svCount,
	eMode_svReplace,
};

static bool StringVar_Find_Execute(COMMAND_ARGS, UInt32 mode)
{
	const char separatorChar = '@';	//can't type '|' at console so use this for testing
	*result = -1;
	UInt32 strID = 0;
	float startPos = 0;
	float endPos = -1;
	float bCaseSensitive = 0;
	float howMany = 0;
	char toFind[kMaxMessageLength] = { 0 };

	if (!ExtractFormatStringArgs(1, toFind, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, kCommandInfo_sv_Find.numParams, &strID, &startPos, &endPos, &bCaseSensitive, &howMany))
		return true;

	Console_Print("start: %.2f end: %.2f bCase: %.2f howMany: %.2f", (double)startPos, (double)endPos, (double)bCaseSensitive, (double)howMany);	//DELETEME

	StringVar* strVar = g_StringMap.Get(strID);
	if (strVar)
	{
		switch (mode)
		{
		case eMode_svFind:
			*result = strVar->Find(toFind, startPos, endPos, bCaseSensitive ? true : false);
			break;
		case eMode_svCount:
			*result = strVar->Count(toFind, startPos, endPos, bCaseSensitive ? true : false);
			break;
		case eMode_svReplace:
			{
				std::string str(toFind);
				UInt32 splitPoint = str.find(separatorChar);
				if (splitPoint != -1 && splitPoint < str.length() - 1)
				{
					toFind[splitPoint] = '\0';
					*result = strVar->Replace(toFind, toFind + splitPoint + 1, startPos, endPos, howMany);
				}
				break;
			}
		}
	}

	return true;
}

static bool Cmd_sv_Find_Execute(COMMAND_ARGS)
{
	StringVar_Find_Execute(PASS_COMMAND_ARGS, eMode_svFind);
	return true;
}

static bool Cmd_sv_Count_Execute(COMMAND_ARGS)
{
	StringVar_Find_Execute(PASS_COMMAND_ARGS, eMode_svCount);
	return true;
}

static bool Cmd_sv_Replace_Execute(COMMAND_ARGS)
{
	StringVar_Find_Execute(PASS_COMMAND_ARGS, eMode_svReplace);
	return true;
}

static bool Cmd_sv_ToNumeric_Execute(COMMAND_ARGS)
{
	UInt32 strID = 0;
	UInt32 startPos = 0;
	*result = 0;

	if (!ExtractArgs(PASS_EXTRACT_ARGS, &strID, &startPos))
		return true;

	StringVar* strVar = g_StringMap.Get(strID);
	if (strVar)
	{
		const char* cStr = strVar->GetCString();
		*result = strtod(cStr + startPos, NULL);
	}

	return true;
}

static bool Cmd_sv_Insert_Execute(COMMAND_ARGS)
{
	UInt32 strID = 0;
	float insertionPos = 0;
	char subString[kMaxMessageLength] = { 0 };
	*result = 0;

	if (!ExtractFormatStringArgs(1, subString, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, kCommandInfo_sv_Insert.numParams, &strID, &insertionPos))
		return true;

	StringVar* lhs = g_StringMap.Get(strID);
	if (lhs)
		lhs->Insert(subString, insertionPos);

	return true;
}

static bool Cmd_sv_GetChar_Execute(COMMAND_ARGS)
{
	UInt32 strID = 0;
	UInt32 charPos = 0;
	*result = 0;

	if (!ExtractArgs(PASS_EXTRACT_ARGS, &strID, &charPos))
		return true;

	StringVar* strVar = g_StringMap.Get(strID);
	if (strVar)
		*result = strVar->At(charPos);

	return true;
}

static bool MatchCharType_Execute(COMMAND_ARGS, UInt32 mask)
{
	UInt32 charCode = 0;
	*result = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &charCode))
		if ((StringVar::GetCharType(charCode) & mask) == mask)
			*result = 1;

	return true;
}

static bool Cmd_IsLetter_Execute(COMMAND_ARGS)
{
	return MatchCharType_Execute(PASS_COMMAND_ARGS, kCharType_Alphabetic);
}

static bool Cmd_IsDigit_Execute(COMMAND_ARGS)
{
	return MatchCharType_Execute(PASS_COMMAND_ARGS, kCharType_Digit);
}

static bool Cmd_IsPunctuation_Execute(COMMAND_ARGS)
{
	return MatchCharType_Execute(PASS_COMMAND_ARGS, kCharType_Punctuation);
}

static bool Cmd_IsPrintable_Execute(COMMAND_ARGS)
{
	return MatchCharType_Execute(PASS_COMMAND_ARGS, kCharType_Printable);
}

static bool Cmd_IsUpperCase_Execute(COMMAND_ARGS)
{
	return MatchCharType_Execute(PASS_COMMAND_ARGS, kCharType_Uppercase);
}

#endif

static ParamInfo kParams_sv_Construct[21] =
{
	FORMAT_STRING_PARAMS,
};

DEFINE_COMMAND(sv_Construct,
			   returns a formatted string,
			   0,
			   21,
			   kParams_sv_Construct);

static ParamInfo kParams_sv_Compare[23] =
{
	{	"stringVar",		kParamType_StringVar,	0	},
	FORMAT_STRING_PARAMS,
	{	"bCaseSensitive",	kParamType_Float,		1	},
};

DEFINE_COMMAND(sv_Compare,
			   compares two strings,
			   0,
			   23,
			   kParams_sv_Compare);

DEFINE_COMMAND(sv_Length,
			   returns the number of characters in a string,
			   0,
			   1,
			   kParams_OneInt);

static ParamInfo kParams_SubString[3] =
{
	{	"stringVar",	kParamType_Integer,	0	},
	{	"startPos",		kParamType_Integer,	1	},
	{	"howMany",		kParamType_Integer,	1	},
};

DEFINE_COMMAND(sv_Erase,
			   erases a portion of a string variable,
			   0,
			   3,
			   kParams_SubString);

DEFINE_COMMAND(sv_SubString,
			   returns a substring of a string variable,
				0,
				3,
				kParams_SubString);

static ParamInfo kParams_sv_ToNumeric[2] = 
{
	{	"stringVar",	kParamType_Integer,	0	},
	{	"startPos",		kParamType_Integer,	1	},
};

DEFINE_COMMAND(sv_ToNumeric,
			   converts a string variable to a numeric type,
			   0,
			   2,
			   kParams_sv_ToNumeric);

DEFINE_COMMAND(sv_Insert,
			   inserts a substring in a string variable,
			   0,
			   23,
			   kParams_sv_Compare);

static ParamInfo kParams_sv_Find[25] =
{
	{	"stringVar",	kParamType_Integer,		0	},
	FORMAT_STRING_PARAMS,
	{	"startPos",		kParamType_Float,		1	},
	{	"endPos",		kParamType_Float,		1	},
	{	"bCaseSensitive", kParamType_Float,		1	},
};

DEFINE_COMMAND(sv_Count,
			   returns the number of occurences of a substring within a string variable,
			   0,
			   23,
			   kParams_sv_Find);

DEFINE_COMMAND(sv_Find,
			   returns the position of a substring within a string variable or -1 if not found,
			   0,
			   25,
			   kParams_sv_Find);

static ParamInfo kParams_sv_Replace[26] =
{
	{	"stringVar",	kParamType_Integer,		0	},
	FORMAT_STRING_PARAMS,
	{	"startPos",		kParamType_Float,		1	},
	{	"endPos",		kParamType_Float,		1	},
	{	"bCaseSensitive", kParamType_Float,		1	},
	{	"howMany",		kParamType_Float,		1	},
};

DEFINE_COMMAND(sv_Replace,
			   replaces 1 or more occurences of a substring within a string variable,
			   0,
			   26,
			   kParams_sv_Replace);

DEFINE_COMMAND(IsLetter,
			   returns 1 if the character is an alphabetic character,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(IsDigit,
			   returns 1 if the character is a numeric character,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(IsPrintable,
			   returns 1 if the character is a printable character,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(IsPunctuation,
			   returns 1 if the character is a punctuation character,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(IsUpperCase,
			   returns 1 if the character is an uppercase letter,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(sv_GetChar,
			   returns the ASCII code of the character at the specified position in a string variable,
			   0,
			   2,
			   kParams_TwoInts);
