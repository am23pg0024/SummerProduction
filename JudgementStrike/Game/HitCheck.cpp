#include "Game.h"
#include "HitCheck.h"
#include "Stage.h"
#include "EnemyManager.h"
#include <math.h>
#include "Stage.h"

// 矩形同士の判定
bool IsHit(FRECT* a, FRECT* b)
{
	if (a->right > b->left &&
		a->left < b->right &&
		a->bottom > b->top &&
		a->top < b->bottom)
		return true;

	return false;
}

bool IsHitLeft(FRECT* self, FRECT* target)
{
	if (self->right - 5 >= target->left &&
		self->left  - 1 <= target->right &&
		self->bottom - 5 >= target->top &&
		self->top + 5 <= target->bottom)
		return true;

	return false;
}

bool IsHitRight(FRECT* self, FRECT* target)
{
	if (self->right  + 1 >= target->left &&
		self->left + 5 <= target->right &&
		self->bottom - 5 >= target->top &&
		self->top + 5 <= target->bottom)
		return true;

	return false;
}

bool IsHitTop(FRECT* self, FRECT* target)
{
	if (self->right - 5 >= target->left &&
		self->left + 5 <= target->right &&
		self->bottom  - 5 >= target->top &&
		self->top - 1 <= target->bottom)
		return true;

	return false;
}

bool IsHitBottom(FRECT* self, FRECT* target)
{
	if (self->right - 5 >= target->left &&
		self->left + 5 <= target->right &&
		self->bottom + 1 >= target->top &&
		self->top  + 5 <= target->bottom)
		return true;

	return false;
}

// 円同士の判定
bool IsHitCircle(CIRCLE& a, CIRCLE& b)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	float r = a.radius + b.radius;

	return x * x + y * y < r * r;
}

// 矩形と円の判定
bool IsHitRectWithCircle(FRECT& rect, CIRCLE& cir)
{
	/* ブロードフェーズ */
	FRECT rect2 = { cir.x - cir.radius, cir.y - cir.radius, cir.x + cir.radius, cir.y + cir.radius };
	if (!IsHit(&rect, &rect2))
		return false;

	/* ナローフェーズ */
	// 辺の判定
	if (rect.left <= cir.x && cir.x <= rect.right
		&& rect.top - cir.radius <= cir.y && cir.y <= rect.bottom + cir.radius)
		return true;
	if (rect.left - cir.radius <= cir.x && cir.x <= rect.right + cir.radius
		&& rect.top <= cir.y && cir.y <= rect.bottom)
		return true;

	// 角の判定
	// 左上
	float x = rect.left - cir.x;
	float y = rect.top - cir.y;
	float length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	// 左下
	x = rect.left - cir.x;
	y = rect.bottom - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	// 右上
	x = rect.right - cir.x;
	y = rect.top - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;
	
	// 右下
	x = rect.right - cir.x;
	y = rect.bottom - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	return false;
}

// 線分の交差判定
bool IsCrossLine(VECTOR& vec1, VECTOR& vec2, VECTOR& vec3, VECTOR& vec4)
{
	VECTOR a = { vec2.x - vec1.x, vec2.y - vec1.y };
	VECTOR b = { vec3.x - vec1.x, vec3.y - vec1.y };
	VECTOR c = { vec4.x - vec1.x, vec4.y - vec1.y };

	float cross_ab = a.x * b.y - a.y * b.x;
	float cross_ac = a.x * c.y - a.y * c.x;

	if (cross_ab * cross_ac > 0) return false;

	a = { vec4.x - vec3.x, vec4.y - vec3.y };
	b = { vec1.x - vec3.x, vec1.y - vec3.y };
	c = { vec2.x - vec3.x, vec2.y - vec3.y };

	cross_ab = a.x * b.y - a.y * b.x;
	cross_ac = a.x * c.y - a.y * c.x;

	if (cross_ab * cross_ac > 0) return false;

	return true;
}

//bool IsHitSquareWithLine(FRECT& rect, VECTOR& vec1, VECTOR& vec2)
//{
//	// 線分と矩形の辺が重なっている
//
//	// 重なってなければ線分の端点のうちどちらかが矩形の内部
//	return false;
//}


//bool IsCrossLineWithCircle(FAN& circle, FRECT& rect, float angle, float* crossX1, float* crossY1, float* crossX2, float* crossY2)
//{
//    // 左辺
//    float dist = rect.left - circle.x;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // 交点が1つ
//        *crossX1 = rect.left;
//        *crossY1 = circle.y;
//        *crossX2 = rect.left;
//        *crossY2 = circle.y;
//    }
//    else {
//        // 交点が2つ
//        *crossX1 = rect.left;
//        *crossY1 = circle.y + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossX2 = rect.left;
//        *crossY2 = circle.y - sqrtf(circle.radius * circle.radius - dist * dist);
//    }
//
//    // 右辺
//    dist = circle.x - rect.right;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // 交点が1つ
//        *crossX1 = rect.right;
//        *crossY1 = circle.y;
//        *crossX2 = rect.right;
//        *crossY2 = circle.y;
//    }
//    else {
//        // 交点が2つ
//        *crossX1 = rect.right;
//        *crossY1 = circle.y + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossX2 = rect.right;
//        *crossY2 = circle.y - sqrtf(circle.radius * circle.radius - dist * dist);
//    }
//
//    // 上辺
//    dist = rect.top - circle.y;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // 交点が1つ
//        *crossX1 = circle.x;
//        *crossY1 = rect.top;
//        *crossX2 = circle.x;
//        *crossY2 = rect.top;
//    }
//    else {
//        // 交点が2つ
//        *crossX1 = circle.x + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY1 = rect.top;
//        *crossX2 = circle.x - sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY2 = rect.top;
//    }
//
//    // 下辺
//    dist = circle.y - rect.bottom;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // 交点が1つ
//        *crossX1 = circle.x;
//        *crossY1 = rect.bottom;
//        *crossX2 = circle.x;
//        *crossY2 = rect.bottom;
//    }
//    else {
//        // 交点が2つ
//        *crossX1 = circle.x + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY1 = rect.bottom;
//        *crossX2 = circle.x - sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY2 = rect.bottom;
//    }
//
//    float segmentX = rect.right - rect.left;   // 線分X
//    float segmentY = 0;   // 線分Y
//    float vecX = circle.x - rect.left; // ベクトルX
//    float vecY = circle.y - rect.top;  // ベクトルY
//
//    float  segmentLength = sqrtf((segmentX * segmentX) + (segmentY * segmentY));    // 線分の始点(or終点)と円の中心との長さ
//
//    if (segmentLength != 0)     // ベクトルの正規化
//    {
//        segmentX /= segmentLength;
//        segmentY /= segmentLength;
//    }
//   
//    float crossProduct = segmentX * vecY - vecX * segmentY;     // 外積計算
//    if (crossProduct > circle.radius) return false;     // 上記計算から出された線分が円の半径より大きければfalse
//    else if (crossProduct == circle.radius) // trueの場合IsHitFan()に渡すための点X,Yを計算する
//    {
//        *crossX = circle.radius * cos(angle) + circle.x;
//        *crossY = circle.radius * sin(angle) + circle.y;
//        return true;
//    }
//    else if (crossProduct < circle.radius)
//    {
//      // *crossX = circle.x + cos(crossProduct);
//      // *crossY =  circle.y - sin(crossProduct);
//        return true;
//    }
//    return false;
//}

// 点が扇形の中にあるか判定
bool IsInFan(FAN& fan, float angle, VECTOR point)   // float fanox, float fanoy, float fanLength, float fanangle, float angle, 
{             // 扇の中心X、  扇の中心Y、   扇の半径、扇の範囲の角度(内角)、方向の角度、対象のX、     対象のY
	float fanVectorX = point.x - fan.x;  // 点と扇のベクトルX
	float fanVectorY = point.y - fan.y;  // 点と扇のベクトルY
	float fanVectorlength = sqrtf((fanVectorX * fanVectorX) + (fanVectorY * fanVectorY)); // 扇形と対象のベクトル計算

	if (fanVectorlength < fan.radius)  // 扇と対象のベクトルが扇の半径より小さければ進む
	{
		/*==========扇の範囲内に矩形の角などの点があった時の判定==========*/
		// 扇を2等分する線のベクトルを求める
		float cosCenter = cosf(angle);     // ベクトル成分に変化
		float sinCenter = sinf(angle);     // ベクトル成分に変化

		// 扇と点のベクトルを単位ベクトルにする
		float fanToPointVectorY = fanVectorY / fanVectorlength; // 点と扇の単位ベクトルY
		float fanToPointVectorX = fanVectorX / fanVectorlength; // 点と扇の単位ベクトルX

		// 内積計算
		float dot = (fanToPointVectorX * cosCenter) + (fanToPointVectorY * sinCenter);

		// 扇の範囲をcosにする
		float fan_cos = cosf(fan.angle * 3.14f / 180 / 2.0f);

		// 点が扇の範囲内にあるかを比較する
		if (fan_cos < dot) return true;  // 当たってる fabsf(fan_cos) > fabsf(dot)
		/*==============================================================*/       
	}
	return false;
}

// 矩形と扇形の当たり判定
bool IsHitRectWithFan(FRECT& rect, FAN& fan, float angle)
{
	/* ブロードフェーズ */
	FRECT rect2 = { fan.x - fan.radius, fan.y - fan.radius, fan.x + fan.radius, fan.y + fan.radius };
	if (!IsHit(&rect, &rect2))
		return false;

	/* ナローフェーズ */
	VECTOR leftTop = {rect.left, rect.top};
	VECTOR rightTop = {rect.right, rect.top};
	VECTOR leftBottom = {rect.left, rect.bottom};
	VECTOR rightBottom = {rect.right, rect.bottom};
	VECTOR fanCenter = {fan.x, fan.y};

	/*扇の中心が矩形の中にあるか*/
	if (rect.left <= fan.x && fan.x <= rect.right && rect.top <= fan.y && fan.y <= rect.bottom) return true;

	/*矩形の角が扇の中にあるか*/
	//左上
	if (IsInFan(fan, angle, leftTop)) return true;
	// 右上
	if (IsInFan(fan, angle, rightTop)) return true;
	// 左下
	if (IsInFan(fan, angle, leftBottom)) return true;
	// 右下
	if (IsInFan(fan, angle, rightBottom)) return true;

    /*矩形と扇が交差してるか*/
    float rad = fan.angle * 3.14f / 180;
    VECTOR fanA = { fan.x + cos(angle - rad / 2) * fan.radius, fan.y + sin(angle - rad / 2) * fan.radius };
    VECTOR fanB = { fan.x + cos(angle + rad / 2) * fan.radius, fan.y + sin(angle + rad / 2) * fan.radius };
    // 左辺
    if(IsCrossLine(leftTop, leftBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(leftTop, leftBottom, fanCenter, fanB)) return true;
    // 右辺
    if(IsCrossLine(rightTop, rightBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(rightTop, rightBottom, fanCenter, fanB)) return true;
    // 上辺
    if(IsCrossLine(leftTop, rightTop, fanCenter, fanA)) return true;
    if(IsCrossLine(leftTop, rightTop, fanCenter, fanB)) return true;
    // 下辺
    if(IsCrossLine(leftBottom, rightBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(leftBottom, rightBottom, fanCenter, fanB)) return true;
     
    return false;
}

// オブジェクト間に通行不能なタイルがあるかを判定
bool CheckBetween(OBJECT& self, OBJECT& target)
{
    // 扇の中心と矩形の中心のベクトル計算
    VECTOR vec = { target.x - self.x, target.y - self.y };
    float vecLength = sqrtf((vec.x * vec.x) + (vec.y * vec.y));

    // 単位ベクトル化
    VECTOR unitVector = { vec.x / vecLength, vec.y / vecLength };

    // ステージ関連読み込み
    const int* stage = GetStage();
    const int* hitTable = GetHitTable();

    for (int i = 1; i < vecLength / TILE_SIZE; i++) {
            VECTOR pointVec = { unitVector.x * TILE_SIZE * i, unitVector.y * TILE_SIZE * i }; 
            int stageX = (self.x + pointVec.x) / TILE_SIZE;
            int stageY = (self.y + pointVec.y) / TILE_SIZE;

            if (hitTable[stage[stageX+ stageY * STAGE_SIZE_X]] == 1)
            {
                return true;
            }
        }
    return false;
}
