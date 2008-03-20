/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64

This file is part of OblivionOnline.

    OblivionOnline is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    OblivionOnline is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Linking OblivionOnline statically or dynamically with other modules is making a combined work based
	on OblivionOnline. Thus, the terms and conditions of the GNU General Public License cover 
	the whole combination.

    In addition, as a special exception, the copyright holders of  OblivionOnline give you permission to 
	combine OblivionOnline program with free software programs or libraries that are released under
	the GNU LGPL and with code included in the standard release of Oblivion Script Extender by Ian Patterson (OBSE)
	under the OBSE license (or modified versions of such code, with unchanged license). You may copy and distribute such a system 
	following the terms of the GNU GPL for  OblivionOnline and the licenses of the other code concerned,
	provided that you include the source code of that other code when and as the GNU GPL
	requires distribution of source code.

	Note that people who make modified versions of  OblivionOnline are not obligated to grant this special exception
	for their modified versions; it is their choice whether to do so. 
	The GNU General Public License gives permission to release a modified version without this
	exception; this exception also makes it possible to release a modified version which carries 
	forward this exception.
*/
#include "main.h"
#include "D3DHook.h"
#include "UserInterface.h"
#include "cegui/CEGUIDefaultResourceProvider.h"
CEGUI::DirectX9Renderer * myRenderer;
bool bUIInitialized = false;
DWORD WINAPI InitialiseUI()
{
	if(!bUIInitialized)
	{
		char _ApplicationPath [MAX_PATH];
		GetModuleFileName(0,_ApplicationPath,MAX_PATH);
		strrchr(_ApplicationPath,'\\')[0] = '\0';
		//we could in theory free ApplicationPath
		std::string ApplicationPath(_ApplicationPath);
		//Sleep(20000);// 20 seconds for Oblivion to load
		_MESSAGE("Loading CEGUI");
		if(!OblivionDirect3D9Device)
		{
			_ERROR("Device not found - aborting GUI");
			return false;
		}
		myRenderer = new CEGUI::DirectX9Renderer(OblivionDirect3D9Device,100);
		_MESSAGE("Created renderer");
		CEGUI::Window* myRoot;
		_MESSAGE("Creating CEGUI::System - view CEGUI.log for further information");
		CEGUI::System *system = new CEGUI::System(myRenderer);
		CEGUI::DefaultResourceProvider *rp = ((CEGUI::DefaultResourceProvider *)system->getResourceProvider());
		rp->setResourceGroupDirectory("default",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setResourceGroupDirectory("schemes",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setResourceGroupDirectory("imagesets",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setResourceGroupDirectory("fonts",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setResourceGroupDirectory("looknfeel",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setResourceGroupDirectory("layouts",ApplicationPath + "\\OblivionOnline\\GUI\\");
		rp->setDefaultResourceGroup("default");
		CEGUI::ImagesetManager::getSingleton().createImageset("TaharezLook.imageset");
		// load in the scheme file, which auto-loads the TaharezLook imageset
		CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
		// load in a font.  The first font loaded automatically becomes the default font.
		CEGUI::FontManager::getSingleton().createFont("fkp-16.font");
		CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
		myRoot = CEGUI::WindowManager::getSingleton().loadWindowLayout("OOChat.layout");
		CEGUI::System::getSingleton().setGUISheet(myRoot);
		
		bUIInitialized = true;
		_MESSAGE("Successfully loaded GUI");
	}
	return 1;
}
void DeinitialiseUI()
{
	bUIInitialized = false;
}
void RegisterChatMessage(char *message)
{
	/*
	CEGUI::Window *chatwin = CEGUI::WindowManager::getSingleton().getWindow("ChatMessage");
	if(chatwin)
		chatwin->setText(chatwin->getText() +"\n"+message);
	else
	{
		Console_Print("Could not find CEGUI Window ChatMessage");
		_ERROR("Could not find ChatMessage");
	}
	*/
}
void SetConnectionMessage(char *message)
{
	CEGUI::Window *chatwin = CEGUI::WindowManager::getSingleton().getWindow("StatusLabel");
	chatwin->setText(message);
}
