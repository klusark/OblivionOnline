#pragma once

// 028
class Menu
{
public:
	Menu();
	~Menu();

	virtual void	Destructor(void);
	virtual void	SetField(UInt32 idx, UInt32 value);
	virtual void	Unk_02(UInt32 arg0, UInt32 arg1);	// show menu?
	virtual void	Unk_03(UInt32 arg0, UInt32 arg1);	// handle event?
	virtual void	Unk_04(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_05(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_06(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_07(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_08(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_09(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_0A(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_0B(void);
	virtual bool	Unk_0C(UInt32 arg0);
	virtual UInt32	GetID(void);
	virtual bool	Unk_0C(UInt32 arg0, UInt32 arg1);

//	void	** _vtbl;	// 00
	UInt32	unk04;		// 04
	UInt32	unk08;		// 08
	UInt32	unk0C;		// 0C
	UInt32	unk10;		// 10
	UInt32	unk14;		// 14
	UInt32	unk18;		// 18
	UInt32	unk1C;		// 1C - initialized to 1
	UInt32	unk20;		// 20 - uninitialized
	UInt32	unk24;		// 24 - initialized to 4
};

// 64
class MessageMenu : public Menu
{
public:
	MessageMenu();
	~MessageMenu();

	UInt32	fields[14];	// 28
	UInt8	unk60;		// 60
	UInt8	pad61[3];	// 61
};

// 58
class InventoryMenu : public Menu
{
public:
	InventoryMenu();
	~InventoryMenu();

	UInt32	fields[5];	// 28
	// 3C
	// ...
};

// ... lots more
