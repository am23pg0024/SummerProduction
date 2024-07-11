#pragma once

struct CAMERA {
	float x, y;	// ワールド座標
	float zoom;	// ズーム倍率
};

void InitCamera();
void CameraFinalize();
void CameraUpdate();

const CAMERA* GetCamera();
void SetCameraXY(float x, float y);
void SetCameraZoom(float zoom);
