#pragma once

void Effect_Initialize();
void Effect_Finalize();
void Effect_Update();
void Effect_Render();
void Effect_Restart();

void CreateSwordEffect(float x, float y, float angle);
bool IsSwordEffectActive();