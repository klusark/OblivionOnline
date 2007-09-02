#pragma once

#include "common/ITypes.h"

void _AssertionFailed(const char * file, UInt32 line, const char * desc);
void _AssertionFailed_ErrCode(const char * file, UInt32 line, const char * desc, UInt64 code);
void _AssertionFailed_ErrCode(const char * file, UInt32 line, const char * desc, const char * code);

//! Exit the program if the condition is not true
#define ASSERT(a)					do { if(!(a)) _AssertionFailed(__FILE__, __LINE__, #a); } while(0)
//! Exit the program if the condition is not true, with an error message
#define ASSERT_STR(a, b)			do { if(!(a)) _AssertionFailed(__FILE__, __LINE__, #b); } while(0)
//! Exit the program if the condition is not true, reporting an error code
#define ASSERT_CODE(a, b)			do { if(!(a)) _AssertionFailed_ErrCode(__FILE__, __LINE__, #a, b); } while(0)
//! Exit the program if the condition is not true, reporting an error code and message
#define ASSERT_STR_CODE(a, b, c)	do { if(!(a)) _AssertionFailed_ErrCode(__FILE__, __LINE__, #b, c); } while(0)
//! Exit the program with an error message
#define HALT(a)						do { _AssertionFailed(__FILE__, __LINE__, #a); } while(0)
//! Exit the program with and error code and message
#define HALT_CODE(a, b)				do { _AssertionFailed_ErrCode(__FILE__, __LINE__, #a, b); } while(0)

#define THROW_EXCEPTION(type, desc)		do { throw type##(__FILE__, __LINE__, desc); } while(0)
#define EXCEPTION_DEF_CONSTRUCTOR(type)	type##(char * inFile, int inLine, char * inError) : IException(inFile, inLine, inError) { }
#define DEF_EXCEPTION(type)				class type : public IException { public: EXCEPTION_DEF_CONSTRUCTOR(type) virtual ~##type##() { } }

// based on the boost implementation of static asserts
template <bool x> struct StaticAssertFailure;
template <> struct StaticAssertFailure <true> { enum { a = 1 }; };
template <int x> struct static_assert_test { };

#define MACRO_JOIN(a, b)	MACRO_JOIN_2(a, b)
#define MACRO_JOIN_2(a, b)	MACRO_JOIN_3(a, b)
#define MACRO_JOIN_3(a, b)	a##b

#define STATIC_ASSERT(a)	typedef static_assert_test <sizeof(StaticAssertFailure<(bool)(a)>)> MACRO_JOIN_3(static_assert_typedef_, __COUNTER__)

/**
 *	The base exception class
 */
class IException
{
	public:
		IException(char * inFile, int inLine, char * inError)	{ file = inFile; line = inLine; error = inError; }
		virtual ~IException()									{ }

	protected:
		char	* file;
		int		line;
		char	* error;
};
