/**
 *	@file
 *	@brief	フォント関連
 */
#include "../UnlimitedLib.h"
#include "Font.h"
#include "FontData.h"
#include <math.h>


struct FontData
{
	FontHeader		header;			//!< ヘッダー
	FontCharacter*	pCharacters;	//!< 文字情報
	FontConvert*	pConvData;		//!< コンバート情報
	FontTexture*	pTexturesInfo;	//!< テクスチャ―情報
	int*			paSpriteIndex;	//!< フォントスプライト
	float			scale;			//!< スケール
};

FontData gaFonts[FONT_MAX]{};

/// フォントの空きを検索
int SearchFreeFont(void)
{
	for (int i = 0; i < FONT_MAX; i++)
	{
		if (gaFonts[i].pCharacters == NULL)
		{
			return i;
		}
	}
	return -1;
}


/// 次の文字コードを取得
const char* GetNextCode(const char* pText, int16_t* pCode)
{
	int16_t code = (short)pText[0] & 0xFF;
	if (code == '\0')
	{
		return NULL;
	}
	else if (IsDBCSLeadByte(pText[0]))
	{
		code = ((pText[1] << 8) & 0xFF00) | code;
		++pText;
	}
	++pText;
	*pCode = code;
	return pText;
}

/// 文字情報の取得
FontCharacter* FindCharacter(int index, int16_t code)
{
	FontData* pFont = &gaFonts[index];
	FontHeader* pHeader = &pFont->header;
	// 見つからない場合は末尾のエラー文字を表示
	int no = 0;
	// コンバートテーブル内から文字情報へのインデックスを取得する
	for (int i = 0; i < pHeader->charCount; i++)
	{
		if (pFont->pConvData[i].code == code)
		{
			no = (int)pFont->pConvData[i].index;
		}
	}
	return &pFont->pCharacters[no];
}


/// フォントの生成
int CreateFont(const char* pFontPath)
{
	int index = SearchFreeFont();
	if (index == -1)return index;

	FILE* pFile;
	//	ファイルを開く
	int ret = fopen_s(&pFile, pFontPath, "rb");
	if (ret != 0)
	{
		TRACE("%sの読み込みにしっぽいしました(FontSystem::Initialize)\n", pFontPath);
		return -1;
	}

	FontData* pFont = &gaFonts[index];
	FontHeader* pHeader = &pFont->header;

	//	ヘッダーの読み込み
	fread(pHeader, sizeof(*pHeader),1, pFile);

	// バッファの確保
	pFont->pCharacters = (FontCharacter*)malloc(sizeof(FontCharacter) * pHeader->charCount);
	pFont->pConvData = (FontConvert*)malloc(sizeof(FontConvert) * pHeader->charCount);
	pFont->pTexturesInfo = (FontTexture*)malloc(sizeof(FontTexture) * pHeader->textureCount);
	pFont->paSpriteIndex = (int*)malloc(sizeof(int) * pHeader->textureCount);
	pFont->scale = 1.0f;

	ZeroMemory(pFont->paSpriteIndex, sizeof(int) * pHeader->textureCount);

	// 文字データの読み込み
	fseek(pFile, pHeader->charOffset, SEEK_SET);
	fread(pFont->pCharacters, sizeof(FontCharacter), pHeader->charCount, pFile);

	// 文字ShiftJISからUTF16への変換データの読み込み
	fseek(pFile, pHeader->convertOffset, SEEK_SET);
	fread(pFont->pConvData, sizeof(FontConvert), pHeader->charCount, pFile);

	// 文字ShiftJISからUTF16への変換データの読み込み
	fseek(pFile, pHeader->textureOffset, SEEK_SET);
	fread(pFont->pTexturesInfo, sizeof(FontTexture), pHeader->textureCount, pFile);

	fclose(pFile);

	// ファイルのディレクトリを取得
	char dir[MAX_PATH], path[MAX_PATH];
	strcpy_s(dir, pFontPath);
	for (int i = strlen(dir); 0 < i; i--)
	{
		if (dir[i] == '/' || dir[i] == '\\')
		{
			dir[i] = '\0';
			break;
		}
	}
	// テクスチャ―の生成
	for (int i = 0; i < pHeader->textureCount; i++)
	{
		sprintf_s(path, "%s/%d.bmp", dir, i);
		pFont->paSpriteIndex[i] = CreateSprite(path);
		if (pFont->paSpriteIndex[i] == -1)
		{
			ReleaseFont(index);
			return -1;
		}
	}

	return index;
}

/// フォントの解放
void ReleaseFont(int index)
{
	FontData* pFont = &gaFonts[index];
	if (pFont->pCharacters == NULL)
	{
		// 解放済み
		return;
	}

	for (int i = 0; i < pFont->header.textureCount; i++)
	{
		ReleaseSprite(pFont->paSpriteIndex[i]);
	}
	// バッファの確保
	free(pFont->pCharacters);
	free(pFont->pConvData);
	free(pFont->pTexturesInfo);
	free(pFont->paSpriteIndex);

	ZeroMemory(pFont, sizeof(*pFont));
}

/// フォントのスケール設定
void SetFontScale(int index, float scale)
{
	FontData* pFont = &gaFonts[index];
	pFont->scale = scale;
}

/// テキストのサイズ取得
void GetTextSize(int index, const char* pText, float* pWidth, float* pHeight)
{
	FontData* pFont = &gaFonts[index];

	float w, h;
	w = h = 0;
	int16_t code;
	while (pText = GetNextCode(pText, &code))
	{
		FontCharacter* pCharacter = FindCharacter(index, code);
		w += (float)pCharacter->sizeU * pFont->scale;
		h = max(h, (float)pCharacter->sizeV * pFont->scale);
	}
	*pWidth = w;
	*pHeight = h;
}

/// テキストのサイズ取得
void GetFormatTextSize(int index, float* pWidth, float* pHeight, const char* pFormat, ...)
{
	char temp[1024];
	va_list list;
	va_start(list, pFormat);
	vsprintf_s(temp, pFormat, list);
	va_end(list);

	GetTextSize(index, temp, pWidth, pHeight);
}

/// テキストの描画
void RenderTextCode(int index, int* pX, int* pY, int color, int16_t code)
{
	FontData* pFont = &gaFonts[index];

	FontCharacter* pCharacter = FindCharacter(index, code);
	FontTexture* pTexture = &pFont->pTexturesInfo[pCharacter->textureIndex];
	int spriteIndex = pFont->paSpriteIndex[pCharacter->textureIndex];

	float x = (float)*pX;
	float y = (float)*pY;
	float w = (float)pCharacter->sizeU * pFont->scale;
	float h = (float)pCharacter->sizeV * pFont->scale;

	SetSpriteSize(spriteIndex, w, h);
	SetSpriteUV(spriteIndex, pCharacter->u, pCharacter->v, pCharacter->sizeU, pCharacter->sizeV);

	SetRenderMode(ERenderMode::Subtract, true);
	SetSpriteColor(spriteIndex, 0xffffffff);
	RenderSpritePos(spriteIndex, x, y);
	SetRenderMode(ERenderMode::Add, true);
	SetSpriteColor(spriteIndex, color);
	RenderSpritePos(spriteIndex, x, y);

	*pX += (int)w;
}

/// テキストの描画
void RenderText(int index, const char* pText, int x, int y, int color)
{
	int16_t code;
	while (pText = GetNextCode(pText, &code))
	{
		RenderTextCode(index, &x, &y, color, code);
	}
	SetRenderMode(ERenderMode::Normal, true);
}

/// テキストの描画
void RenderFormatText(int index, int x, int y, int color, const char* pFormat, ...)
{
	char temp[1024];
	va_list list;
	va_start(list, pFormat);
	vsprintf_s(temp, pFormat, list);
	va_end(list);

	RenderText(index, temp, x, y, color);
}
