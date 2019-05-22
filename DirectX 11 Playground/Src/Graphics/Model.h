#pragma once
#include "Mesh.h"
#include "ModelInterface.h"

using namespace DirectX;

class Model : public ModelInterface<Vertex, CB_VS_VertexShader>
{
public:
	bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	bool init( std::vector<Vertex> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexShader)override;
	void setTexture(ID3D11ShaderResourceView* texture)override;
	void setTexture2(ID3D11ShaderResourceView* texture)
	{
		this->texture2 = texture;
	}
	void draw(const XMMATRIX& viewProjectionMatrix)override;

private:
	std::vector<Mesh> meshes;
	bool loadModel(const std::string & filePath);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);
	ID3D11ShaderResourceView* texture2 = nullptr;
};

