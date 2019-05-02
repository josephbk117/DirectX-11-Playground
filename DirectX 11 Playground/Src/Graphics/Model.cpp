#include "Model.h"

bool Model::init(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)
{
	this->device = device;
	this->context = context;
	this->texture = texture;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	try
	{
		//Textured Square
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};

		//Load Vertex Data
		HRESULT hr = this->vertexBuffer.init(this->device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");

		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};

		//Load Index Data
		hr = this->indexBuffer.init(this->device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::log(exception);
		return false;
	}

	this->updateWorldMatrix();
	return true;
}

void Model::setTexture(ID3D11ShaderResourceView * texture)
{
	this->texture = texture;
}

void Model::draw(const XMMATRIX & viewProjectionMatrix)
{
	cb_vs_vertexShader->data.mat = worldMatrix * viewProjectionMatrix;
	cb_vs_vertexShader->data.mat = XMMatrixTranspose(cb_vs_vertexShader->data.mat);

	cb_vs_vertexShader->applyChanges();

	context->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->getAddressOf());
	context->PSSetShaderResources(0, 1, &texture);
	context->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.getStridePtr(), &offset);
	context->DrawIndexed(indexBuffer.getBufferSize(), 0, 0);
}

void Model::updateWorldMatrix()
{
	worldMatrix = XMMatrixIdentity();
}
