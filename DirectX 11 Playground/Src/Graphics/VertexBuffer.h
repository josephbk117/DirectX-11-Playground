#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>
template<class T>
class VertexBuffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	UINT vertexCount = 0;
public:
	VertexBuffer() {}
	VertexBuffer(const VertexBuffer<T> &rhs)
	{
		buffer = rhs.buffer;
		vertexCount = rhs.vertexCount;
		stride = rhs.stride;
	}

	VertexBuffer<T> & operator= (const VertexBuffer<T>& vertexBuffer)
	{
		buffer = vertexBuffer.buffer;
		vertexCount = vertexBuffer.vertexCount;
		stride = vertexBuffer.stride;
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
	UINT getVertexCount()const
	{
		return vertexCount;
	}
	const UINT getStride() const
	{
		return stride;
	}
	const UINT* getStridePtr() const
	{
		return &stride;
	}

	HRESULT init(ID3D11Device* device, T* data, UINT numVertices)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		vertexCount = numVertices;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * numVertices;
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

template<class T>
class DynamicVertexBuffer : public VertexBuffer<T>
{
private:
	ID3D11Device* device = nullptr;
public:
	HRESULT init(ID3D11Device* device, T* data, UINT numVertices)
	{
		this->device = device;

		if (VertexBuffer<T>::buffer.Get() != nullptr)
			VertexBuffer<T>::buffer.Reset();

		VertexBuffer<T>::vertexCount = numVertices;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = VertexBuffer<T>::stride * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, VertexBuffer<T>::buffer.GetAddressOf());
		return hr;
	}

	void updateBuffer(ID3D11DeviceContext* context, T* data, UINT numVertices)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		context->Map(VertexBuffer<T>::buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, data, numVertices);
		context->Unmap(VertexBuffer<T>::buffer, 0);
	}
};