/**
 *	@file
 *	@brief	テクスチャー関連
 */
#include "UnlimitedLib.h"
#include "Helper/DDSHelpter.h"
#include <vector>
#include <filesystem>

#define	ALIGN(n, align)		(((n) + (align) - 1) & ~((align) - 1))

struct LoadedTextureInfo
{
	char		filename[MAX_PATH];
	int			count;
	TextureData	texData;
};
static LoadedTextureInfo g_loadedTextures[TEXTURE_MAX] = {};
static bool g_DDSReplace = false;


UINT GetFormatBlockSize(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_UNORM:
		return 8;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC3_UNORM:
		return 16;
	}
	return 0;
}

static const TextureData* LoadDDSTexture(const char* pFileName, int index)
{
	logPrintf("LoadDDSTexture \"%s\"", pFileName);
	LoadedTextureInfo* pInfo = &g_loadedTextures[index];

	FILE* hFile;
	fopen_s(&hFile, pFileName, "rb");
	if(hFile == nullptr){
		logPrintf(" File not found.\n");
		return nullptr;
	}
	fseek(hFile, 0, SEEK_END);
	int fileSize = ftell(hFile);
	fseek(hFile, 0, SEEK_SET);

	void* pBuffer = malloc(fileSize);
	if(pBuffer == nullptr) MessageBox(nullptr, "LoadDDSTexture\nメモリーが足りません。", "Unlimited", MB_OK);
	fread_s(pBuffer, fileSize, fileSize, 1, hFile);
	fclose(hFile);

	DDSAnalyzer ddsAnalyzer;
	if (!ddsAnalyzer.Analyze(pBuffer))
	{
		logPrintf(" enough memory error.\n");
		free(pBuffer);
		return NULL;
	}

	DXGI_FORMAT fmt;
	switch (ddsAnalyzer.Format())
	{
	case DDSFMT_DXT1:		fmt = DXGI_FORMAT_BC1_UNORM;		break;
	case DDSFMT_DXT3:		fmt = DXGI_FORMAT_BC2_UNORM;		break;
	case DDSFMT_DXT5:		fmt = DXGI_FORMAT_BC3_UNORM;		break;
	case DDSFMT_A8R8G8B8:	fmt = DXGI_FORMAT_R8G8B8A8_UNORM;	break;
	default:
		logPrintf(" unknown format error.\n");
		free(pBuffer);
		return NULL;
	}

	ID3D11Texture2D* pTexture;

	int width = ddsAnalyzer.Width();
	int height = ddsAnalyzer.Height();
	int mipLevels = ddsAnalyzer.MipLevels();

	std::vector<D3D11_SUBRESOURCE_DATA> InitResource;
	//	初期化データの設定
	const char* pOffset = (const char*)ddsAnalyzer.PixelData();
	for (int i = 0; i < mipLevels; i++)
	{
		UINT size = 0, pitch = 0;
		switch (fmt)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		{
			UINT w = max(width >> i, 1U);
			UINT h = max(height >> i, 1U);
			pitch = w * 4;
			size = h * w * 4;
		} break;

		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
		{
			UINT uiBlockSize = GetFormatBlockSize(fmt);
			UINT w = max(width >> i, 1U);
			UINT h = max(height >> i, 1U);
			pitch = ALIGN(w, 4U) / 4 * uiBlockSize;
			size = ALIGN(h, 4U) / 4 * pitch;
		} break;
		}
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = pOffset;
		data.SysMemPitch = pitch;
		data.SysMemSlicePitch = size;

		InitResource.push_back(data);

		pOffset += size;
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipLevels;
	desc.ArraySize = 1;
	desc.Format = fmt;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	if (FAILED(GetDevice()->CreateTexture2D(&desc, InitResource.data(), &pTexture))) {
		logPrintf(" CreateTexture failed.\n");
		return nullptr;
	}
	ID3D11ShaderResourceView* pView;
	if (FAILED(GetDevice()->CreateShaderResourceView(pTexture, NULL, &pView))) {
		SAFE_RELEASE(pTexture);
		logPrintf(" CreateTexture failed.\n");
		return nullptr;
	}
	// テクスチャー情報の設定
	pInfo->texData.pTexture = pTexture;
	pInfo->texData.pView = pView;
	pInfo->texData.srcWidth = width;
	pInfo->texData.srcHeight = height;
	pInfo->texData.width = desc.Width;
	pInfo->texData.height = desc.Height;
	strcpy_s(pInfo->filename, pFileName);
	pInfo->count++;

	free(pBuffer);
	logPrintf(" Ok.\n");
	return &pInfo->texData;
}

static const TextureData* LoadFullColorTexture(const char* pFileName, int index)
{
	logPrintf("LoadFullColorTexture \"%s\"", pFileName);
	LoadedTextureInfo* pInfo = &g_loadedTextures[index];

	ID3D11Texture2D* pTexture;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(pFileName);
	logPrintf(" format:%d", fif);
	FIBITMAP* dib = FreeImage_Load(fif, pFileName);
	if(dib == nullptr){
		logPrintf(" File not found.\n");
		return nullptr;
	}
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);
	int width2 = 1;
	int height2 = 1;
	while (width2 < width) width2 *= 2;
	while (height2 < height) height2 *= 2;


	struct RGBA
	{
		BYTE r, g, b, a;
	};
	std::vector<RGBA> bits(width2 * height2);
	RGBQUAD rgbquad;
	RGBQUAD* paPalette;
	BYTE value;
	unsigned char orAlpha = 0xff;
	if (FreeImage_IsTransparent(dib)) orAlpha = 0x00;
	unsigned bpp = FreeImage_GetBPP(dib);
	if (bpp < 16) {
		paPalette = FreeImage_GetPalette(dib);
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (bpp >= 16) {
				FreeImage_GetPixelColor(dib, x, y, &rgbquad);
				rgbquad.rgbReserved |= orAlpha;
			}
			else {
				FreeImage_GetPixelIndex(dib, x, y, &value);
				rgbquad = paPalette[value];
				if (rgbquad.rgbBlue || rgbquad.rgbGreen || rgbquad.rgbRed) rgbquad.rgbReserved = 0xff;
			}
			RGBA rgba{ rgbquad.rgbRed, rgbquad.rgbGreen, rgbquad.rgbBlue, rgbquad.rgbReserved };
			memcpy(&bits[(width2 * (height - 1 - y) + x)], &rgba, sizeof(rgba));
		}
	}
	FreeImage_Unload(dib);

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = bits.data();
	initData.SysMemPitch = width2 * sizeof(bits[0]);
	initData.SysMemSlicePitch = bits.size() * sizeof(bits[0]);

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width2;
	desc.Height = height2;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	HRESULT hr = GetDevice()->CreateTexture2D(&desc, &initData, &pTexture);
	if (FAILED(hr)) {
		logPrintf(" CreateTexture failed.\n");
		return nullptr;
	}
	ID3D11ShaderResourceView* pView;
	if (FAILED(GetDevice()->CreateShaderResourceView(pTexture, NULL, &pView))) {
		SAFE_RELEASE(pTexture);
		logPrintf(" CreateTexture failed.\n");
		return nullptr;
	}

	// テクスチャー情報の設定
	pInfo->texData.pTexture = pTexture;
	pInfo->texData.pView = pView;
	pInfo->texData.srcWidth = width;
	pInfo->texData.srcHeight = height;
	pInfo->texData.width = desc.Width;
	pInfo->texData.height = desc.Height;
	strcpy_s(pInfo->filename, pFileName);
	pInfo->count++;

	logPrintf(" Ok.\n");
	return &pInfo->texData;
}

// テクスチャーの読み込み
static const TextureData* LoadTexture(const char* pFileName)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		LoadedTextureInfo* pInfo = &g_loadedTextures[i];
		if (pInfo->count == 0) continue;

		if (strcmp(pInfo->filename, pFileName) == 0)
		{
			pInfo->count++;
			return &pInfo->texData;
		}
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		LoadedTextureInfo* pInfo = &g_loadedTextures[i];
		if (pInfo->count != 0) continue;

		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(pFileName);
		switch (fif)
		{
		case FIF_DDS:
			return LoadDDSTexture(pFileName, i);
		default:
			return LoadFullColorTexture(pFileName, i);
		}
	}
	return NULL;
}


// テクスチャーの読み込み
bool LoadTexture(const char* pFileName, TextureData* pOut)
{
	if (pOut == NULL) return false;

	if (g_DDSReplace)
	{
		char temp[256];
		strcpy(temp, pFileName);
		int i;
		for (i = strlen(temp); i > 0; i--)
		{
			if(temp[i] == '.') break;
		}
		strcpy(&temp[i], ".dds");

		if (std::filesystem::exists(temp))
		{
			const TextureData* pData = LoadTexture(temp);
			if (pData)
			{
				*pOut = *pData;
				return true;
			}
		}
	}

	const TextureData * pData = LoadTexture(pFileName);
	if (pData == NULL) return false;

	*pOut = *pData;
	return true;
}

// テクスチャーの解放
void ReleaseTexture(TextureData * pTextureData)
{
	if (pTextureData == NULL) return;

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		LoadedTextureInfo* pInfo = &g_loadedTextures[i];
		if (pInfo->count == 0) continue;

		if (pInfo->texData.pTexture == pTextureData->pTexture)
		{
			pInfo->count--;
			if (pInfo->count == 0)
			{
				ZeroMemory(pInfo, sizeof(*pInfo));

				SAFE_RELEASE(pTextureData->pTexture);
				SAFE_RELEASE(pTextureData->pView);
			}
		}
	}
}

int GetTextureUseCount(void)
{
	int count = 0;

	for(int i=0; i<TEXTURE_MAX; i++){
		if(g_loadedTextures[i].count > 0) count++;
	}

	return count;
}

void EnableDDSReplace(bool enable)
{
	g_DDSReplace = enable;
}