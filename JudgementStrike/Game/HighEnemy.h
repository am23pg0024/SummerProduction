#pragma once

struct ENEMY;

void HighEnemy_Initialize();
void HighEnemy_Finalize();
void HighEnemy_Update();
void HighEnemy_Render();

void HighEnemyIdle(ENEMY* enemy);
void HighEnemyMove(ENEMY* enemy);
void HighEnemyAttack(ENEMY* enemy);
