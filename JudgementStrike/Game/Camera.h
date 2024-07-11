#pragma once

struct CAMERA {
	float x, y;	// ���[���h���W
	float zoom;	// �Y�[���{��
};

void InitCamera();
void CameraFinalize();
void CameraUpdate();

const CAMERA* GetCamera();
void SetCameraXY(float x, float y);
void SetCameraZoom(float zoom);
