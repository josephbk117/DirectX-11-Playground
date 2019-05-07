#include "Mesh.h"

Mesh::Mesh(ID3D11Device * device, ID3D11DeviceContext * context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices)
{
	this->deviceContext = context;
	HRESULT hr = vertexBuffer.init(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");
	hr = indexBuffer.init(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for mesh");
}

Mesh::Mesh(const Mesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
}

void Mesh::draw()
{
	UINT offset = 0;
	deviceContext->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
}
