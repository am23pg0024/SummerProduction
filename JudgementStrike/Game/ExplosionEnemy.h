#pragma once

struct ENEMY;

void ExplosionEnemy_Initialize();
void ExplosionEnemy_Finalize();
void ExplosionEnemy_Update();
void ExplosionEnemy_Render();

void ExplosionEnemyIdle(ENEMY* enemy);
void ExplosionEnemyMove(ENEMY* enemy);
void ExplosionEnemyAttack(ENEMY* enemy);
void ExplosionEnemyDead(ENEMY* enemy);
