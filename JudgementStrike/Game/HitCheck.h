#pragma once

struct FRECT;
struct CIRCLE;
struct FAN;
struct VECTOR;
struct OBJECT;

// ��`���m�̔���
bool IsHit(FRECT* a, FRECT* b);
bool IsHitLeft(FRECT* self, FRECT* target);
bool IsHitRight(FRECT* self, FRECT* target);
bool IsHitTop(FRECT* self, FRECT* target);
bool IsHitBottom(FRECT* self, FRECT* target);

// �~���m�̔���
bool IsHitCircle(CIRCLE& a, CIRCLE& b);

// ��`�Ɖ~�̔���
bool IsHitRectWithCircle(FRECT& rect, CIRCLE& cir);
// �����̌�������
bool IsCrossLine(VECTOR& vec1, VECTOR& vec2, VECTOR& vec3, VECTOR& vec4);
// ��`�Ɛ�`�̓����蔻��
bool IsHitRectWithFan(FRECT& rect, FAN& fan, float angle);
// �I�u�W�F�N�g�Ԃɒʍs�s�\�ȃ^�C�������邩�𔻒�
bool CheckBetween(OBJECT& self, OBJECT& target);
