#include "Game.h"
#include "Camera.h"
#include "Animation.h"
#include "Player.h"

static CAMERA camera;

const CAMERA* GetCamera() { return &camera; }
void SetCameraXY(float x, float y) { camera.x = x; camera.y = y; }
void SetCameraZoom(float zoom) { camera.zoom = zoom; }

void InitCamera()
{
	camera.x = camera.y = 0;
	camera.zoom = 1.f;
}

void CameraFinalize()
{
}

void CameraUpdate()
{
	const PLAYER* p = GetPlayer();

	SetCameraXY(p->obj.x - SCREEN_WIDTH / 2, p->obj.y - SCREEN_HEIGHT / 2);
}
