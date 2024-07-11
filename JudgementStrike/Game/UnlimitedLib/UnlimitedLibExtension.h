#include "UnlimitedLib.h"

// シザリングの開始
void BeginScissor(int x, int y, int width, int height);
// シザリングの終了
void EndScissor();



// シェーダの読み込み
int LoadCustomShader(const char* pFilename, const char* pEntryPoint);
// シェーダの解放
void ReleaseCustomShader(int index);

// カスタムシェーダの開始
void BeginCustomShader(int index);
// カスタムシェーダの終了
void EndCustomShader();


// シェーダパラメータの設定
void SetCustomShaderParam(void* pParam, int size);