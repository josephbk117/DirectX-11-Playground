#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>
template<class T>
class VertexBuffer
{
private:
	VertexBuffer(const VertexBuffer<T> &rhs);
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;
public:
	VertexBuffer() {}
	ID3D11Buffer* get() const
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* getAddressOf() const
	{
		return buffer.GetAddressOf();
	}
	UINT getBufferSize()const
	{
		return bufferSize;
	}
	const UINT getStride() const
	{
		return *stride.get();
	}
	const UINT* getStridePtr() const
	{
		return stride.get();
	}

	HRESULT init(ID3D11Device* device, T* data, UINT numVertices)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		bufferSize = numVertices;
		stride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, buffer.GetAddressOf());
		return hr;
	}
};
