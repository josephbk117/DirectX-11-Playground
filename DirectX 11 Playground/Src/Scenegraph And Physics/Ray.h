#pragma once
#include "..\Graphics\Vertex.h"
#include "..\Graphics\VertexBuffer.h"
#include "..\Graphics\IndexBuffer.h"
#include "..\Graphics\ConstantBuffer.h"
#include <DirectXMath.h>

class Ray
{
private:
	DirectX::XMFLOAT3 m_origin;
	DirectX::XMFLOAT3 m_direction;

	DynamicVertexBuffer<Vertex> m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	bool hasInit = false;
	bool isDirty = true;
public:
	Ray();
	~Ray();
	Ray(const Ray& ref) = default;
	Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : m_origin(origin), m_direction(direction) {};
	const DirectX::XMFLOAT3& getOrigin()const;
	DirectX::FXMVECTOR getOriginAsVector()const;
	const DirectX::XMFLOAT3& getDirection()const;
	DirectX::FXMVECTOR getDirectionAsVector()const;
	void setOrigin(const DirectX::XMFLOAT3& origin);
	void setDirection(const DirectX::XMFLOAT3& direction);

	void draw(ID3D11Device* device,ID3D11DeviceContext * context, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader, const DirectX::XMMATRIX & viewProjectionMatrix);
};