#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
class IndexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT indexCount = 0;
public:
	IndexBuffer() {}
	IndexBuffer(const IndexBuffer& rhs)
	{
		buffer = rhs.buffer;
		indexCount = rhs.indexCount;
	}
	IndexBuffer& operator= (const IndexBuffer& rhs)
	{
		if (buffer != nullptr)
			buffer.Reset();
		buffer = rhs.buffer;
		indexCount = rhs.indexCount;
		return *this;
	}
	ID3D11Buffer* get() const
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* getAddressOf() const
	{
		return buffer.GetAddressOf();
	}
	UINT getIndexCount() const
	{
		return indexCount;
	}
	HRESULT init(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		indexCount = numIndices;

		D3D11_BUFFER_DESC indicesBufferDesc;
		ZeroMemory(&indicesBufferDesc, sizeof(indicesBufferDesc));

		indicesBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indicesBufferDesc.ByteWidth = sizeof(DWORD) *numIndices;
		indicesBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indicesBufferDesc.CPUAccessFlags = 0;
		indicesBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indicesBufferData;
		ZeroMemory(&indicesBufferData, sizeof(indicesBufferData));
		indicesBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&indicesBufferDesc, &indicesBufferData, buffer.GetAddressOf());
		return hr;
	}
};