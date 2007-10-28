#pragma once
#include "Overlay/OverlayManager.h"
#include "Overlay/OverlayDraw.h"
class UserInterface
{
public:
	UserInterface(void);
	~UserInterface(void);
	//  Call this when a message is incoming 
	bool RegisterChatMessage(std::string Message, std::string Sender);
	// Sets if the chat is visible 
	bool SetDisplayState(bool Visible);
	// Sets if the User Interface should be displayed. Note that this DOES release all resources etc. 
	bool SetGlobalState(bool Visible);
	//Renders the Frame
	void RenderingCallback(IDirectDrawSurface7* overlayBackSurface,
								  Overlay::PixelFormat format,
								  void* userData);
	
private:
	bool LoadFont(); // this creates our font
	bool LoadBMP();
	bool bVisible;
	bool FillRenderBuffer(); // A function that renders the current text
	FILE *BmpFileHandle;
	BYTE *FileData; // Raw Data of the File
	BYTE *RenderBuffer; // This contains the raw BMP data to be rendered. Like that we only have to redo the complex font calculations when a new message is incoming
	/* 
	We start rendering at 25 , 25 . The Last Line starts at 155. This means we can have 8 lines of 16 pt(spacing included) font - which we really need , until we get a sharper one :D 
	The Last x character is 355, so we could  have 20 characters per line if we used all 16 Pixels. 
	However We only use 10 pixels so we can have 33 characters. This cuts of some special characters. 
	Later I should fix these, but later we should also have a better font.
	*/
	char ChatLines[8][33]; // We do NOT have a "scroll-up" ourselves, but we also print into the console ....
	DWORD Filesize;
	BITMAPINFOHEADER *Header;
	BYTE *BmpData;
	unsigned short Width,Height; 
	Overlay::OverlayManager *OverlayMgr;
	BYTE *BmpFont;
	BYTE *FontFileData;

};

