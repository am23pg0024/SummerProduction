#pragma once
/**
 *	@file
 *	@brief	2Dヘルパー関連
 */

#include "UnlimitedLib.h"
#include "Shader.h"

#define SPRITE_MAX			1024


// シンプルなテクスチャ―付き頂点情報
struct Simple2DVertex
{
	float x, y, z;
	float rhw;
	DWORD color;
	float u, v;
};


/// モデル情報
struct Sprite
{
	
	bool		flag;			//!< 使用フラグ
	float		x;				//!< x座標
	float		y;				//!< y座標
	float		width;			//!< 幅
	float		height;			//!< 高さ
	float		u1;				//!< 左テクスチャー座標
	float		v1;				//!< 上テクスチャー座標
	float		u2;				//!< 右テクスチャー座標
	float		v2;				//!< 下テクスチャー座標
	float		angle;			//!< 角度
	float		scax;			//!< x方向拡大率
	float		scay;			//!< y方向拡大率
	float		cx;				//!< 中心x座標
	float		cy;				//!< 中心y座標
	D3DCOLOR	color;			//!< 色
	TextureData	textureData;	//!< テクスチャー情報
};


// スプライトの解放
void ReleaseSprite(int index);
// スプライツの解放
void ReleaseSpriteMatrix(int *paIndex, int nTotal);

// スプライトの生成
int CreateSprite(const char *pTexturePath = NULL);
// スプライツの生成
bool CreateSpriteMatrix(const char *pTexturePath, int spriteWidth, int spriteHeight, int nHorizon, int nVertical, int nTotal, int *paIndex);

// スプライトの座標設定
void SetSpritePos(int index, float x, float y);
// スプライトのサイズ設定
void SetSpriteSize(int index, float width, float height);
// スプライトの回転設定
void SetSpriteRotation(int index, float angle);
// スプライトの拡縮設定
void SetSpriteScale(int index, float scax, float scay);
// スプライトの原点設定
void SetSpriteOrigin(int index, float x, float y);
// スプライトのUV設定
void SetSpriteUV(int index, int u, int v, int width, int height);
// スプライトのカラー設定
void SetSpriteColor(int index, D3DCOLOR color);

// スプライトの取得
const Sprite* GetSprite(int index);
// スプライトに紐づいたテクスチャーの横幅
int GetSpriteTextureWidth(int index);
// スプライトに紐づいたテクスチャーの縦幅
int GetSpriteTextureHeight(int index);


// スプライト描画
void RenderSprite(int index);
// スプライト描画 位置指定
void RenderSpritePos(int index, float x, float y);
void RenderSpritePos(int index, int x, int y);
// スプライト描画 位置指定(左右反転)
void RenderSpritePosLR(int index, float x, float y);
void RenderSpritePosLR(int index, int x, int y);
// スプライト位置指定描画 拡縮付き
void RenderSpriteSca(int index, float x, float y, float scale);
void RenderSpriteSca(int index, int x, int y, float scale);
// スプライト位置指定描画 拡縮付き(左右反転)
void RenderSpriteScaLR(int index, float x, float y, float scale);
void RenderSpriteScaLR(int index, int x, int y, float scale);
// スプライト位置指定描画 回転拡縮付き
void RenderSpriteRot(int index, float ox, float oy, float x, float y, float angle, float sca);
void RenderSpriteRot(int index, int ox, int oy, int x, int y, float angle, float sca);
// 色付きポリゴン描画
void RenderPoly(float x, float y, float width, float height, D3DCOLOR color);
void RenderPoly(int x, int y, int width, int height, D3DCOLOR color);
// 点描画
void RenderPixel(int x, int y, D3DCOLOR color);
void RenderPixel(float x, float y, D3DCOLOR color);


// カスタムシェーダでの描画開始
void BeginShader(PixelShader* pPixelShader, VertexShader* pVertexShader = nullptr);
// カスタムシェーダでの描画終了
void EndShader();