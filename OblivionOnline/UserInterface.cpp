#include "UserInterface.h"
#include <cstdio>
#include "main.h"
UserInterface::UserInterface(void)
{
// 0 everything
	bVisible = false;
	Height = 200;
	Width = 400;
	OverlayMgr = 0;
	BmpFileHandle = fopen("/OblivionOnline/GUI/OOChat.bmp","r");
	if(!BmpFileHandle)
		Console_Print("Could not open file /gui/OOchat.bmp");
	LoadBMP();
	LoadFont();
}

UserInterface::~UserInterface(void)
{
	if(BmpFileHandle)
		fclose(BmpFileHandle);
	if(FileData)
	{
		free(FileData);
		FileData = NULL;
		Header = NULL;
		BmpData = NULL;
	}
	if(FontFileData)
	{
		free(FontFileData);
		FontFileData = NULL;
		BmpFont =  NULL;
	}
}

// Call this when a message is incoming 
bool UserInterface::RegisterChatMessage(std::string Message, std::string Sender)
{
	return false;
}

// Sets if the chat is visible 
bool UserInterface::SetDisplayState(bool Visible)
{
	return false;
}

// Sets if the User Interface should be displayed. Note that this DOES release all resources etc. 
bool UserInterface::SetGlobalState(bool Visible)
{
	if(Visible)
	{
		if(!OverlayMgr)
		{
			OverlayMgr = new Overlay::OverlayManager();
			if(OverlayMgr->SupportAvailable())
			{
				OverlayMgr->Reset(Width,Height); // Make that customisable. I use the size of the Default skin.
			}
		}
	}
	else
	{
		if(OverlayMgr)
		{
			delete OverlayMgr;
			OverlayMgr = NULL;
		}
		//Deinitialise
	}
	return false;
}
void UserInterface::RenderingCallback(IDirectDrawSurface7* overlayBackSurface,
								  Overlay::PixelFormat format,
								  void* userData)
{
	
	
}
bool UserInterface::LoadBMP()
{
	if(BmpFileHandle)
	{
		char  fileheader[14]; // we do not write that header into the data storage
		for (int i = 0;i < 14; i++)
		{
			fileheader[i] = fgetc(BmpFileHandle);
		}
		if((WORD)fileheader[0] == 19778) // It is a bitmap
		{
			Filesize = ((DWORD)fileheader[2]);
			FileData = (BYTE *)malloc(Filesize);// we keep 14 Bytes as extra because of the file header. These are not used, but nevertheless useful because sometimes it does not work
			if(FileData)
			{
				for (int i = 0;((i < Filesize)&&(!feof(BmpFileHandle))); i++) 
				{
					FileData[i] = fgetc(BmpFileHandle);
				}
				Header = (BITMAPINFOHEADER *)FileData;
				Width = Header->biWidth;
				Height = Header->biHeight;
				if(Header->biBitCount != 24)
				{
					Console_Print("Chat Skin is not a 24 bit bitmap");
					fclose(BmpFileHandle);
					return false;
				}
				if(Header->biCompression != 0)
				{
					Console_Print("Chat Skin is not uncompressed");
					fclose(BmpFileHandle);
					return false;
				}
				BmpData = (BYTE *)(FileData +((DWORD)fileheader[10]-14)); // We use the offset - 14 bytes because we did NOT copy the file header
				fclose(BmpFileHandle);
			}
		}

	}
}
bool UserInterface::LoadFont()
{
	BITMAPINFOHEADER *Header;
	FILE *font = fopen("/OblivionOnline/GUI/OOfont.bmp","r");
	if(!font)
	{
		Console_Print("Couldn't open font  /OblivionOnline/GUI/OOfont.bmp");
		return false;
	}
	char  fileheader[14]; // we do not write that header into the data storage
	DWORD Filesize;
	for (int i = 0;i < 14; i++)
	{
		fileheader[i] = fgetc(font);
	}
	if((WORD)fileheader[0] == 19778) // It is a bitmap
	{
		Filesize = ((DWORD)fileheader[2]);		
		FontFileData = (BYTE *)malloc(Filesize);// we keep 14 Bytes as extra because of the file header. These are not used, but nevertheless useful because sometimes it does not work
		if(FileData)
		{
			for (int i = 0;((i < Filesize)&&(!feof(font))); i++) 
			{
				fileheader[i] = fgetc(font);
			}
			Header = (BITMAPINFOHEADER *)FileData;
			if(Header->biWidth != 256)
			{
				Console_Print("Chat Font is not 256 pixels wide");
				fclose(font);
				return false;
			}
			if(Header->biHeight != 256)
			{
				Console_Print("Chat Font is not 256 pixels high");
				fclose(font);
				return false;
			}
			if(Header->biBitCount != 24)
			{
				Console_Print("Chat Font is not a 24 bit bitmap");
				fclose(font);
				return false;
			}
			if(Header->biCompression != 0)
			{
				Console_Print("Chat Font is not uncompressed");
				fclose(font);
				return false;
			}
			BmpFont = (BYTE *)(FileData + ((DWORD)fileheader[10]-14)); // We use the offset - 14 bytes because we did NOT copy the file header
			}
		}

}