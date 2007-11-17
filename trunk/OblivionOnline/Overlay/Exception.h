/* These Files has been edited by masterfreek64. They have been changed to remove the dependency on the BOOST library. */
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <exception>
#include <windows.h>
#include <dxerr9.h>
#include <string>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WIDE_FILE__ WIDEN(__FILE__)

#ifndef __TFILE__
#ifdef UNICODE
#define __TFILE__ __WIDE_FILE__
#else
#define __TFILE__ __FILE__
#endif
#endif

// Get the library
#pragma comment(lib, "dxerr9.lib")

/*
Macro: HRE
Checks HRESULT values for success, and throws <DirectX::Exception> on failure.

Usage:
HRE(device->DrawPrimitive(...));
*/
#define HRE(expression) DirectX::CheckHr<>::Test((expression), __TFILE__, __LINE__, #expression)

/*
Macro: HREX
Checks HRESULT values for success, and throws an exception of a type specified by the caller.

Usage:
HREX(DirectX::Exception, device->DrawPrimitive(...));
*/
#define HREX(exceptionType, expression) \
	DirectX::CheckHr<exceptionType>::Test((expression), __TFILE__, __LINE__, #expression)

namespace DirectX
{
	struct Exception : public std::exception
	{
		HRESULT hr;
		std::basic_string<TCHAR> hrString, hrDescription;
		std::basic_string<TCHAR> fileName;
		DWORD line;
		std::string expression;

		Exception(const char* what) : std::exception(what), hr(S_OK), line(0)
		{
		}
	};

	/*
	Struct: CheckHr
		We actually want a function, but default template parameters are allowed on
		classes/structs only. We'll	use a static function inside the structure.
	*/
	template<typename T = Exception>
	struct CheckHr
	{
		/*
		Checks the given HRESULT for success, and throws an exception of the specified
		type if the HR indicates failure.
		*/
		static void Test(
			HRESULT hr, 
			const TCHAR* fileName, 
			long line,
			const char* expression)
		{
			if(FAILED(hr))
			{
				T ex(DXGetErrorDescription9A(hr));
				ex.hr = hr;
				ex.hrString = DXGetErrorString9(hr);
				ex.hrDescription = DXGetErrorDescription9(hr);
				ex.fileName = fileName;
				ex.line = line;
				if(expression)
					ex.expression = expression;
				throw ex;
			}
		}
	};
}