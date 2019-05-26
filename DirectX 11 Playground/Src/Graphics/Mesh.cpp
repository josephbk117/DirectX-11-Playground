#include "Mesh.h"
#include "Primitive3DModels.h"
Mesh::Mesh(ID3D11Device * device, ID3D11DeviceContext * context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices)
{
	this->deviceContext = context;
	HRESULT hr = vertexBuffer.init(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");
	hr = indexBuffer.init(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for mesh");

	float maxX = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
	float maxY = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
	float maxZ = std::max_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;

	float minX = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; })->pos.x;
	float minY = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; })->pos.y;
	float minZ = std::min_element(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; })->pos.z;

	obb.posExtent = { maxX, maxY, maxZ };
	obb.negExtent = { minX, minY, minZ };
	float ymid = (minY + maxY) * 0.5f;

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
	hr = obb.vertexBuffer.init(device, obbVertices.data(), obbVertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for OBB");
	hr = obb.indexBuffer.init(device, obbIndices.data(), obbIndices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for OBB");
}

Mesh::Mesh(const Mesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->obb = mesh.obb;
}

Mesh & Mesh::operator=(const Mesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->obb = mesh.obb;
	return *this;
}

void Mesh::draw()
{
	UINT offset = 0;
	deviceContext->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
}

void Mesh::drawOBB()
{
	obb.draw(deviceContext);
}
