#include "UnlimitedLib/UnlimitedLib.h"
#include "Game.h"
#include "Stage.h"
#include "Camera.h"
#if USE_IMGUI
#include "imgui/imgui.h"
#endif

int tileMap[TILE_NUM];
int stage[STAGE_SIZE_Y * STAGE_SIZE_X];

int hitTable[TILE_NUM] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 2,
	2, 2, 2, 2, 0, 0, 0, 0,
};

const int* GetTileMap() { return tileMap; }
const int* GetStage() { return stage; }
const int* GetHitTable() { return hitTable; }

void InitStage()
{
	LoadStage("Data/stage1.dat", stage);
}

void Stage_Initialize()
{
	for (int i = 0; i < TILE_NUM; i++) {
		char filename[256];
		sprintf(filename, "Assets/Stage/tiles/TileSep-%d.png", i + 1);
		tileMap[i] = CreateSprite(filename);
		SetSpriteSize(tileMap[i], 32, 32);
	}

	for (int i = 0; i < STAGE_SIZE_Y; i++) {
		for (int j = 0; j < STAGE_SIZE_X; j++) {
			stage[j + i * STAGE_SIZE_X] = tileMap[53];
		}
	}
}

void Stage_Finalize()
{
	ReleaseSpriteMatrix(tileMap, TILE_NUM);
}

void Stage_Update()
{

}

void Stage_Render()
{
	const CAMERA* cam = GetCamera();
	int cam_start_x = cam->x / TILE_SIZE - 1;
	int cam_start_y = cam->y / TILE_SIZE - 1;
	int cam_end_x = cam_start_x + SCREEN_WIDTH / TILE_SIZE + 2;
	int cam_end_y = cam_start_y + SCREEN_HEIGHT / TILE_SIZE + 3;

	if (cam_start_x < 0) cam_start_x = 0;
	if (cam_start_y < 0) cam_start_y = 0;
	if (cam_end_x > STAGE_SIZE_X) cam_end_x = STAGE_SIZE_X;
	if (cam_end_y > STAGE_SIZE_Y) cam_end_y = STAGE_SIZE_Y;

	for (int i = cam_start_y; i < cam_end_y; i++) {
		for (int j = cam_start_x; j < cam_end_x; j++) {
			RenderSpritePos(stage[j + i * STAGE_SIZE_X] + tileMap[0], j * TILE_SIZE - cam->x, i * TILE_SIZE - cam->y);
		}
	}
}

/**
 * マップデータロード
 *
 * @param stage: ステージデータ
 * @return 0: 成功, 0以外: 失敗
 */
int LoadStage(const char* filename, int* stage)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == nullptr) return 1;

	fread(stage, sizeof(int), STAGE_SIZE_X * STAGE_SIZE_Y, fp);

	fclose(fp);
	return 0;
}

/**
 * マップデータセーブ
 *
 * @param stage: ステージデータ
 * @return 0: 成功, 0以外: 失敗
 */
int SaveStage(const char* filename, int* stage)
{
	FILE* fp = fopen(filename, "wb");
	if (fp == nullptr) return 1;

	fwrite(stage, sizeof(int), STAGE_SIZE_X * STAGE_SIZE_Y, fp);

	fclose(fp);
	return 0;
}
