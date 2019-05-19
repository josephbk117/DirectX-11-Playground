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

XMMATRIX SkinnedModel::animate(float time)
{
	JointTransform A = tempBoneTransforms[2];
	JointTransform B = tempBoneTransforms[4];
	return JointTransform::interpolate(A, B, time).getLocalTransform();
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
	if (scene->HasAnimations())
	{
		unsigned int numberOfKeys = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
		for (unsigned int i = 0; i < numberOfKeys; i++)
		{
			DirectX::XMFLOAT3 pos;
			aiVector3D vec3 = scene->mAnimations[0]->mChannels[0]->mPositionKeys[i].mValue;
			pos.x = vec3.x;
			pos.y = vec3.y;
			pos.z = vec3.z;
			DirectX::XMVECTOR quat;
			aiQuaternion vec4 = scene->mAnimations[0]->mChannels[0]->mRotationKeys[i].mValue;
			quat.m128_f32[0] = vec4.x;
			quat.m128_f32[1] = vec4.y;
			quat.m128_f32[2] = vec4.z;
			quat.m128_f32[3] = vec4.w;
			JointTransform jT(pos, quat);
			tempBoneTransforms.push_back(jT);
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
