#include "Ray.h"

Ray::Ray()
{
}

Ray::~Ray()
{
}

const DirectX::XMFLOAT3& Ray::getOrigin() const
{
	return m_origin;
}

DirectX::FXMVECTOR Ray::getOriginAsVector() const
{
	return DirectX::FXMVECTOR{ m_origin.x, m_origin.y ,m_origin.z , 1 };
}

const DirectX::XMFLOAT3& Ray::getDirection() const
{
	return m_direction;
}

DirectX::FXMVECTOR Ray::getDirectionAsVector() const
{
	DirectX::FXMVECTOR f = DirectX::XMVector3Normalize({ m_direction.x, m_direction.y , m_direction.z });
	return f;
}

void Ray::setOrigin(const DirectX::XMFLOAT3 & origin)
{
	isDirty = true;
	m_origin = origin;
}

void Ray::setOrigin(const DirectX::XMVECTOR & origin)
{
	isDirty = true;
	m_origin.x = origin.m128_f32[0];
	m_origin.y = origin.m128_f32[1];
	m_origin.z = origin.m128_f32[2];
}

void Ray::setDirection(const DirectX::XMFLOAT3 & direction)
{
	isDirty = true;
	m_direction = direction;
}

void Ray::setDirection(const DirectX::XMVECTOR & direction)
{
	isDirty = true;
	m_direction.x = direction.m128_f32[0];
	m_direction.y = direction.m128_f32[1];
	m_direction.z = direction.m128_f32[2];
}

void Ray::draw(ID3D11Device* device, ID3D11DeviceContext * context, VertexConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader, const DirectX::XMMATRIX & viewProjectionMatrix)
{
	float minX = -0.5f, minY = -0.5f, minZ = -0.5f;
	float maxX = 0.5f, maxY = 0.5f, maxZ = 0.5f;

	DirectX::XMFLOAT3 rayEndpoint = m_origin;

	DirectX::XMVECTOR normalizedDir = DirectX::XMVector3Normalize({ m_direction.x, m_direction.y , m_direction.z });
	rayEndpoint.x += normalizedDir.m128_f32[0];
	rayEndpoint.y += normalizedDir.m128_f32[1];
	rayEndpoint.z += normalizedDir.m128_f32[2];

	DirectX::XMVECTOR rightVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross({ m_direction.x, m_direction.y, m_direction.z }, { 0, 1, 0 }));
	rightVec = DirectX::XMVectorSubtract(rightVec, DirectX::XMVectorScale(normalizedDir, 4));
	rightVec = DirectX::XMVector3Normalize(rightVec);

	DirectX::XMVECTOR upVec = DirectX::XMVector3Cross(rightVec, normalizedDir);
	upVec = DirectX::XMVector3Normalize(upVec);

	rightVec = DirectX::XMVectorAdd(rightVec, DirectX::XMVectorScale(upVec, 0.25f));
	rightVec = DirectX::XMVector3Normalize(rightVec);
	rightVec = DirectX::XMVectorScale(rightVec, 0.15f);

	DirectX::XMVECTOR leftVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross({ m_direction.x, m_direction.y, m_direction.z }, { 0, -1, 0 }));
	leftVec = DirectX::XMVectorSubtract(leftVec, DirectX::XMVectorScale(normalizedDir, 4));
	leftVec = DirectX::XMVector3Normalize(leftVec);
	leftVec = DirectX::XMVectorAdd(leftVec, DirectX::XMVectorScale(upVec, 0.25f));
	leftVec = DirectX::XMVector3Normalize(leftVec);
	leftVec = DirectX::XMVectorScale(leftVec, 0.15f);

	DirectX::XMVECTOR topVec, bottomVec;
	topVec = DirectX::XMVector3Reflect(rightVec, upVec);
	bottomVec = DirectX::XMVector3Reflect(leftVec, upVec);

	std::vector<Vertex> obbVertices = { Vertex(m_origin.x, m_origin.y, m_origin.z, 0, 0, 0, 1, 0),
										Vertex(rayEndpoint.x, rayEndpoint.y, rayEndpoint.z, 0, 0, 0, 1, 0),
										Vertex(
											rayEndpoint.x + rightVec.m128_f32[0],
											rayEndpoint.y + rightVec.m128_f32[1],
											rayEndpoint.z + rightVec.m128_f32[2], 0, 0, 0, 1, 0),
										Vertex(
											rayEndpoint.x + leftVec.m128_f32[0],
											rayEndpoint.y + leftVec.m128_f32[1],
											rayEndpoint.z + leftVec.m128_f32[2], 0, 0, 0, 1, 0),
										Vertex(
											rayEndpoint.x + topVec.m128_f32[0],
											rayEndpoint.y + topVec.m128_f32[1],
											rayEndpoint.z + topVec.m128_f32[2], 0, 0, 0, 1, 0),
										Vertex(
											rayEndpoint.x + bottomVec.m128_f32[0],
											rayEndpoint.y + bottomVec.m128_f32[1],
											rayEndpoint.z + bottomVec.m128_f32[2], 0, 0, 0, 1, 0),
	};

	std::vector<DWORD> obbIndices = {
									0, 1,
									1, 2,
									1, 3,
									2, 3,
									1, 4,
									1, 5,
									4, 5,
									2, 4,
									3, 5
	};

	if (!hasInit)
	{
		hasInit = true;

		HRESULT hr = m_vertexBuffer.init(device, obbVertices.data(), obbVertices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize dynamic vertex buffer for Ray");
		hr = m_indexBuffer.init(device, obbIndices.data(), obbIndices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for Ray");
	}
	else
	{
		if (isDirty)
		{
			m_vertexBuffer.updateBuffer(context, obbVertices.data(), obbVertices.size());
			isDirty = false;
		}

		cb_vs_vertexShader.data.mvpMatrix = viewProjectionMatrix;
		cb_vs_vertexShader.data.worldMatrix = DirectX::XMMatrixIdentity();
		cb_vs_vertexShader.applyChanges();

		context->VSSetConstantBuffers(0, 1, cb_vs_vertexShader.getAddressOf());

		UINT offset = 0;
		context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, m_vertexBuffer.getAddressOf(), m_vertexBuffer.getStridePtr(), &offset);
		context->DrawIndexed(m_indexBuffer.getIndexCount(), 0, 0);
	}
}
