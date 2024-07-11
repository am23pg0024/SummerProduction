#pragma once

#define TILE_NUM 64
#define TILE_SIZE 32
#define STAGE_SIZE_X 80
#define STAGE_SIZE_Y 45

void Stage_Initialize();
void Stage_Finalize();
void Stage_Update();
void Stage_Render();

void InitStage();

const int* GetTileMap();
const int* GetStage();
const int* GetHitTable();

int LoadStage(const char* filename, int* stage);
int SaveStage(const char* filename, int* stage);
