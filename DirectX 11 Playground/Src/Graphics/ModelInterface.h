#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "ConstantBuffer.h"
#include "Vertex.h"
#include "..\Scenegraph And Physics\Transform.h"

using namespace DirectX;

class ModelInterface
{
public:
	ModelInterface() = default;
	ModelInterface(const ModelInterface& rhs) = default;
	virtual ~ModelInterface() {}
	virtual void setTexture(ID3D11ShaderResourceView* texture) = 0;
	virtual void draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix) = 0;
protected:
	virtual bool loadModel(const std::string & filePath) = 0;
	virtual void processNode(aiNode * node, const aiScene * scene) = 0;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11ShaderResourceView* texture1 = nullptr;
};

template<class VERTEX_DATA_STRUCT, class CONSTANT_BUFFER_STRUCT>
class BaseModel : public ModelInterface
{
public:
	BaseModel() = default;
	BaseModel(const BaseModel& rhs) = default;
	virtual ~BaseModel() {}
	virtual bool init(const std::string& filePath, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CONSTANT_BUFFER_STRUCT>& cb_vs_vertexShader) = 0;
	virtual bool init(std::vector<VERTEX_DATA_STRUCT> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CONSTANT_BUFFER_STRUCT>& cb_vs_vertexShader) = 0;
protected:

	virtual bool loadModel(const std::string & filePath) = 0;
	virtual void processNode(aiNode * node, const aiScene * scene) = 0;
	VertexConstantBuffer<CONSTANT_BUFFER_STRUCT> * cb_vs_vertexShader = nullptr;
};