#include "GameMenus.h"

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00AFC094;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00B3B33C;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00B3B33C;

#else

#error unsupported version of oblivion

#endif
