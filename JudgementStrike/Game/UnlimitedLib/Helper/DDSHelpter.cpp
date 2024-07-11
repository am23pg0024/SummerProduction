/**
 *	@file
 *	@brief	DirectDrawSurface関連
 */
#include "../UnlimitedLib.h"
#include "DDSHelpter.h"

/// FourCC作成マクロ
inline DWORD MakeFourcc(char a, char b, char c, char d) { return ((a) | (b << 8) | (c << 16) | (d << 24)); }
//	フォーマット判定用
const DWORD FOURCC_DXT1	= MakeFourcc('D','X','T','1');		//!< DXT1 シグネチャ
const DWORD FOURCC_DXT3	= MakeFourcc('D','X','T','3');		//!< DXT3 シグネチャ
const DWORD FOURCC_DXT5	= MakeFourcc('D','X','T','5');		//!< DXT5 シグネチャ

//----------------------------------------------------------------------
///	コンストラクタ
//----------------------------------------------------------------------
DDSAnalyzer::DDSAnalyzer(void)
	: m_pPixels(NULL), m_Width(0), m_Height(0)
	, m_MipLevels(0), m_CubemapFlags(0), m_Format(DDSFMT_UNKNOWN)
{}

//----------------------------------------------------------------------
///	DDS分析
//----------------------------------------------------------------------
bool DDSAnalyzer::Analyze(const void* pAddr)
{
	if( pAddr == NULL)	return false;

	const char* pData = reinterpret_cast<const char*>(pAddr);

	UINT ID = *reinterpret_cast<const UINT*>(pData);
	if( ID == MakeFourcc('D', 'D', 'S', ' ') )
	{
		const DDSURFACEDESC2* pDdsd;
		pDdsd = reinterpret_cast<const DDSURFACEDESC2*>(pData + 4);

		DDSURFACEDESC2 Ddsd = *pDdsd;

		m_Format = DDSFMT_UNKNOWN;
		if( Ddsd.ddspf.dwFourCC == FOURCC_DXT1 )
		{
			m_Format = DDSFMT_DXT1;
		}
		else if (Ddsd.ddspf.dwFourCC == FOURCC_DXT3)
		{
			m_Format = DDSFMT_DXT3;
		}
		else if (Ddsd.ddspf.dwFourCC == FOURCC_DXT5)
		{
			m_Format = DDSFMT_DXT5;
		}
		else
		{
			if( Ddsd.ddspf.dwRGBBitCount == 32 )
			{
				if( Ddsd.ddspf.dwRBitMask == 0x00ff0000 &&
					Ddsd.ddspf.dwGBitMask == 0x0000ff00 &&
					Ddsd.ddspf.dwBBitMask == 0x000000ff )
				{
					m_Format = DDSFMT_A8R8G8B8;
				}
			}
			else if( Ddsd.ddspf.dwRGBBitCount == 16 )
			{
				if( Ddsd.ddspf.dwRBitMask == 0x001f0000 &&
					Ddsd.ddspf.dwGBitMask == 0x00001f00 &&
					Ddsd.ddspf.dwBBitMask == 0x0000001f )
				{
					m_Format = DDSFMT_A1R5G5B5;
				}
				else if( Ddsd.ddspf.dwRBitMask == 0x000f0000 &&
							Ddsd.ddspf.dwGBitMask == 0x00000f00 &&
							Ddsd.ddspf.dwBBitMask == 0x0000000f &&
							Ddsd.ddspf.dwABitMask == 0x01000000 )
				{
					m_Format = DDSFMT_A4R4G4B4;
				}
			}
			else if( Ddsd.ddspf.dwRGBBitCount == 8 )
			{
				if( Ddsd.ddspf.dwABitMask == 0xff000000 )
				{
					m_Format = DDSFMT_A8;
				}
				if( Ddsd.ddspf.dwRBitMask == 0x00ff0000 )
				{
					m_Format = DDSFMT_L8;
				}
			}
			// 未対応のフォーマット
			if( m_Format == DDSFMT_UNKNOWN )
			{
				OutputDebugString("DDSファイルが未対応のフォーマットです(DDSAnalyzer::Analyze)");
				return false;
			}
		}

		m_pPixels		= pDdsd + 1;
		m_Width			= Ddsd.dwWidth;
		m_Height		= Ddsd.dwHeight;
		m_MipLevels		= max(Ddsd.dwMipMapCount, 1);
		m_CubemapFlags	= Ddsd.dwCubemapFlags;

		return true;
	}

	return false;
}

