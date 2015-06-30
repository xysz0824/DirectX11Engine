#ifndef _LOOKATCAMERA_H
#define _LOOKATCAMERA_H
#include <d3d11.h>
#include <xnamath.h>

enum class CameraType
{
	LookAt,
	LookTo
};
class Camera
{
public:
	void Init(XMFLOAT3 pos, XMFLOAT3 target, UINT viewportWidth, UINT viewportHeight, FLOAT nearZ, FLOAT farZ);
	void SetPosition(XMFLOAT3 pos) { _position = pos; }
	void SetTarget(XMFLOAT3 target) { _target = target; }
	void SetDirection(XMFLOAT3 direction) { _direction = direction; }
	void SetFovAngle(FLOAT fovAngle) { _fovAngle = fovAngle; }
	void SetAspectRatio(UINT viewportWidth, UINT viewportHeight) 
	{
		_aspectRatio = viewportHeight != 0 ? (float)viewportWidth / viewportHeight : _aspectRatio;
	}
	void SetFrustum(FLOAT nearZ, FLOAT farZ) 
	{ 
		_nearZ = nearZ;
		_farZ = farZ;
	}
	void Translate(XMFLOAT3 add);
	XMFLOAT3 GetPosition() { return _position; }
	XMFLOAT3 GetTarget() { return _target; }
	XMFLOAT3 GetDirection() { return _direction; }
	FLOAT GetFovAngle() { return _fovAngle; }
	XMMATRIX GetViewProjecitonMatrix();
private:
	CameraType		_type;
	XMFLOAT3		_position;
	XMFLOAT3		_target;
	XMFLOAT3		_direction;
	XMFLOAT3		_up;
	FLOAT				_fovAngle;
	FLOAT				_aspectRatio;
	FLOAT				_nearZ;
	FLOAT				_farZ;
};
#endif