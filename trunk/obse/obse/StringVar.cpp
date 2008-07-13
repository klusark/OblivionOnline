#include <string>
#include <map>
#include "StringVar.h"
#include "GameForms.h"

StringVar::StringVar(const char* in_data, UInt32 in_refID)
{
	data = std::string(in_data);
	owningModIndex = in_refID >> 24;
}

const char* StringVar::GetCString()
{
	return data.c_str();
}

void StringVar::Set(const char* newString)
{
	data = std::string(newString);
}

SInt32 StringVar::Compare(char* rhs, bool caseSensitive)
{
	SInt32 cmp = 0;
	if (!caseSensitive)
	{
		cmp = _stricmp(data.c_str(), rhs);
		if (cmp < 0)
			return -1;
		else if (cmp > 0)
			return 1;
		else
			return 0;
	}
	else
	{
		std::string str2(rhs);
		if (data == str2)
			return 0;
		else if (data < str2)
			return -1;
		else
			return 1;
	}
}

void StringVar::Insert(char* subString, UInt32 insertionPos)
{
	if (insertionPos < GetLength())
		data.insert(insertionPos, subString);
}

UInt32 StringVar::Find(char* subString, UInt32 startPos, UInt32 endPos, bool bCaseSensitive)
{
	return data.find(subString, startPos);	//returns -1 if not found
}

UInt32 StringVar::Count(char* subString, UInt32 startPos, UInt32 endPos, bool bCaseSensitive)
{
	if (endPos == -1)
		endPos = GetLength();

	UInt32 strIdx = startPos;
	bool bContinue = true;
	UInt32 count = 0;
	while (bContinue && (strIdx = data.find(subString, strIdx) != -1 && strIdx < endPos))
		count++;

	return count;
}

UInt32 StringVar::GetLength()
{
	return data.length();
}

UInt32 StringVar::Replace(char* toReplace, char* replaceWith, UInt32 startPos, UInt32 endPos, UInt32 numToReplace)
{
	UInt32 numReplaced = 0;
	UInt32 strIdx = startPos;
	bool bContinue = true;

	while (bContinue && (strIdx = data.find(toReplace, strIdx)) != -1 && strIdx < endPos)
	{
		numReplaced++;
		numToReplace--;

		data.erase(strIdx, strlen(toReplace));
		Insert(replaceWith, strIdx);

		if (!numToReplace)
			bContinue = false;
		else
			strIdx += strlen(replaceWith);
	}

	return numReplaced;
}

void StringVar::Erase(UInt32 startPos, UInt32 endPos)
{
	if (endPos == -1)
		endPos = GetLength();
	if (startPos < endPos)
		data.erase(startPos, endPos - startPos);
}

std::string StringVar::SubString(UInt32 startPos, UInt32 endPos)
{
	if (endPos == -1)
		endPos = GetLength() - startPos;

	return data.substr(startPos, endPos - startPos);
}

UInt8 StringVar::GetOwningModIndex()
{
	return owningModIndex;
}

UInt32 StringVar::GetCharType(char ch)
{
	UInt32 charType = 0;
	if (isalpha(ch))
		charType |= kCharType_Alphabetic;
	if (isdigit(ch))
		charType |= kCharType_Digit;
	if (ispunct(ch))
		charType |= kCharType_Punctuation;
	if (isprint(ch))
		charType |= kCharType_Printable;
	if (isupper(ch))
		charType |= kCharType_Uppercase;

	return charType;
}

char StringVar::At(UInt32 charPos)
{
	if (charPos < GetLength())
		return data[charPos];
	else
		return -1;
}

StringVarMap::StringVarMap()
{
	//
}

StringVarMap::~StringVarMap()
{
	//
}

UInt32 StringVarMap::GetUnusedID()
{	//could probably be made faster
	UInt32 id = 1;
	while (strings.find(id) != strings.end())
	{
		id++;
	}
	return id;
}

bool StringVarMap::StringExists(UInt32 strID)
{
	if (strID == 0)
		return false;
	else if (Get(strID))
		return true;
	else
	{
		_MESSAGE("Error: initialized string %d not found", strID);
		return false;
	}
}

StringVar* StringVarMap::Get(UInt32 strID)
{
	if (strID == 0)
		return NULL;
	else
		return strings[strID];
}

UInt32 StringVarMap::Add(UInt8 varModIndex, const char* data)
{
	UInt32 strID = GetUnusedID();
	strings[strID] = new StringVar(data, varModIndex);

	return strID;
}

void StringVarMap::Save(OBSESerializationInterface* intfc)
{
	intfc->OpenRecord('STVS', 0);

	for (std::map<UInt32, StringVar*>::iterator iter = strings.begin();
			iter != strings.end();
			iter++)
	{
		intfc->OpenRecord('STVR', 0);
		UInt8 modIndex = iter->second->GetOwningModIndex();
		intfc->WriteRecordData(&modIndex, sizeof(UInt8));
		intfc->WriteRecordData(&iter->first, sizeof(UInt32));
		UInt16 len = iter->second->GetLength();
		intfc->WriteRecordData(&len, sizeof(len));
		intfc->WriteRecordData(iter->second->GetCString(), len);
	}

	intfc->OpenRecord('STVE', 0);
}

void StringVarMap::Load(OBSESerializationInterface* intfc)
{
	_MESSAGE("Loading strings");
	UInt32 type, length, version, stringID, tempRefID;
	UInt16 strLength;
	UInt8 modIndex;
	char buffer[kMaxMessageLength] = { 0 };

	Reset(intfc);
	bool bContinue = true;
	while (bContinue && intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		case 'STVE':			//end of block
			bContinue = false;
			break;
		case 'STVR':
			intfc->ReadRecordData(&modIndex, sizeof(modIndex));
			if (!intfc->ResolveRefID(modIndex << 24, &tempRefID))
			{
				_MESSAGE("RefID %08x resolved to %08x returns false", modIndex << 24, tempRefID);
				continue;
			}

			intfc->ReadRecordData(&stringID, sizeof(stringID));
			intfc->ReadRecordData(&strLength, sizeof(strLength));
			
			intfc->ReadRecordData(buffer, strLength);
			buffer[strLength] = 0;

			strings[stringID] = new StringVar(buffer, tempRefID);
			break;
		default:
			_MESSAGE("Error loading string map: unhandled chunk type %d", type);
			break;
		}
	}
}

void StringVarMap::Reset(OBSESerializationInterface* intfc)
{
	for (std::map<UInt32, StringVar*>::iterator iter = strings.begin();
			iter != strings.end();
			++iter)
	{
		delete iter->second;
	}

	strings.clear();
}

StringVarMap g_StringMap;