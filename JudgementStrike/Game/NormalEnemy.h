#pragma once

struct ENEMY;

void NormalEnemy_Initialize();
void NormalEnemy_Finalize();
void NormalEnemy_Update();
void NormalEnemy_Render();

void NormalEnemyIdle(ENEMY* enemy);
void NormalEnemyMove(ENEMY* enemy);
void NormalEnemyAttack(ENEMY* enemy);
