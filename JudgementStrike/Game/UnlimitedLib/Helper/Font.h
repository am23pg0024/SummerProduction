#pragma once
/**
 *	@file
 *	@brief	フォント関連
 */

#include "../UnlimitedLib.h"


#undef CreateFont

#define FONT_MAX	128		//!< フォント最大数


// フォントの生成
int CreateFont(const char* pFontPath);
// フォントの解放
void ReleaseFont(int index);

// フォントのスケール設定
void SetFontScale(int index, float scale);

// テキストのサイズ取得
void GetTextSize(int index, const char* pText, float* pWidth, float* pHeight);
// テキストのサイズ取得
void GetFormatTextSize(int index, float* pWidth, float* pHeight, const char* pFormat, ...);

// テキストの描画
void RenderText(int index, const char* pText, int x, int y, int color = 0xFFFFFFFF);
// テキストの描画
void RenderFormatText(int index, int x, int y, int color, const char* pFormat, ...);
