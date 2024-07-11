#pragma once
/**
 *	@file
 *	@brief	DirectDrawSurface関連
 */

#include "../UnlimitedLib.h"

//----------------------------------------------------------------------
//!	DDSフォーマット
//----------------------------------------------------------------------
enum DDSFMT
{
	DDSFMT_DXT1,
	DDSFMT_DXT3,
	DDSFMT_DXT5,
	DDSFMT_A8R8G8B8,
	DDSFMT_A1R5G5B5,
	DDSFMT_A4R4G4B4,
	DDSFMT_A8,
	DDSFMT_L8,

	DDSFMT_UNKNOWN	= -1,
};

//----------------------------------------------------------------------
//!	DDSピクセルフォーマット
//----------------------------------------------------------------------
struct DDS_PIXELFORMAT
{
	DWORD	dwSize;
	DWORD	dwFlags;
	DWORD	dwFourCC;
	DWORD	dwRGBBitCount;
	DWORD	dwRBitMask;
	DWORD	dwGBitMask;
	DWORD	dwBBitMask;
	DWORD	dwABitMask;
};

//----------------------------------------------------------------------
//!	DDSサーフェイス情報
//----------------------------------------------------------------------
struct DDSURFACEDESC2
{
	DWORD			dwSize;
	DWORD			dwHeaderFlags;
	DWORD			dwHeight;
	DWORD			dwWidth;
	DWORD			dwPitchOrLinearSize;
	DWORD			dwDepth;
	DWORD			dwMipMapCount;
	DWORD			dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD			dwSurfaceFlags;
	DWORD			dwCubemapFlags;
	DWORD			dwReserved2[3];
};

//----------------------------------------------------------------------
//!	DDSデータヘッダ
//----------------------------------------------------------------------
struct DDS_HEADER
{
	DWORD			dwMagic;
	DDSURFACEDESC2	ddsd;
};

//======================================================================
//!	DDS分析クラス
//======================================================================
class DDSAnalyzer
{
public:
	//!	コンストラクタ
	DDSAnalyzer(void);
	//!	DDS分析
	//!	@param pAddr [in] DDSデータ
	//!	@return 結果
	bool Analyze(const void* pAddr);

public:
	//!	ピクセルデータを取得
	//!	@return ピクセルデータ
	inline const void* PixelData(void) const { return m_pPixels; }
	//!	幅の取得
	//!	@return 幅
	inline UINT Width(void) const { return m_Width; }
	//!	高さの取得
	//!	@return 高さ
	inline UINT Height(void) const { return m_Height; }
	//!	フォーマットの取得
	//!	@return フォーマット
	inline DDSFMT Format(void) const { return m_Format; }
	//!	ミップマップ数の取得
	//!	@return ミップマップ数
	inline int MipLevels(void) const { return m_MipLevels; }
	//!	キューブマップフラグの取得
	//!	@return キューブマップフラグ
	inline UINT CubemapFlags(void) const { return m_CubemapFlags; }

private:
	const void*		m_pPixels;			//!< ピクセルデータ
	UINT			m_Width;			//!< 幅
	UINT			m_Height;			//!< 高さ
	DDSFMT			m_Format;			//!< フォーマット
	int				m_MipLevels;		//!< ミップマップ数
	UINT			m_CubemapFlags;		//!< キューブマップフラグ
};
