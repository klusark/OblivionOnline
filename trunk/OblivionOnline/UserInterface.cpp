#include "UserInterface.h"
#include <cstdio>
#include "main.h"
extern "C" HINSTANCE hDll;
extern bool NetChat(char *Message);
//extern UserInterface usrInterface; // Pointer to the sole instance
LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardHookProc(int nCode,WPARAM wParam, 
                            LPARAM lParam)
{       
	if(HC_ACTION == nCode) // alternative codee
   // if (!((DWORD)lParam & 2147483648) &&(HC_ACTION==nCode)) //byte 31 is set ....
    {        
		//usrInterface.RegisterKeystroke(wParam);
    }

    LRESULT RetVal = CallNextHookEx( 0, nCode, wParam, lParam );
    return  1;
	
}
UserInterface::UserInterface(void)
{
// 0 everything
	_MESSAGE("User Interface Loading ...");
	bVisible = false;
	Height = 200;
	Width = 400;
	OverlayMgr = 0;
	BmpFileHandle = fopen("/OblivionOnline/GUI/OOChat.bmp","r");
	if(!BmpFileHandle)
		Console_Print("Could not open file /gui/OOchat.bmp");
	LoadBMP();
	LoadFont();
	//Hook the Keyboard
	SetWindowsHookEx(WH_KEYBOARD,KeyboardHookProc,hDll,NULL);
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
	int NumLines = (Message.length() +Sender.length() + 1)/33; // Number of Lines
	Message = Sender + ":" + Message; // we create the real message
	//

	if(NumLines == 0) // means we got 1 line ... incomplete
	{
		// Easiest way for now....
		strcpy(ChatLines[0],ChatLines[1]);
		strcpy(ChatLines[1],ChatLines[2]);
		strcpy(ChatLines[2],ChatLines[3]);
		strcpy(ChatLines[3],ChatLines[4]);
		strcpy(ChatLines[4],ChatLines[5]);
		strcpy(ChatLines[5],ChatLines[6]);
		strcpy(ChatLines[6],ChatLines[7]);
		strncpy(ChatLines[7],Message.c_str(),33);
		return true;
	}
	if(NumLines <= 7)
	{
		int cx = 7;
		do  // first we move the lines up , just like we need them
		{
			strcpy(ChatLines[cx-NumLines],ChatLines[cx]);
			cx--;
		}
		while(cx - NumLines >= 0);
		for(int cx = 0;cx <= NumLines;cx++)
		{
			// now we set our text
			strncpy(ChatLines[(7-NumLines)],(Message.c_str()+cx*33),33); // We draw 33 Characters per line. Not bery elegant , but why care...
		}
		return true;		
	}
	else
	{
		//special code for people like Stalker. I know it is hard, but 6*33 = 198 characters is ok. If that is not enough write multiple messages or view the console
		Message = "Spam blocked.See Console.Sender:";
		Message += Sender;
		// Easiest way for now....
		strcpy(ChatLines[0],ChatLines[1]);
		strcpy(ChatLines[1],ChatLines[2]);
		strcpy(ChatLines[2],ChatLines[3]);
		strcpy(ChatLines[3],ChatLines[4]);
		strcpy(ChatLines[4],ChatLines[5]);
		strcpy(ChatLines[5],ChatLines[6]);
		strcpy(ChatLines[6],ChatLines[7]);
		strncpy(ChatLines[7],Message.c_str(),33);
		return true;
	}
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
	UserInterface *thisObj = (UserInterface *)userData;
	Overlay::ImageRgb24 image =
	{
		thisObj->RenderBuffer, // data
		thisObj->Width,
		thisObj->Height
	};
	Overlay::Blit(overlayBackSurface,0,0,image,0,0);
	
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
	return false;
}
bool UserInterface::RegisterKeystroke(WPARAM Key)
{
	if(IsTyping)
	{
		if((DWORD)Key != VK_RETURN)
		{
			char buf = MapVirtualKey(Key,0); // We make a char
			strncat(ChatLines[8],&buf,1);
		}
		else
		{
			if(IsTyping)
			{
				IsTyping = false; // we have to send a chat message now
				NetChat(ChatLines[8]);
				strcpy(ChatLines[8],"\0");
			}
		}
	}
	else
	{
		if((DWORD)Key == VK_RETURN)
		{
			IsTyping = true;
		}
	}
	return true;
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
	return false;
}
bool UserInterface::FillRenderBuffer()
{
	if(!RenderBuffer)
		Console_Print("No render buffer present, creating it");
	RenderBuffer = (BYTE *)malloc(3*Width*Height); // 3 bytes per pixel , Width * Height Pixels;
	if(!RenderBuffer)
	{
		Console_Print("Could not create render buffer. UserInterface::FillRenderBuffer() fails");
		return false;
	}
	//memcpy(RenderBuffer,BmpData,(3*Width*Height));// We fill it with the background first
	//Bitmap contains BGR, we need RGB... so we have got to cdopy it ourselves
	int maxval = (3*Width*Height);
	for(int i = 0 ;i < maxval;i += 3)
	{
		RenderBuffer[i] = BmpData[i+2]; // We copy the red value
		RenderBuffer[i+1] = BmpData[i+1]; // We copy the green value
		RenderBuffer[i+2] = BmpData [i]; // we copy the 
	}
	// now we do the text
	//these are variables in  the for loop , but we define them outside of the loop to save a bit of CPU time with cleanup
	/*USHORT y = 9;// 25 is our standard offset, but we add 16 before rendering ...
	USHORT x = 15;// 25 is our standard offset, but we add 10 before rendering ...*/
	//CurrentPixel = (BYTE *)(RenderBuffer + ((y+row)*3*Width+((x+col)*3))); 
	
	BYTE *CurrentBasePixel = (BYTE *)(RenderBuffer + 25*3*Width + 25*3 ); //This defines the pointer to the upper left of the current character
	BYTE *CurrentPixel; // The pixel the loop is working on
	size_t length;  

	BYTE *SourcePixel;
	// Memory - Time tradeoff
	// We now create all strings that are present 
	for(int i = 0; i < 9; i++) // Row 8 is for input but still rendered
	{
		length = strlen(ChatLines[i]); // We use it multiple times, and strlen is slow .
		if(length) // This string exists , we can render it
		{			
			// then column
			for(int cx = 0; cx < length; cx++)
			{
				// here we render a single character				
				CurrentPixel = CurrentBasePixel;
				// We render the actual character
				// first  row
				//1. We get the upper left corner of our character
				// NOTE: c does INTEGER DIVISON here -> so we cannot remove that
				// explanation: We divide the ASCII value by 16 , thus getting our row. Then , because each row is 16 pixels high we advacne 16 lines
				// then we take the modulo and advance to the upper left pixel
				SourcePixel = BmpFont  + 3*256*( ChatLines[i][cx]/16)*16 + 3*(ChatLines[i][cx]%16)*16; 
				for(int row = 0; row < 16;row++)
				{
					// then col
					for (int col = 0; col < 10 ; col++)
					{
						// for font transparency we check if all values are not null
						if(*(CurrentPixel+2) && *(CurrentPixel+1) && *CurrentPixel)
						{
							// We render the pixel we are working on
							// again we shift from BGR to RGB....
							//original calculation on the pixel source pointer
							//*(BmpFont + (3*((ChatLines[i][cx] / 16)*15+row)*Width)+(3*((ChatLines[i][cx] % 16)*15+col)));
							CurrentPixel[2] = SourcePixel[0];
							CurrentPixel[1] = SourcePixel[1];
							CurrentPixel[0] = SourcePixel[2];
							CurrentPixel += 3; 
						}
						SourcePixel += 3; // next byte
					}
					//we advanced 30 bytes already , now advance the rest so we get a new line
					CurrentPixel += (3 * Width - 30);
					SourcePixel += 678; //  We advance to the next line -> 3*(256 -30)= 678
				}
				CurrentBasePixel += 3 * 10;  // We go to the next character
			}
			// we advace the rest of the line ...
			CurrentBasePixel += 3*(Width-10*length);
		}		
	}
	return false;
}
bool UserInterface::Update() // This can indeed waste CPU time ... I will have to think of a way to place EVERYTHING in a special thread.
{
	FillRenderBuffer(); // We do that here....
	while(!OverlayMgr->Update(&UserInterface::RenderingCallback,this))
		Sleep(50); // we wait a bit until we can get the surfaces
	return true;
}
// our Hook Procedure
