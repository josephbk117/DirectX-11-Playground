#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#include "ConstantBufferTypes.h"
#include "../ErrorLogger.h"

class BaseConstantBuffer
{
public:
	BaseConstantBuffer() {}
	virtual ~BaseConstantBuffer() {}
	virtual ID3D11Buffer* get() const = 0;
	virtual ID3D11Buffer* const* getAddressOf()const = 0;
	virtual HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
	virtual bool applyChanges() = 0;
};

class BaseVertexConstantBuffer : public BaseConstantBuffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext = nullptr;
public:
	BaseVertexConstantBuffer() {}
	ID3D11Buffer* get() const final
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* getAddressOf() const final
	{
		return buffer.GetAddressOf();
	}
	virtual HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
	virtual bool applyChanges() = 0;
};

class BasePixelConstantBuffer : public BaseConstantBuffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext = nullptr;
public:
	BasePixelConstantBuffer() {}
	ID3D11Buffer* get() const final
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* getAddressOf() const final
	{
		return buffer.GetAddressOf();
	}
	virtual HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
	virtual bool applyChanges() = 0;
};

template<class T>
class VertexConstantBuffer : public BaseVertexConstantBuffer
{
private:
	VertexConstantBuffer(const VertexConstantBuffer& rhs);
public:
	VertexConstantBuffer() {}
	T data;

	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();
		this->deviceContext = deviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - sizeof(T) % 16));
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());

		return hr;
	}

	bool applyChanges() override
	{
		D3D11_MAPPED_SUBRESOURCE mappedresource;
		HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
		if (FAILED(hr))
		{
			ErrorLogger::log(hr, "Could not bind constant buffer");
			return false;
		}
		CopyMemory(mappedresource.pData, &data, sizeof(T));
		deviceContext->Unmap(buffer.Get(), 0);
		deviceContext->VSSetConstantBuffers(0, 1, buffer.GetAddressOf());
		return true;
	}
};


template<class T>
class PixelConstantBuffer : public BasePixelConstantBuffer
{
private:
	PixelConstantBuffer(const PixelConstantBuffer& rhs);
public:
	PixelConstantBuffer() {}
	T data;

	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();
		this->deviceContext = deviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - sizeof(T) % 16));
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());

		return hr;
	}

	bool applyChanges() override
	{
		D3D11_MAPPED_SUBRESOURCE mappedresource;
		HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
		if (FAILED(hr))
		{
			ErrorLogger::log(hr, "Could not bind constant buffer");
			return false;
		}
		CopyMemory(mappedresource.pData, &data, sizeof(T));
		deviceContext->Unmap(buffer.Get(), 0);
		deviceContext->PSSetConstantBuffers(0, 1, buffer.GetAddressOf());
		return true;
	}
};