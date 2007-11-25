#pragma once

#include "GameTypes.h"
#include "NiNodes.h"

// 40?
class Tile
{
public:
	Tile();
	~Tile();

	virtual void			Destructor(void);
	virtual void			Unk_01(UInt32 unk0, const char * str, UInt32 unk2);
	virtual UInt32			Unk_02(void);
	virtual UInt32			Unk_03(void);
	virtual const char *	GetType(void);
	virtual UInt32			Unk_05(UInt32 unk0, UInt32 unk1, UInt32 unk2);
	virtual void			Unk_06(void);

	typedef NiTListBase <Tile *>	TileRefList;
	typedef NiTListBase <void *>	TileValueRefList;

//	void	** _ctor;	// 00
	UInt8	unk04;		// 04 - 0 = base tile
	UInt8	unk05;		// 05
	UInt8	unk06;		// 06
	UInt8	pad07;		// 07
	String	unk08;		// 08
	UInt32	unk10;		// 10
	TileRefList			refList;	// 14 - list of all the pointers to this tile?
	UInt32	unk24;		// 24
	void	* unk28;	// 28
	UInt32	unk2C;		// 2C
	TileValueRefList	unk30;	// 30 - const NiTPointerListBase<DFALL<Tile::Value *>,Tile::Value *>::`vftable'
};

// 4C
class TileImage : public Tile
{
public:
	TileImage();
	~TileImage();

	float	unk40;		// 40
	NiNode	* unk44;	// 44
	UInt8	unk48;		// 48
	UInt8	pad49[3];	// 49
};

// 54
class TileText : public Tile
{
public:
	TileText();
	~TileText();

	UInt32	pad40[4];	// 40
	UInt8	unk50;		// 50
	UInt8	pad51[3];	// 51
};

// 44
class TileRect : public Tile
{
public:
	TileRect();
	~TileRect();

	UInt32	unk44;		// 44
};
