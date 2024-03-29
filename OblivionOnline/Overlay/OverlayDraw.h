/* These Files has been edited by masterfreek64. They have been changed to remove the dependency on the BOOST library. */
/*
Copyright (c) 2007 Muhammad Haggag

This software is provided 'as-is', without any express or implied warranty. In no event 
will the author(s) be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including 
commercial applications, and to alter it and redistribute it freely, subject to 
the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you 
wrote the original software. If you use this software in a product, an acknowledgment in 
the product documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

3. This notice may not be removed or altered from any source distribution.
*/
#pragma once

#include <ddraw.h>

namespace Overlay
{
	struct ImageRgb24
	{
		BYTE* data;
		int width, height;
	};

	void Blit(
		IDirectDrawSurface7* destinationSurface,
		int xDest, int yDest,
		const ImageRgb24& image,
		int xSrc, int ySrc);
}