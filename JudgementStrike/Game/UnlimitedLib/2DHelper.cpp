/**
 *	@file
 *	@brief	2Dヘルパー関連
 */
#include "UnlimitedLib.h"
#include <math.h>

Sprite gaSprites[SPRITE_MAX];


void CalcVertexPos(Simple2DVertex* pVertex)
{
	pVertex->x /= GetTargetWidth();
	pVertex->y /= GetTargetHeight();
	pVertex->x -= 0.5f;
	pVertex->x *= 2.0f;
	pVertex->y = 0.5f - pVertex->y;
	pVertex->y *= 2.0f;
}


int SearchFreeSprite(void)
{
	for(int i=0; i<SPRITE_MAX; i++){
		if(!gaSprites[i].flag){
			gaSprites[i].flag = true;
			return i;
		}
	}

	return -1;
}


// スプライトの解放
void ReleaseSprite(int index)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	ReleaseTexture(&pSprite->textureData);
	ZeroMemory(pSprite, sizeof(Sprite));
}

// スプライツの解放
void ReleaseSpriteMatrix(int *paIndex, int nTotal)
{
	if(paIndex == NULL) return;

	for(int i=0; i<nTotal; i++){
		ReleaseSprite(paIndex[i]);
	}
}

/**
 *	@brief	スプライトの生成
 *	@param	[in] pTexturePath	フルパスファイル名
 *	@return	スプライトインデックス
 *	@author	Kenji Nishida
 *	@date	2020/00/00 作成
 *	@date	2020/09/16 Doxygenコメント付け
 *	@note	スプライトバッファが足りなければ-1を返す
 */
int CreateSprite(const char *pTexturePath)
{
	int index = SearchFreeSprite();
	if(index == -1) return index;
	Sprite* pSprite = &gaSprites[index];

	float width  = 1.0f;
	float height = 1.0f;
	if(pTexturePath){
		if(LoadTexture(pTexturePath, &pSprite->textureData)){
			width = (float)pSprite->textureData.srcWidth;
			height = (float)pSprite->textureData.srcHeight;

			SetSpriteUV(index, 0, 0, (int)pSprite->textureData.srcWidth, (int)pSprite->textureData.srcHeight);
		}
	}
	SetSpriteSize(index, width, height);
	SetSpriteColor(index, 0xffffffff);
	SetSpriteOrigin(index, 0, 0);
	SetSpriteScale(index, 1, 1);

	return index;
}

// スプライツの生成
bool CreateSpriteMatrix(const char *pTexturePath, int spriteWidth, int spriteHeight, int nHorizon, int nVertical, int nTotal, int *paIndex)
{
	if(paIndex == NULL) return false;

	for(int i=0; i<nTotal; i++){
		paIndex[i] = SearchFreeSprite();
		if(paIndex[i] == -1) return false;
		Sprite *pSprite = &gaSprites[paIndex[i]];
		if(pTexturePath){
			if(LoadTexture(pTexturePath, &pSprite->textureData)){
				SetSpriteUV(paIndex[i], i % nHorizon * spriteWidth, i / nHorizon * spriteHeight, spriteWidth, spriteHeight);
			}
		}
		SetSpriteSize(paIndex[i], (float)spriteWidth, (float)spriteHeight);
		SetSpriteColor(paIndex[i], 0xffffffff);
		SetSpriteOrigin(paIndex[i], 0, 0);
		SetSpriteScale(paIndex[i], 1, 1);
	}

	return true;
}

// スプライトの座標設定
void SetSpritePos(int index, float x, float y)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	pSprite->x = x;
	pSprite->y = y;
}

// スプライトのサイズ設定
void SetSpriteSize(int index, float width, float height)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	pSprite->width = width;
	pSprite->height = height;
}

// スプライトの回転設定
void SetSpriteRotation(int index, float angle)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	pSprite->angle = angle;
}

// スプライトの拡縮設定
void SetSpriteScale(int index, float scax, float scay)
{
	if (index < 0 || index >= SPRITE_MAX) return;
	Sprite* pSprite = &gaSprites[index];
	if (!pSprite->flag) return;

	pSprite->scax = scax;
	pSprite->scay = scay;
}

// スプライトの原点設定
void SetSpriteOrigin(int index, float x, float y)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	pSprite->cx = x;
	pSprite->cy = y;
}

// スプライトのUV設定
void SetSpriteUV(int index, int u, int v, int width, int height)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	float imageW = (float)pSprite->textureData.width;
	float imageH = (float)pSprite->textureData.height;
	pSprite->u1	= (float)(u) / imageW;
	pSprite->v1	= (float)(v) / imageH;
	pSprite->u2	= (float)(u +  width) / imageW;
	pSprite->v2	= (float)(v + height) / imageH;
}

// スプライトの取得
const Sprite* GetSprite(int index)
{
	if (index < 0 || index >= SPRITE_MAX) return NULL;
	Sprite* pSprite = &gaSprites[index];
	if (!pSprite->flag) return NULL;

	return pSprite;
}

// スプライトのカラー設定
void SetSpriteColor(int index, D3DCOLOR color)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	pSprite->color	= color;
}

// スプライトに紐づいたテクスチャーの横幅
int GetSpriteTextureWidth(int index)
{
	if (index < 0 || index >= SPRITE_MAX) return 0;
	Sprite* pSprite = &gaSprites[index];
	if (!pSprite->flag) return 0;

	return pSprite->textureData.srcWidth;
}

// スプライトに紐づいたテクスチャーの縦幅
int GetSpriteTextureHeight(int index)
{
	if (index < 0 || index >= SPRITE_MAX) return 0;
	Sprite* pSprite = &gaSprites[index];
	if (!pSprite->flag) return 0;

	return pSprite->textureData.srcHeight;
}


// スプライト描画
void RenderSprite(int index)
{
	if(index < 0 || index >= SPRITE_MAX) return;
	Sprite *pSprite = &gaSprites[index];
	if(!pSprite->flag) return;

	ID3D11DeviceContext* pContext = GetDeviceContext();

	if( pSprite->textureData.pTexture )
	{
		pContext->PSSetShaderResources(0, 1, &pSprite->textureData.pView);
		SetShader(SHADER_TYPE_SIMPLE_2D_TEX);
	}
	else
	{
		ID3D11ShaderResourceView* pView = NULL;
		pContext->PSSetShaderResources(0, 1, &pView);
		SetShader(SHADER_TYPE_SIMPLE_2D);
	}
	SetInputLayout(VERTEX_FORMAT_SIMPLE_2D);

	Simple2DVertex vertices[4];
	vertices[0].x		= 0.0f;
	vertices[0].y		= 0.0f;
	vertices[0].u		= pSprite->u1;
	vertices[0].v		= pSprite->v1;

	vertices[1].x		= pSprite->width;
	vertices[1].y		= 0.0f;
	vertices[1].u		= pSprite->u2;
	vertices[1].v		= pSprite->v1;

	vertices[2].x		= 0.0f;
	vertices[2].y		= pSprite->height;
	vertices[2].u		= pSprite->u1;
	vertices[2].v		= pSprite->v2;

	vertices[3].x		= pSprite->width;
	vertices[3].y		= pSprite->height;
	vertices[3].u		= pSprite->u2;
	vertices[3].v		= pSprite->v2;

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = pSprite->color;
	vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = 0.0f;
	vertices[0].rhw = vertices[1].rhw = vertices[2].rhw = vertices[3].rhw = 1.0f;

	struct Vector2
	{
		float x, y;

		Vector2(float xx, float yy) : x(xx), y(yy) {}
	};
	for( int i=0; i < 4; i++ )
	{
		Vector2 pos(vertices[i].x - pSprite->cx, vertices[i].y - pSprite->cy);

		float fsin = sinf(pSprite->angle);
		float fcos = cosf(pSprite->angle);
		Vector2 tmp = Vector2(
			pos.x * fcos - pos.y * fsin,
			pos.x * fsin + pos.y * fcos
		);
		tmp.x *= pSprite->scax;
		tmp.y *= pSprite->scay;
		pos = tmp;

		vertices[i].x = pos.x + pSprite->x;
		vertices[i].y = pos.y + pSprite->y;

		CalcVertexPos(&vertices[i]);
	}
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DrawPrimitiveUP(vertices, sizeof(Simple2DVertex), ARRAYSIZE(vertices));
}

// スプライト位置指定描画
void RenderSpritePos(int index, float x, float y)
{
	SetSpriteOrigin(index, 0, 0);
	SetSpriteRotation(index, 0.0f);
	SetSpriteScale(index, 1.0f, 1.0f);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}
void RenderSpritePos(int index, int x, int y)
{
	RenderSpritePos(index, (float)x, (float)y);
}

// スプライト位置指定描画(左右反転)
void RenderSpritePosLR(int index, float x, float y)
{
	float width = 0.0f;
	const Sprite *p = GetSprite(index);
	if(p != nullptr){
		width = p->width;
	}
	SetSpriteOrigin(index, width, 0);
	SetSpriteRotation(index, 0.0f);
	SetSpriteScale(index, -1.0f, 1.0f);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}
void RenderSpritePosLR(int index, int x, int y)
{
	RenderSpritePosLR(index, (float)x, (float)y);
}

// スプライト位置指定描画 拡縮付き
void RenderSpriteSca(int index, float x, float y, float scale)
{
	SetSpriteOrigin(index, 0, 0);
	SetSpriteRotation(index, 0.0f);
	SetSpriteScale(index, scale, scale);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}
void RenderSpriteSca(int index, int x, int y, float scale)
{
	RenderSpriteSca(index, (float)x, (float)y, scale);
}

// スプライト位置指定描画 拡縮付き(左右反転)
void RenderSpriteScaLR(int index, float x, float y, float scale)
{
	float width = 0.0f;
	const Sprite* p = GetSprite(index);
	if (p != nullptr) {
		width = p->width;
	}
	SetSpriteOrigin(index, width, 0);
	SetSpriteRotation(index, 0.0f);
	SetSpriteScale(index, -scale, scale);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}
// スプライト位置指定描画 拡縮付き(左右反転)
void RenderSpriteScaLR(int index, int x, int y, float scale)
{
	RenderSpriteScaLR(index, (float)x, (float)y, scale);
}

// スプライト位置指定描画 回転拡縮付き
void RenderSpriteRot(int index, float ox, float oy, float x, float y, float angle, float sca)
{
	SetSpriteOrigin(index, ox, oy);
	SetSpriteRotation(index, angle);
	SetSpriteScale(index, sca, sca);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}

// スプライト位置指定描画 回転拡縮付き
void RenderSpriteRot(int index, int ox, int oy, int x, int y, float angle, float sca)
{
	RenderSpriteRot(index, (float)ox, (float)oy, (float)x, (float)y, angle, sca);
}

// 色付きポリゴン描画
void RenderPoly(float x, float y, float width, float height, D3DCOLOR color)
{
	ID3D11DeviceContext* pContext = GetDeviceContext();
	ID3D11ShaderResourceView* pView = NULL;
	pContext->PSSetShaderResources(0, 1, &pView);

	SetShader(SHADER_TYPE_SIMPLE_2D);
	SetInputLayout(VERTEX_FORMAT_SIMPLE_2D);

	Simple2DVertex vertices[4];
	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;

	vertices[1].x = width;
	vertices[1].y = 0.0f;

	vertices[2].x = 0.0f;
	vertices[2].y = height;

	vertices[3].x = width;
	vertices[3].y = height;

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;
	vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = 0.0f;
	vertices[0].rhw = vertices[1].rhw = vertices[2].rhw = vertices[3].rhw = 1.0f;

	for (int i = 0; i < 4; i++)
	{
		vertices[i].x += x;
		vertices[i].y += y;

		CalcVertexPos(&vertices[i]);
	}

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DrawPrimitiveUP(vertices, sizeof(Simple2DVertex), ARRAYSIZE(vertices));
}
void RenderPoly(int x, int y, int width, int height, D3DCOLOR color)
{
	RenderPoly((float)x, (float)y, (float)width, (float)height, color);
}

/*!
 *	\brief	点描画
 *	\param	[in] x	x座標
 *	\param	[in] y	y座標
 *	\author	Kenji Nishida
 *	\date	2019/12/04
 */
void RenderPixel(float x, float y, D3DCOLOR color)
{
	ID3D11DeviceContext* pContext = GetDeviceContext();
	ID3D11ShaderResourceView* pView = NULL;
	pContext->PSSetShaderResources(0, 1, &pView);

	SetShader(SHADER_TYPE_SIMPLE_2D);
	SetInputLayout(VERTEX_FORMAT_SIMPLE_2D);

	Simple2DVertex vertex;
	vertex.x = x;
	vertex.y = y;
	vertex.z = 0.0f;
	vertex.color = color;
	vertex.rhw = 1.0f;

	CalcVertexPos(&vertex);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	DrawPrimitiveUP(&vertex, sizeof(Simple2DVertex), 1);
}
void RenderPixel(int x, int y, D3DCOLOR color)
{
	RenderPixel((float)x, (float)y, color);
}



extern VertexShader* g_pVertexShader;
extern PixelShader* g_pPixelShader;

// カスタムシェーダでの描画開始
void BeginShader(PixelShader* pPixelShader, VertexShader* pVertexShader)
{
	g_pVertexShader = pVertexShader;
	g_pPixelShader = pPixelShader;
}

// カスタムシェーダでの描画終了
void EndShader()
{
	g_pVertexShader = NULL;
	g_pPixelShader = NULL;
}