#include "SkinnedMesh.h"
SkinnedMesh::SkinnedMesh(ID3D11Device * device, ID3D11DeviceContext * context, std::vector<SkinnedVertex>& vertices, std::vector<DWORD>& indices)
{
	this->deviceContext = context;
	HRESULT hr = vertexBuffer.init(device, vertices.data(), static_cast<UINT>(vertices.size()));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");
	hr = indexBuffer.init(device, indices.data(), static_cast<UINT>(indices.size()));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize indices buffer for mesh");
}

SkinnedMesh::SkinnedMesh(const SkinnedMesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
}

void SkinnedMesh::draw()
{
	UINT offset = 0;
	deviceContext->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	deviceContext->DrawIndexed(indexBuffer.getIndexCount(), 0, 0);
}
