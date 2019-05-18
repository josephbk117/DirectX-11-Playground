#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "ConstantBuffer.h"
#include "Vertex.h"

using namespace DirectX;

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
class ModelInterface
{
public:
	virtual bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CONSTANT_BUFFER_STRUCT>& cb_vs_vertexShader) = 0;
	virtual bool init(std::vector<VERTEX_DATA_STRUCT> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CONSTANT_BUFFER_STRUCT>& cb_vs_vertexShader) = 0;
	virtual void setTexture(ID3D11ShaderResourceView* texture) = 0;
	virtual void draw(const XMMATRIX& viewProjectionMatrix) = 0;

	const XMVECTOR & getPositionVector() const;
	const XMFLOAT3 & getPositionFloat3() const;
	const XMVECTOR & getRotationVector() const;
	const XMFLOAT3 & getRotationFloat3() const;

	void setPosition(const XMVECTOR & pos);
	void setPosition(const XMFLOAT3 & pos);
	void setPosition(float x, float y, float z);
	void adjustPosition(const XMVECTOR & pos);
	void adjustPosition(const XMFLOAT3 & pos);
	void adjustPosition(float x, float y, float z);
	void setRotation(const XMVECTOR & rot);
	void setRotation(const XMFLOAT3 & rot);
	void setRotation(float x, float y, float z);
	void adjustRotation(const XMVECTOR & rot);
	void adjustRotation(const XMFLOAT3 & rot);
	void adjustRotation(float x, float y, float z);
	void setLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR & getForwardVector();
	const XMVECTOR & getRightVector();
	const XMVECTOR & getBackwardVector();
	const XMVECTOR & getLeftVector();

protected:

	virtual bool loadModel(const std::string & filePath) = 0;
	virtual void processNode(aiNode * node, const aiScene * scene) = 0;
	void updateWorldMatrix();

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ConstantBuffer<CONSTANT_BUFFER_STRUCT> * cb_vs_vertexShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
};


template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::updateWorldMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getPositionVector() const
{
	return this->posVector;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMFLOAT3 & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getPositionFloat3() const
{
	return this->pos;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getRotationVector() const
{
	return this->rotVector;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMFLOAT3 & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getRotationFloat3() const
{
	return this->rot;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setPosition(const XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustPosition(const XMFLOAT3 & pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setRotation(const XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustRotation(const XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::adjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->updateWorldMatrix();
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
void ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::setLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->setRotation(pitch, yaw, 0.0f);
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getForwardVector()
{
	return this->vec_forward;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getRightVector()
{
	return this->vec_right;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getBackwardVector()
{
	return this->vec_backward;
}

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
const XMVECTOR & ModelInterface<VERTEX_DATA_STRUCT, CONSTANT_BUFFER_STRUCT>::getLeftVector()
{
	return this->vec_left;
}
