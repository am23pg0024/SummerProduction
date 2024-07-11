#pragma once

struct FRECT;
struct CIRCLE;
struct FAN;
struct VECTOR;
struct OBJECT;

// 矩形同士の判定
bool IsHit(FRECT* a, FRECT* b);
bool IsHitLeft(FRECT* self, FRECT* target);
bool IsHitRight(FRECT* self, FRECT* target);
bool IsHitTop(FRECT* self, FRECT* target);
bool IsHitBottom(FRECT* self, FRECT* target);

// 円同士の判定
bool IsHitCircle(CIRCLE& a, CIRCLE& b);

// 矩形と円の判定
bool IsHitRectWithCircle(FRECT& rect, CIRCLE& cir);
// 線分の交差判定
bool IsCrossLine(VECTOR& vec1, VECTOR& vec2, VECTOR& vec3, VECTOR& vec4);
// 矩形と扇形の当たり判定
bool IsHitRectWithFan(FRECT& rect, FAN& fan, float angle);
// オブジェクト間に通行不能なタイルがあるかを判定
bool CheckBetween(OBJECT& self, OBJECT& target);
