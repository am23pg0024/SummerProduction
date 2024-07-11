#include "UnlimitedLib.h"

// �V�U�����O�̊J�n
void BeginScissor(int x, int y, int width, int height);
// �V�U�����O�̏I��
void EndScissor();



// �V�F�[�_�̓ǂݍ���
int LoadCustomShader(const char* pFilename, const char* pEntryPoint);
// �V�F�[�_�̉��
void ReleaseCustomShader(int index);

// �J�X�^���V�F�[�_�̊J�n
void BeginCustomShader(int index);
// �J�X�^���V�F�[�_�̏I��
void EndCustomShader();


// �V�F�[�_�p�����[�^�̐ݒ�
void SetCustomShaderParam(void* pParam, int size);