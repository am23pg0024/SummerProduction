#include "UnlimitedLib/UnlimitedLib.h"
#include "Game.h"
#if USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#endif

#if USE_IMGUI
// Unlimitedライブラリーデバッグ情報
extern Sprite gaSprites[SPRITE_MAX];
void DebugUsage()
{
	int sprite = 0;
	for(int i=0; i<SPRITE_MAX; i++){
		if(gaSprites[i].flag) sprite++;
	}

	int texture = GetTextureUseCount();
	int se = GetSEUseCount();

	ImGui::Begin("Unlimited");
	ImGui::Text("SE : %d / %d", se, SE_MAX);
	ImGui::Text("Sprite : %d / %d", sprite, SPRITE_MAX);
	ImGui::Text("Texture : %d / %d", texture, TEXTURE_MAX);
	ImGui::End();
}
#endif

// エントリーポイント
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	bool isFullscreen = false;
#ifndef _DEBUG
	isFullscreen = MessageBox(nullptr, "Unlimited", "フルスクリーンモードで起動しますか？", MB_YESNO) == IDYES;
#endif
#if USE_IMGUI
	ImGui::CreateContext();
#endif
	// UnlimitedLibの初期化
	if(!InitializeUnlimitedLib("Unlimited", SCREEN_WIDTH, SCREEN_HEIGHT, isFullscreen)){
		return true;
	}

	Initialize();

	while (!IsQuitMessage())
	{
		if (!UpdateWindowMessage())
		{
			UpdateInput();

			// 描画の開始
			BeginRenderScene();

			// 画面のクリア
			ClearDisp();


			SetDefaultRenderState();
#if USE_IMGUI
			// imgui update
			ImGuiIO &io = ImGui::GetIO();
			io.DeltaTime = 1.0f / 60.0f;
			io.DisplaySize.x = SCREEN_WIDTH;
			io.DisplaySize.y = SCREEN_HEIGHT;
			io.MouseDown[0] = GetKeyState(VK_LBUTTON) & 0x8000;
			io.MouseDown[1] = GetKeyState(VK_RBUTTON) & 0x8000;
			//	int mx, my;
		//	GetMousePosition(&mx, &my);
		//	io.MousePos = { (float)mx, (float)my };
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif

			GameMain();
#if USE_IMGUI
			DebugUsage();

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

			// 描画の終了
			EndRenderScene();
			// 画面の表示
			FlipDisp();
		}
	}
	Finalize();

	// UnlimitedLibの解放
	FinalizeUnlimitedLib();

	return 0;
}
