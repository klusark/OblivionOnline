#include "obse/GameTypes.h"
#include "obse/GameAPI.h"
#include <string>
#include <algorithm>

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

NiTPointerMap <TESForm>			* g_formTable = (NiTPointerMap <TESForm> *)0x00AEDE44;
NiTPointerMap <EffectSetting>	* g_EffectSettingCollection = (NiTPointerMap <EffectSetting> *)0x00AEB380;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

NiTPointerMap <TESForm>			* g_formTable = (NiTPointerMap <TESForm> *)0x00B0613C;
NiTPointerMap <EffectSetting>	* g_EffectSettingCollection = (NiTPointerMap <EffectSetting> *)0x00B33508;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

NiTPointerMap <TESForm>			* g_formTable = (NiTPointerMap <TESForm> *)0x00B0613C;
NiTPointerMap <EffectSetting>	* g_EffectSettingCollection = (NiTPointerMap <EffectSetting> *)0x00B33508;

#else
#error unsupported version of oblivion
#endif

/*** String ***/

bool String::Set(const char * src)
{
	if (!src) {
		FormHeap_Free(m_data);
		m_data = 0;
		m_bufLen = 0;
		return true;
	}
	
	UInt32	srcLen = strlen(src);

	// realloc if needed
	if(srcLen > m_bufLen)
	{
		FormHeap_Free(m_data);
		m_data = (char *)FormHeap_Allocate(srcLen + 1);
		m_bufLen = m_data ? srcLen : 0;
	}

	if(m_data)
	{
		strcpy_s(m_data, m_bufLen + 1, src);
		m_dataLen = srcLen;
	}
	else
	{
		m_dataLen = 0;
	}

	return m_data != NULL;
}

bool ci_equal(char ch1, char ch2)
{
	return tolower((unsigned char)ch1) == tolower((unsigned char)ch2);
}

bool String::Includes(const char *toFind) const
{
	std::string curr(m_data, m_dataLen);
	std::string str2(toFind);
	std::string::iterator currEnd = curr.end();
	return (std::search(curr.begin(), currEnd, str2.begin(), str2.end(), ci_equal) != currEnd);
}

bool String::Replace(const char *_toReplace, const char *_replaceWith)
{
	std::string curr(m_data, m_dataLen);
	std::string toReplace(_toReplace);

	std::string::iterator currBegin = curr.begin();
	std::string::iterator currEnd = curr.end();
	std::string::iterator replaceIt = std::search(currBegin, currEnd, toReplace.begin(), toReplace.end(), ci_equal);
	if (replaceIt != currEnd) {
		std::string replaceWith(_replaceWith);
		// we found the substring, now we need to do the modification
		std::string::size_type replacePos = distance(currBegin, replaceIt);
		curr.replace(replacePos, toReplace.size(), replaceWith);
		Set(curr.c_str());
		return true;
	}
	return false;
}

bool String::Append(const char* toAppend)
{
	std::string curr(m_data, m_dataLen);
	curr += toAppend;
	Set(curr.c_str());
	return true;
}