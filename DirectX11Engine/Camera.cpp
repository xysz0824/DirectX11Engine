#include "Camera.h"

void Camera::Init(XMFLOAT3 pos, XMFLOAT3 target, UINT width, UINT height, FLOAT nearZ, FLOAT farZ)
{
	_fovAngle = XM_PIDIV4;
	_aspectRatio = (float)width / height;
	_nearZ = nearZ;
	_farZ = farZ;
	this->SetPosition(pos);
	this->SetTarget(target);
	_up = XMFLOAT3(0, 1, 0);
}

void Camera::Translate(XMFLOAT3 add)
{
	XMVECTOR originV = XMLoadFloat3(&_position);
	XMVECTOR addV = XMLoadFloat3(&add);
	XMStoreFloat3(&_position, originV + addV);
}

XMMATRIX Camera::GetViewProjecitonMatrix()
{
	XMMATRIX viewMatrix = XMMatrixLookAtLH(
		XMLoadFloat3(&_position), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
	XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(_fovAngle, _aspectRatio, _nearZ, _farZ);
	//It must be transposed so that it can be set to constant buffer and get right result.
	return XMMatrixTranspose(XMMatrixMultiply(viewMatrix, projMatrix));
}