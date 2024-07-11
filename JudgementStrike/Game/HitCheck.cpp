#include "Game.h"
#include "HitCheck.h"
#include "Stage.h"
#include "EnemyManager.h"
#include <math.h>
#include "Stage.h"

// ��`���m�̔���
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

// �~���m�̔���
bool IsHitCircle(CIRCLE& a, CIRCLE& b)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	float r = a.radius + b.radius;

	return x * x + y * y < r * r;
}

// ��`�Ɖ~�̔���
bool IsHitRectWithCircle(FRECT& rect, CIRCLE& cir)
{
	/* �u���[�h�t�F�[�Y */
	FRECT rect2 = { cir.x - cir.radius, cir.y - cir.radius, cir.x + cir.radius, cir.y + cir.radius };
	if (!IsHit(&rect, &rect2))
		return false;

	/* �i���[�t�F�[�Y */
	// �ӂ̔���
	if (rect.left <= cir.x && cir.x <= rect.right
		&& rect.top - cir.radius <= cir.y && cir.y <= rect.bottom + cir.radius)
		return true;
	if (rect.left - cir.radius <= cir.x && cir.x <= rect.right + cir.radius
		&& rect.top <= cir.y && cir.y <= rect.bottom)
		return true;

	// �p�̔���
	// ����
	float x = rect.left - cir.x;
	float y = rect.top - cir.y;
	float length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	// ����
	x = rect.left - cir.x;
	y = rect.bottom - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	// �E��
	x = rect.right - cir.x;
	y = rect.top - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;
	
	// �E��
	x = rect.right - cir.x;
	y = rect.bottom - cir.y;
	length = x * x + y * y;
	if (length <= cir.radius * cir.radius)
		return true;

	return false;
}

// �����̌�������
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
//	// �����Ƌ�`�̕ӂ��d�Ȃ��Ă���
//
//	// �d�Ȃ��ĂȂ���ΐ����̒[�_�̂����ǂ��炩����`�̓���
//	return false;
//}


//bool IsCrossLineWithCircle(FAN& circle, FRECT& rect, float angle, float* crossX1, float* crossY1, float* crossX2, float* crossY2)
//{
//    // ����
//    float dist = rect.left - circle.x;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // ��_��1��
//        *crossX1 = rect.left;
//        *crossY1 = circle.y;
//        *crossX2 = rect.left;
//        *crossY2 = circle.y;
//    }
//    else {
//        // ��_��2��
//        *crossX1 = rect.left;
//        *crossY1 = circle.y + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossX2 = rect.left;
//        *crossY2 = circle.y - sqrtf(circle.radius * circle.radius - dist * dist);
//    }
//
//    // �E��
//    dist = circle.x - rect.right;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // ��_��1��
//        *crossX1 = rect.right;
//        *crossY1 = circle.y;
//        *crossX2 = rect.right;
//        *crossY2 = circle.y;
//    }
//    else {
//        // ��_��2��
//        *crossX1 = rect.right;
//        *crossY1 = circle.y + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossX2 = rect.right;
//        *crossY2 = circle.y - sqrtf(circle.radius * circle.radius - dist * dist);
//    }
//
//    // ���
//    dist = rect.top - circle.y;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // ��_��1��
//        *crossX1 = circle.x;
//        *crossY1 = rect.top;
//        *crossX2 = circle.x;
//        *crossY2 = rect.top;
//    }
//    else {
//        // ��_��2��
//        *crossX1 = circle.x + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY1 = rect.top;
//        *crossX2 = circle.x - sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY2 = rect.top;
//    }
//
//    // ����
//    dist = circle.y - rect.bottom;
//    if (dist > circle.radius) {
//        return false;
//    }
//    else if (dist == circle.radius) {
//        // ��_��1��
//        *crossX1 = circle.x;
//        *crossY1 = rect.bottom;
//        *crossX2 = circle.x;
//        *crossY2 = rect.bottom;
//    }
//    else {
//        // ��_��2��
//        *crossX1 = circle.x + sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY1 = rect.bottom;
//        *crossX2 = circle.x - sqrtf(circle.radius * circle.radius - dist * dist);
//        *crossY2 = rect.bottom;
//    }
//
//    float segmentX = rect.right - rect.left;   // ����X
//    float segmentY = 0;   // ����Y
//    float vecX = circle.x - rect.left; // �x�N�g��X
//    float vecY = circle.y - rect.top;  // �x�N�g��Y
//
//    float  segmentLength = sqrtf((segmentX * segmentX) + (segmentY * segmentY));    // �����̎n�_(or�I�_)�Ɖ~�̒��S�Ƃ̒���
//
//    if (segmentLength != 0)     // �x�N�g���̐��K��
//    {
//        segmentX /= segmentLength;
//        segmentY /= segmentLength;
//    }
//   
//    float crossProduct = segmentX * vecY - vecX * segmentY;     // �O�όv�Z
//    if (crossProduct > circle.radius) return false;     // ��L�v�Z����o���ꂽ�������~�̔��a���傫�����false
//    else if (crossProduct == circle.radius) // true�̏ꍇIsHitFan()�ɓn�����߂̓_X,Y���v�Z����
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

// �_����`�̒��ɂ��邩����
bool IsInFan(FAN& fan, float angle, VECTOR point)   // float fanox, float fanoy, float fanLength, float fanangle, float angle, 
{             // ��̒��SX�A  ��̒��SY�A   ��̔��a�A��͈̔͂̊p�x(���p)�A�����̊p�x�A�Ώۂ�X�A     �Ώۂ�Y
	float fanVectorX = point.x - fan.x;  // �_�Ɛ�̃x�N�g��X
	float fanVectorY = point.y - fan.y;  // �_�Ɛ�̃x�N�g��Y
	float fanVectorlength = sqrtf((fanVectorX * fanVectorX) + (fanVectorY * fanVectorY)); // ��`�ƑΏۂ̃x�N�g���v�Z

	if (fanVectorlength < fan.radius)  // ��ƑΏۂ̃x�N�g������̔��a��菬������ΐi��
	{
		/*==========��͈͓̔��ɋ�`�̊p�Ȃǂ̓_�����������̔���==========*/
		// ���2����������̃x�N�g�������߂�
		float cosCenter = cosf(angle);     // �x�N�g�������ɕω�
		float sinCenter = sinf(angle);     // �x�N�g�������ɕω�

		// ��Ɠ_�̃x�N�g����P�ʃx�N�g���ɂ���
		float fanToPointVectorY = fanVectorY / fanVectorlength; // �_�Ɛ�̒P�ʃx�N�g��Y
		float fanToPointVectorX = fanVectorX / fanVectorlength; // �_�Ɛ�̒P�ʃx�N�g��X

		// ���όv�Z
		float dot = (fanToPointVectorX * cosCenter) + (fanToPointVectorY * sinCenter);

		// ��͈̔͂�cos�ɂ���
		float fan_cos = cosf(fan.angle * 3.14f / 180 / 2.0f);

		// �_����͈͓̔��ɂ��邩���r����
		if (fan_cos < dot) return true;  // �������Ă� fabsf(fan_cos) > fabsf(dot)
		/*==============================================================*/       
	}
	return false;
}

// ��`�Ɛ�`�̓����蔻��
bool IsHitRectWithFan(FRECT& rect, FAN& fan, float angle)
{
	/* �u���[�h�t�F�[�Y */
	FRECT rect2 = { fan.x - fan.radius, fan.y - fan.radius, fan.x + fan.radius, fan.y + fan.radius };
	if (!IsHit(&rect, &rect2))
		return false;

	/* �i���[�t�F�[�Y */
	VECTOR leftTop = {rect.left, rect.top};
	VECTOR rightTop = {rect.right, rect.top};
	VECTOR leftBottom = {rect.left, rect.bottom};
	VECTOR rightBottom = {rect.right, rect.bottom};
	VECTOR fanCenter = {fan.x, fan.y};

	/*��̒��S����`�̒��ɂ��邩*/
	if (rect.left <= fan.x && fan.x <= rect.right && rect.top <= fan.y && fan.y <= rect.bottom) return true;

	/*��`�̊p����̒��ɂ��邩*/
	//����
	if (IsInFan(fan, angle, leftTop)) return true;
	// �E��
	if (IsInFan(fan, angle, rightTop)) return true;
	// ����
	if (IsInFan(fan, angle, leftBottom)) return true;
	// �E��
	if (IsInFan(fan, angle, rightBottom)) return true;

    /*��`�Ɛ�������Ă邩*/
    float rad = fan.angle * 3.14f / 180;
    VECTOR fanA = { fan.x + cos(angle - rad / 2) * fan.radius, fan.y + sin(angle - rad / 2) * fan.radius };
    VECTOR fanB = { fan.x + cos(angle + rad / 2) * fan.radius, fan.y + sin(angle + rad / 2) * fan.radius };
    // ����
    if(IsCrossLine(leftTop, leftBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(leftTop, leftBottom, fanCenter, fanB)) return true;
    // �E��
    if(IsCrossLine(rightTop, rightBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(rightTop, rightBottom, fanCenter, fanB)) return true;
    // ���
    if(IsCrossLine(leftTop, rightTop, fanCenter, fanA)) return true;
    if(IsCrossLine(leftTop, rightTop, fanCenter, fanB)) return true;
    // ����
    if(IsCrossLine(leftBottom, rightBottom, fanCenter, fanA)) return true;
    if(IsCrossLine(leftBottom, rightBottom, fanCenter, fanB)) return true;
     
    return false;
}

// �I�u�W�F�N�g�Ԃɒʍs�s�\�ȃ^�C�������邩�𔻒�
bool CheckBetween(OBJECT& self, OBJECT& target)
{
    // ��̒��S�Ƌ�`�̒��S�̃x�N�g���v�Z
    VECTOR vec = { target.x - self.x, target.y - self.y };
    float vecLength = sqrtf((vec.x * vec.x) + (vec.y * vec.y));

    // �P�ʃx�N�g����
    VECTOR unitVector = { vec.x / vecLength, vec.y / vecLength };

    // �X�e�[�W�֘A�ǂݍ���
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
