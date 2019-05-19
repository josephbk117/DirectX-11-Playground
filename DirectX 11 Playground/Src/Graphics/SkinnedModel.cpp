#include "SkinnedModel.h"
#include <iostream>
bool SkinnedModel::init(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * context, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader)
{
	this->device = device;
	this->context = context;
	this->texture = texture;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	try
	{
		if (!this->loadModel(filePath))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::log(exception);
		return false;
	}

	this->setPosition(0.0f, 0.0f, 0.0f);
	this->setRotation(0.0f, 0.0f, 0.0f);
	this->updateWorldMatrix();
	return true;
}

bool SkinnedModel::init(std::vector<SkinnedVertex> vertices, std::vector<DWORD> indices, ID3D11Device * device, ID3D11DeviceContext * context, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader)
{
	this->device = device;
	this->context = context;
	this->texture = texture;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	meshes.push_back(SkinnedMesh(this->device, this->context, vertices, indices));

	this->setPosition(0.0f, 0.0f, 0.0f);
	this->setRotation(0.0f, 0.0f, 0.0f);
	this->updateWorldMatrix();
	return true;
}

void SkinnedModel::setTexture(ID3D11ShaderResourceView * texture)
{
	this->texture = texture;
}

void SkinnedModel::draw(const XMMATRIX & viewProjectionMatrix)
{
	cb_vs_vertexShader->data.mvpMatrix = worldMatrix * viewProjectionMatrix;
	cb_vs_vertexShader->data.worldMatrix = worldMatrix;
	cb_vs_vertexShader->applyChanges();

	context->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->getAddressOf());
	context->PSSetShaderResources(0, 1, &texture);

	for (SkinnedMesh mesh : meshes)
		mesh.draw();
}

bool SkinnedModel::loadModel(const std::string & filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->processNode(pScene->mRootNode, pScene);
	return true;
}

void SkinnedModel::processNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene);
}

struct VertexSkinInfo
{
	unsigned int id = 0;
	float weights = 0.0f;
};

SkinnedMesh SkinnedModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// Data to fill
	std::vector<SkinnedVertex> vertices;
	std::vector<DWORD> indices;

	std::multimap<unsigned int, VertexSkinInfo> vertexIndexInfoMap;

	if (mesh->HasBones())
	{
		std::cout << "\nHas bones";
		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			std::cout << "\nBones at " << i;
			std::cout << "\n..." << mesh->mBones[i]->mName.C_Str();
		}
		std::cout << "\nNum of weights";
		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			std::cout << "\nNumber of vertices affected by bone " << mesh->mBones[i]->mNumWeights;
			for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				std::cout << ">> Vertex Id " << mesh->mBones[i]->mWeights[j].mVertexId;
				std::cout << ">> Weight " << mesh->mBones[i]->mWeights[j].mWeight;

				vertexIndexInfoMap.insert(std::pair<unsigned int, VertexSkinInfo>
					(mesh->mBones[i]->mWeights[j].mVertexId, VertexSkinInfo{ i, mesh->mBones[i]->mWeights[j].mWeight }));
			}
		}
	}
	
	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		SkinnedVertex vertex;

		vertex.pos.x = mesh->mVertices[i].x * 0.1f;
		vertex.pos.y = mesh->mVertices[i].y * 0.1f;
		vertex.pos.z = mesh->mVertices[i].z * 0.1f;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		auto itr1 = vertexIndexInfoMap.lower_bound(i);
		auto itr2 = vertexIndexInfoMap.upper_bound(i);

		std::cout << "\nVertex at index " << i;
		int count = 0;
		while (itr1 != itr2)
		{
			std::cout << "\n   Bone index : " << (*itr1).second.id << " weight :" << (*itr1).second.weights;
			if (count == 0)
			{
				vertex.jointIds.x = (*itr1).second.id;
				vertex.weights.x = (*itr1).second.weights;
			}
			else if (count == 1)
			{
				vertex.jointIds.y = (*itr1).second.id;
				vertex.weights.y = (*itr1).second.weights;
			}
			else if (count == 2)
			{
				vertex.jointIds.z = (*itr1).second.id;
				vertex.weights.z = (*itr1).second.weights;
			}
			count++;
			itr1++;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return SkinnedMesh(this->device, this->context, vertices, indices);
}
