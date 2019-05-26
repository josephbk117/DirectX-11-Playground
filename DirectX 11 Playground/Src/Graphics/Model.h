#pragma once
#include "Mesh.h"
#include "ModelInterface.h"
#include "Primitive3DModels.h"
using namespace DirectX;

//Oriented Bounding Box
class OBB
{
private:
	XMFLOAT3 negExtent = { 0, 0, 0 };
	XMFLOAT3 posExtent = { 0, 0, 0 };
	std::unique_ptr<Mesh> meshPtr = nullptr;
public:
	OBB() {};
	OBB(const OBB& ref) //TODO : Potential breaking bug, Deep copy not done correctly
	{
		negExtent = ref.negExtent;
		posExtent = ref.posExtent;

		//if (meshPtr.get() != nullptr)
		//{
		//	meshPtr.release();
		//	meshPtr = nullptr;
		//}

		if (ref.meshPtr.get() != nullptr)
		{
			Mesh* _mesh = reinterpret_cast<Mesh*>(new char[sizeof(Mesh)]);
			*_mesh = *(ref.meshPtr);
			*(meshPtr.get()) = *_mesh;
		}
	};
	OBB& operator=(const OBB& rhs)
	{
		negExtent = rhs.negExtent;
		posExtent = rhs.posExtent;

		if (meshPtr.get() != nullptr)
		{
			meshPtr.release();
			meshPtr = nullptr;
		}

		if (rhs.meshPtr != nullptr)
		{
			Mesh* _mesh = reinterpret_cast<Mesh*>(new char[sizeof(Mesh)]);
			*_mesh = *(rhs.meshPtr.get());
			*(meshPtr.get()) = *_mesh;
		}
		return *this;
	}
	~OBB() {};
	void init(ID3D11Device* device, ID3D11DeviceContext* context, const XMFLOAT3& negativeExtent, const XMFLOAT3& positiveExtent)
	{
		negExtent = negativeExtent;
		posExtent = positiveExtent;

		auto verts = Primitive3DModels::QUAD.vertices;
		auto indices = Primitive3DModels::QUAD.indices;

		//meshPtr = std::move(std::make_unique<Mesh>(device, context, verts, indices));
	}

	const XMFLOAT3& getNegativeExtent()const
	{
		return negExtent;
	}

	const XMFLOAT3& getPositiveExtent()const
	{
		return posExtent;
	}

	void draw()
	{
		//meshPtr->draw();
	}
};

class Model : public ModelInterface<Vertex, CB_VS_VertexShader>
{
public:
	Model() {};
	Model(const Model& rhs) = default;
	~Model() {};
	bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	bool init(std::vector<Vertex> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	void setTexture(ID3D11ShaderResourceView* texture)override;
	void setTexture2(ID3D11ShaderResourceView* texture)
	{
		this->texture2 = texture;
	}
	void draw(const XMMATRIX& viewProjectionMatrix)override;
	void drawOBB(const XMMATRIX& viewProjectionMatrix)
	{
		cb_vs_vertexShader->data.mvpMatrix = worldMatrix * viewProjectionMatrix;
		cb_vs_vertexShader->data.worldMatrix = worldMatrix;
		cb_vs_vertexShader->applyChanges();

		context->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->getAddressOf());
		context->PSSetShaderResources(0, 1, &texture1);
		if (texture2 != nullptr)
			context->PSSetShaderResources(1, 1, &texture2);

		//obb.draw();
	}
	OBB& getOBB()
	{
		//return obb;
	}

private:
	std::vector<Mesh> meshes;
	OBB obb;
	bool loadModel(const std::string & filePath);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);
	ID3D11ShaderResourceView* texture2 = nullptr;
};

