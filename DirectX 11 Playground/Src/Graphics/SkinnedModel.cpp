#include "SkinnedModel.h"
#include <iostream>
bool SkinnedModel::init(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture,
	VertexConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc)
{
	this->device = device;
	this->context = context;
	this->texture1 = texture;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	try
	{
		if (!this->loadModel(filePath, vertexProcessFunc))
			return false;
	}
	catch (COMException& exception)
	{
		ErrorLogger::log(exception);
		return false;
	}
	return true;
}

bool SkinnedModel::init(std::vector<SkinnedVertex> vertices, std::vector<DWORD> indices, ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11ShaderResourceView* texture, VertexConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc)
{
	this->device = device;
	this->context = context;
	this->texture1 = texture;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	meshes.push_back(SkinnedMesh(this->device, this->context, vertices, indices));
	return true;
}

void SkinnedModel::setTexture(ID3D11ShaderResourceView* texture)
{
	this->texture1 = texture;
}

void SkinnedModel::setTexture2(ID3D11ShaderResourceView* texture)
{
	this->texture2 = texture;
}

void SkinnedModel::draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	cb_vs_vertexShader->data.mvpMatrix = worldMatrix * viewProjectionMatrix;
	cb_vs_vertexShader->data.worldMatrix = worldMatrix;
	cb_vs_vertexShader->applyChanges();

	context->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->getAddressOf());
	context->PSSetShaderResources(0, 1, &texture1);

	for (SkinnedMesh mesh : meshes)
		mesh.draw();
}

bool SkinnedModel::loadModel(const std::string& filePath, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
		return false;
	this->processNode(pScene->mRootNode, pScene, vertexProcessFunc);
	return true;
}

void SkinnedModel::processNode(aiNode* node, const aiScene* scene, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene, vertexProcessFunc));
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene, vertexProcessFunc);
}

struct VertexSkinInfo
{
	unsigned int id = 0;
	float weights = 0.0f;
};

SkinnedMesh SkinnedModel::processMesh(aiMesh* mesh, const aiScene* scene, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc)
{
	if (scene->HasAnimations())
	{
		unsigned int numberOfChannels = scene->mAnimations[0]->mNumChannels;
		tempBoneTransformCollection.reserve(numberOfChannels);

		for (unsigned int channelIndex = 0; channelIndex < numberOfChannels; channelIndex++)
		{
			aiNodeAnim* nodeAnim = scene->mAnimations[0]->mChannels[channelIndex];
			unsigned int numberOfKeys = nodeAnim->mNumPositionKeys;
			std::vector<JointTransform> jT(numberOfKeys);
			tempBoneTransformCollection.push_back(jT);
			nodeAnim->mNodeName;

			aiVectorKey* positionKeys = nodeAnim->mPositionKeys;
			aiQuatKey* quatKeys = nodeAnim->mRotationKeys;

			for (unsigned int i = 0; i < numberOfKeys; i++)
			{

				DirectX::XMFLOAT3 pos;
				aiVector3D vec3 = positionKeys[i].mValue;

				pos.x = vec3.x;
				pos.y = vec3.y;
				pos.z = vec3.z;
				DirectX::XMVECTOR quat;
				aiQuaternion vec4 = quatKeys[i].mValue;
				quat.m128_f32[0] = vec4.x;
				quat.m128_f32[1] = vec4.y;
				quat.m128_f32[2] = vec4.z;
				quat.m128_f32[3] = vec4.w;
				JointTransform jT(pos, quat);
				tempBoneTransformCollection[channelIndex].push_back(jT);
			}
		}
	}

	// Data to fill
	std::vector<SkinnedVertex> vertices;
	std::vector<DWORD> indices;

	std::multimap<unsigned int, VertexSkinInfo> vertexIndexInfoMap;

	if (mesh->HasBones())
	{
		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				vertexIndexInfoMap.insert(std::pair<unsigned int, VertexSkinInfo>
					(mesh->mBones[i]->mWeights[j].mVertexId, VertexSkinInfo{ i, mesh->mBones[i]->mWeights[j].mWeight }));
			}
		}
	}

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		SkinnedVertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (vertexProcessFunc != nullptr)
			vertexProcessFunc(vertex.pos);

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

		int count = 0;
		while (itr1 != itr2)
		{
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
