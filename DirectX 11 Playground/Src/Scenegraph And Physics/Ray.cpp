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
	DirectX::FXMVECTOR f = DirectX::XMVector3Normalize({ m_direction.x, m_direction.y , m_direction.z});
	return f;
}

void Ray::setOrigin(const DirectX::XMFLOAT3 & origin)
{
	isDirty = true;
	m_origin = origin;
}

void Ray::setDirection(const DirectX::XMFLOAT3 & direction)
{
	isDirty = true;
	m_direction = direction;
}

void Ray::draw(ID3D11Device* device,ID3D11DeviceContext * context, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader, const DirectX::XMMATRIX & viewProjectionMatrix)
{
	float minX = -0.5f, minY = -0.5f, minZ = -0.5f;
	float maxX = 0.5f, maxY = 0.5f, maxZ = 0.5f;

	DirectX::XMFLOAT3 rayEndpoint = m_origin;
	DirectX::FXMVECTOR f = DirectX::XMVector3Normalize({ m_direction.x, m_direction.y , m_direction.z });
	rayEndpoint.x += f.m128_f32[0];
	rayEndpoint.y += f.m128_f32[1];
	rayEndpoint.z += f.m128_f32[2];

	std::vector<Vertex> obbVertices = { Vertex(m_origin.x, m_origin.y, m_origin.z, 0, 0, 0, 1, 0),
										Vertex(rayEndpoint.x, rayEndpoint.y, rayEndpoint.z, 0, 0, 0, 1, 0),
									};

	std::vector<DWORD> obbIndices = {0,1};

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
