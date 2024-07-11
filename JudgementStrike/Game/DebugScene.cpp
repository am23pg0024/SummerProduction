#include "UnlimitedLib/UnlimitedLib.h"
#include "2DHelperEx.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Game.h"
#include "Stage.h"
#include "Button.h"
#include "Camera.h"
#include <math.h>
#if USE_IMGUI
#include "imgui/imgui.h"
#endif

int debugSprite;
int debugBG;
int saveSprite, loadSprite;
BUTTON saveButton, loadButton;

int stage_tmp[STAGE_SIZE_Y * STAGE_SIZE_X];
char stage_filename[] = "Data/stage1.dat";

int curTileID = 0;
int mx, my;	// マウスポジション


void SelectTile(int& curTileID);

static bool SceneInit()
{
	// スプライト読み込み＆初期化
	debugSprite = CreateSprite("Assets/Sprites/Temp/debug.png");
	debugBG = CreateSprite("Assets/Sprites/Temp/white.png");
	saveSprite = CreateSprite("Assets/Sprites/Temp/debug_save.png");
	loadSprite = CreateSprite("Assets/Sprites/Temp/debug_load.png");

	const Sprite* sp = GetSprite(debugBG);
	SetSpriteScale(debugBG, 178 / sp->width, SCREEN_HEIGHT / sp->height);

	sp = GetSprite(saveSprite);
	saveButton = CreateButton(saveSprite, 10, SCREEN_HEIGHT - 51, sp->width, sp->height, true);
	sp = GetSprite(loadSprite);
	loadButton = CreateButton(loadSprite, 20 + sp->width, SCREEN_HEIGHT - 51, sp->width, sp->height, true);

	// ステージ初期化
	const int* tiles = GetTileMap();
	for (int i = 0; i < STAGE_SIZE_Y; i++) {
		for (int j = 0; j < STAGE_SIZE_X; j++) {
			stage_tmp[j + i * STAGE_SIZE_X] = tiles[53];
		}
	}

	LoadStage(stage_filename, stage_tmp);

	InitCamera();

	return true;
}

static void SceneFinal()
{
	ReleaseSprite(debugSprite);
	ReleaseSprite(debugBG);
	ReleaseSprite(saveSprite);
	ReleaseSprite(loadSprite);
}

static void SceneUpdate()
{
	// Escキーでタイトルへ
	if (IsKeyPush(VK_ESCAPE)) {
		JumpScene(SetupTitleScene);
		return;
	}

	const Sprite* bg = GetSprite(debugBG);
	float bg_width = bg->width * bg->scax;

	// カメラ
	const CAMERA* cam = GetCamera();

	// マウス位置取得
	GetMousePosition(&mx, &my);

#if USE_IMGUI
	//ImGui::Begin("DebugScene");
	//ImGui::Text("(mx, my):(%d, %d)", mx, my);
	//ImGui::Text("(camX, camY):(%f, %f)", cam->x, cam->y);
	//ImGui::Text("(mx + camX - bg_width, my + camY):(%f, %f)", mx+cam->x - bg_width, my+cam->y);
	//ImGui::End();
#endif

	// ステージエディット
	if (mx >= bg_width) {

		// 左クリックで設置
		if (IsKeyPress(VK_LBUTTON)) {
			float zoomed_tile_size = TILE_SIZE * cam->zoom;

			if (0 <= mx + cam->x - bg_width && mx + cam->x - bg_width < STAGE_SIZE_X * zoomed_tile_size &&
				0 <= my + cam->y && my + cam->y < STAGE_SIZE_Y * zoomed_tile_size)
			{
				// カメラ座標を考慮したマウスのステージ座標を計算
				int	stage_mx = (mx + cam->x - bg_width) / zoomed_tile_size;
				int	stage_my = (my + cam->y) / zoomed_tile_size;

				stage_tmp[stage_mx + stage_my * STAGE_SIZE_X] = curTileID;
			}
		}
		// 右クリックしながらドラッグでカメラ移動
		else if (IsKeyPress(VK_RBUTTON)) {
			// カメラズーム倍率適用！！！
			static int prev_mx = 0;
			static int prev_my = 0;
			int move_x = 0;
			int move_y = 0;

			// クリック時のマウス位置保持
			if (IsKeyPush(VK_RBUTTON)) {
				prev_mx = mx;
				prev_my = my;
			}

			if (mx != prev_mx || my != prev_my) {
				move_x = mx - prev_mx;
				move_y = my - prev_my;
			}

			// カメラ座標再設定
			SetCameraXY(cam->x - move_x, cam->y - move_y);
			
			prev_mx = mx;
			prev_my = my;
		}

		// ホイールでカメラズーム
		int wheelNotch = GetMouseWheelNotch();
		float delta_zoom = 0;
		if (wheelNotch < 0) {
			delta_zoom = -0.05f;
			if (cam->zoom + delta_zoom <= 0.25f)
				delta_zoom = 0.f;
		}
		else if (wheelNotch > 0) {
			delta_zoom = 0.05f;
			if (cam->zoom + delta_zoom >= 1.5f)
				delta_zoom = 0.f;
		}
		SetCameraZoom(cam->zoom + delta_zoom);
	}
	// タイル選択・セーブ＆ロード
	else {
		if (IsKeyPress(VK_LBUTTON)) {
			SelectTile(curTileID);

			if (IsMouseOnButton(saveButton)) {
				SaveStage(stage_filename, stage_tmp);
			}
			if (IsMouseOnButton(loadButton)) {
				LoadStage(stage_filename, stage_tmp);
				SetCameraXY(0, 0);
				SetCameraZoom(1.f);
			}
		}
	}
}

static void SceneRender()
{
	// カメラ
	const CAMERA* cam = GetCamera();
	float zoomed_tile_size = TILE_SIZE * cam->zoom;

	const Sprite* bg = GetSprite(debugBG);
	float bg_width = bg->width * bg->scax;

	// ステージ描画
	for (int i = 0; i < STAGE_SIZE_Y; i++) {
		for (int j = 0; j < STAGE_SIZE_X; j++) {
			RenderSpriteSca(
				stage_tmp[j + i * STAGE_SIZE_X] + GetTileMap()[0],
				j * zoomed_tile_size - cam->x + bg_width, i * zoomed_tile_size - cam->y,
				cam->zoom
			);
		}
	}

	/*===== 選択中のタイルをマウスの位置に描画 =====*/
	// カメラ座標の原点とのズレを計算
	float offset_x = (fmodf(cam->x, zoomed_tile_size));
	float offset_y = (fmodf(cam->y, zoomed_tile_size));
	int stage_mx = (mx + offset_x - bg_width) / zoomed_tile_size;
	int stage_my = (my + offset_y) / zoomed_tile_size;
	RenderSpriteSca(
		GetTileMap()[curTileID],
		stage_mx * zoomed_tile_size - offset_x + bg_width,
		stage_my * zoomed_tile_size - offset_y,
		cam->zoom
	);

	/*===== タイル選択エリア描画 =====*/
	RenderSprite(debugBG);
	//RenderSpriteEx(
	//	debugBG,
	//	32, 32,
	//	89, SCREEN_HEIGHT / 2,
	//	0.f,
	//	178 / bg->width, SCREEN_HEIGHT / bg->height
	//);
	// タイル一覧
	for (int i = -8; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			RenderSpritePos(
				GetTileMap()[j + (i + 8) * 4],
				10 + (TILE_SIZE + 10) * j,
				SCREEN_HEIGHT / 2 + i * (TILE_SIZE + 6)
			);
		}
	}
	//for (int i = 0; i < 2; i++) {
	//	for (int j = -16; j < 16; j++) {
	//		RenderSpritePos(GetTileMap()[j + 16 + i * 32], SCREEN_WIDTH / 2 + j * (TILE_SIZE + 6), SCREEN_HEIGHT - (2 - i) * (TILE_SIZE + 10));
	//	}
	//}

	// セーブ・ロードボタン
	RenderButton(saveButton);
	RenderButton(loadButton);

#if USE_IMGUI
//	ImGui::Begin("DebugScene");
//	ImGui::Text("(mx, my):(%d, %d)", mx, my);
//	ImGui::End();
#endif
}

// ゲームシーンのセットアップ
SceneProc SetupDebugScene()
{
	SceneProc proc{
		"Debug",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}

void SelectTile(int& curTileID)
{
	for (int i = -8; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			float x = 10 + (TILE_SIZE + 10) * j;
			float y = SCREEN_HEIGHT / 2 + i * (TILE_SIZE + 6);
			BUTTON tile = { -1, x, y, TILE_SIZE, TILE_SIZE, true };

			if (IsMouseOnButton(tile)) {
				curTileID = j + (i + 8) * 4;
				return;
			}
		}
	}
	//for (int i = 0; i < 2; i++) {
	//	for (int j = -16; j < 16; j++) {
	//		float x = SCREEN_WIDTH / 2 + j * (TILE_SIZE + 6);
	//		float y = SCREEN_HEIGHT - (2 - i) * (TILE_SIZE + 10);
	//		BUTTON tile = { -1, x, y, TILE_SIZE, TILE_SIZE, true };

	//		if (IsMouseOnButton(&tile)) {
	//			curTileID = GetTileMap()[j + 16 + i * 32];
	//			return;
	//		}
	//	}
	//}
}
