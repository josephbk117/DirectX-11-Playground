#pragma once
#include "Mesh.h"
#include "ModelInterface.h"
#include "Primitive3DModels.h"
using namespace DirectX;

class Model : public BaseModel<Vertex, CB_VS_VertexShader>
{
public:
	Model() {};
	Model(const Model& rhs) = default;
	~Model() {};
	bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	bool init(std::vector<Vertex> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	void setTexture(ID3D11ShaderResourceView* texture)override;
	void setTexture2(ID3D11ShaderResourceView* texture)
	{
		this->texture2 = texture;
	}
	void draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)override;
	void drawDebugView(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	std::vector<Mesh>& getMeshes()
	{
		return meshes;
	}
private:
	std::vector<Mesh> meshes;
	bool loadModel(const std::string & filePath);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);
	ID3D11ShaderResourceView* texture2 = nullptr;
};

