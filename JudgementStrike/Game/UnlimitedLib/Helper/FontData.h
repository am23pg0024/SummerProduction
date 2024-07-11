﻿#pragma once
//----------------------------------------------------------------------
//!	@file	FontData.h
//!	@brief	テクスチャーフォント関連
//!
//!	@author S.Kawamoto
//----------------------------------------------------------------------
#include <stdint.h>


//!	文字コード
enum ECharSet
{
	Unkown		= -1,	//!< Unknown
	ShiftJIS	= 0,	//!< Shift-JIS
	UTF16,				//!< UTF-16
};

//======================================================================
//!	ヘッダー
//======================================================================
struct FontHeader
{
	int32_t	id;				//!< FOURCC('k', 't', 'f', ' ')
	int32_t version;		//!< バージョン
	int32_t	charSet;		//!< 文字コードの種類
	int32_t	charCount;		//!< フォント数
	int32_t	charOffset;		//!< 文字情報へのオフセット
	int32_t	convertOffset;	//!< コンバート情報へのオフセット(0の場合は無し)
	int32_t charPaddingX;	//!< X軸の余白
	int32_t charPaddingY;	//!< Y軸の余白
	int32_t	textureCount;	//!< テクスチャー数
	int32_t	textureOffset;	//!< テクスチャー情報へのオフセット
};
//======================================================================
//!	文字情報
//======================================================================
struct FontCharacter
{
	int16_t	code;			//!< 文字コード
	int16_t	textureIndex;	//!< テクスチャーのインデックス
	int16_t	u;				//!< U値
	int16_t	v;				//!< V値
	int16_t	sizeU;			//!< 横幅
	int16_t	sizeV;			//!< 縦幅
};
//======================================================================
//!	文字のコンバート情報(Shift-JISからUTF-16へ)
//======================================================================
struct FontConvert
{
	int16_t	code;	//!< 文字コード(Shift-JIS)
	int16_t	index;	//!< 文字情報のインデックス
};

//======================================================================
//!	テクスチャー情報
//======================================================================
struct FontTexture
{
	int32_t	width;	//!< 横幅
	int32_t	height;	//!< 縦幅
	int32_t	size;	//!< データサイズ
};
