#pragma once
/**
 *	@file
 *	@brief	テクスチャー関連
 */

#include "UnlimitedLib.h"

#define	TEXTURE_MAX	1024

// テクスチャーデータ
struct TextureData
{
	UINT						width;		// テクスチャーの幅
	UINT						height;		// テクスチャーの高さ
	UINT						srcWidth;	// 画像の幅
	UINT						srcHeight;	// 画像の高さ
	ID3D11Texture2D*			pTexture;	// 生成したテクスチャーのポインタ
	ID3D11ShaderResourceView*	pView;		// シェーダへのビュー
};

// テクスチャーの読み込み
bool LoadTexture(const char* pFileName, TextureData* pOut);
// テクスチャーの解放
void ReleaseTexture(TextureData* pTextureData);
// 使用中のテクスチャー数を取得
int GetTextureUseCount(void);

// DDS差し替えを行うか？
void EnableDDSReplace(bool enable);
