#pragma once
#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader);
	void setTexture(ID3D11ShaderResourceView* texture);
	void draw(const XMMATRIX& viewProjectionMatrix);

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

private:
	std::vector<Mesh> meshes;

	bool loadModel(const std::string & filePath);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);
	void updateWorldMatrix();


	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ConstantBuffer<CB_VS_VertexShader> * cb_vs_vertexShader = nullptr;
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

