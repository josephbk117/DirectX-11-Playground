#include "OBB.h"

void OBB::init(ID3D11Device * device, std::vector<Vertex>& vertices)
{
	{
		float maxX = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
		float maxY = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
		float maxZ = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;

		float minX = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
		float minY = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
		float minZ = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;

		m_posExtent = { maxX, maxY, maxZ };
		m_negExtent = { minX, minY, minZ };

		std::vector<Vertex> obbVertices = { Vertex(minX, minY, minZ, 0, 0, 0, 1, 0),
											Vertex(minX, minY, maxZ, 0, 0, 0, 1, 0),
											Vertex(maxX, minY, maxZ, 0, 0, 0, 1, 0),
											Vertex(maxX, minY, minZ, 0, 0, 0, 1, 0),

											Vertex(minX, maxY, minZ, 0, 0, 0, 1, 0),
											Vertex(minX, maxY, maxZ, 0, 0, 0, 1, 0),
											Vertex(maxX, maxY, maxZ, 0, 0, 0, 1, 0),
											Vertex(maxX, maxY, minZ, 0, 0, 0, 1, 0) };

		std::vector<DWORD> obbIndices = {
										0,1,
										1,2,
										2,3,
										3,0,

										4,5,
										5,6,
										6,7,
										7,4,

										0,4,
										1,5,
										2,6,
										3,7
		};
		HRESULT hr = m_vertexBuffer.init(device, obbVertices.data(), obbVertices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for OBB");
		hr = m_indexBuffer.init(device, obbIndices.data(), obbIndices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for OBB");

		XMFLOAT3 extents[2] = { m_negExtent, m_posExtent };
		BoundingOrientedBox::CreateFromPoints(m_boundOrientedBox, 2, extents, 3 * sizeof(float));
	}
}

bool OBB::doesRayIntersect(const Ray & ray)const
{
	float dist = 0.0f;
	return m_boundOrientedBox.Intersects(ray.getOriginAsVector(), ray.getDirectionAsVector(), dist);
}

void OBB::draw(ID3D11DeviceContext * context) const
{
	UINT offset = 0;
	context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.getAddressOf(), m_vertexBuffer.getStridePtr(), &offset);
	context->DrawIndexed(m_indexBuffer.getIndexCount(), 0, 0);
}