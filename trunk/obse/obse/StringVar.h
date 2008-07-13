#pragma once
#include "Serialization.h"
#include "GameAPI.h"

// String changes layout:
//
//	STVS - empty chunk indicating start of strings block
//		STVR
//			UInt8 modIndex
//			UInt32 stringID
//			UInt16 length
//			char data[length]
//		[STVR]
//		...
//	STVE - empty chunk indicating end of strings block
//
// Strings are discarded on load if the mod which created them is no longer present.

class StringVar
{
	std::string data;
	UInt8		owningModIndex;
public:
	StringVar(const char* in_data, UInt32 in_refID);
	const char*	GetCString();
	void		Set(const char* newString);
	SInt32		Compare(char* rhs, bool caseSensitive);
	void		Insert(char* subString, UInt32 insertionPos);
	UInt32		Find(char* subString, UInt32 startPos, UInt32 endPos = -1, bool bCaseSensitive = false);	//returns position of substring
	UInt32		Count(char* subString, UInt32 startPos, UInt32 endPos = -1, bool bCaseSensitive = false);
	UInt32		Replace(char* toReplace, char* replaceWith, UInt32 startPos, UInt32 endPos, UInt32 numToReplace = -1);	//returns num replaced
	UInt32		GetLength();
	void		Erase(UInt32 startPos = 0, UInt32 endPos = -1);
	std::string	SubString(UInt32 startPos, UInt32 endPos);
	double*		ToFloat(UInt32 startPos, UInt32 endPos);
	char		At(UInt32 charPos);
	static UInt32	GetCharType(char ch);
	UInt8		GetOwningModIndex();
};

enum {
	kCharType_Alphabetic	= 1 << 0,
	kCharType_Digit			= 1 << 1,
	kCharType_Punctuation	= 1 << 2,
	kCharType_Printable		= 1 << 3,
	kCharType_Uppercase		= 1 << 4,
};

class StringVarMap 
{
	UInt32 GetUnusedID();

	std::map<UInt32, StringVar*> strings;
public:
	StringVarMap();
	~StringVarMap();

	void Save(OBSESerializationInterface* intfc);
	void Load(OBSESerializationInterface* intfc);
	void Reset(OBSESerializationInterface* intfc);
	StringVar* Get(UInt32 strID);
	UInt32 Add(UInt8 varModIndex, const char* data);	//creates new string, returns ID
	bool StringExists(UInt32 strID);
};

extern StringVarMap g_StringMap;


